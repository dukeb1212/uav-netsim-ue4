#include "PIPCamera.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Camera/CameraComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "UAVNetSim/DataStruct/VideoFrameTrack.h"
#include "UAVNetSim/DataStruct/Flow.h"
#include "Engine/World.h"
#include "ImageUtils.h"
#include "Rendering/Texture2DResource.h"

#include <string>
#include <exception>
#include "AirBlueprintLib.h"

//CinemAirSim
APIPCamera::APIPCamera(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer
                .SetDefaultSubobjectClass<UCineCameraComponent>(TEXT("CameraComponent")))
{
    static ConstructorHelpers::FObjectFinder<UMaterial> mat_finder(TEXT("Material'/AirSim/HUDAssets/CameraSensorNoise.CameraSensorNoise'"));
    if (mat_finder.Succeeded()) {
        noise_material_static_ = mat_finder.Object;
    }
    else
        UAirBlueprintLib::LogMessageString("Cannot create noise material for the PIPCamera",
                                           "",
                                           LogDebugLevel::Failure);

    static ConstructorHelpers::FObjectFinder<UMaterial> dist_mat_finder(TEXT("Material'/AirSim/HUDAssets/CameraDistortion.CameraDistortion'"));
    if (dist_mat_finder.Succeeded()) {
        distortion_material_static_ = dist_mat_finder.Object;
        distortion_param_collection_ = Cast<UMaterialParameterCollection>(StaticLoadObject(UMaterialParameterCollection::StaticClass(), NULL, TEXT("'/AirSim/HUDAssets/DistortionParams.DistortionParams'")));
    }
    else
        UAirBlueprintLib::LogMessageString("Cannot create distortion material for the PIPCamera",
                                           "",
                                           LogDebugLevel::Failure);

	static ConstructorHelpers::FObjectFinder<UMaterial> PacketLossMatFinder(TEXT("Material'/AirSim/HUDAssets/Network/M_PacketLossEffect.M_PacketLossEffect'"));
    if (PacketLossMatFinder.Succeeded()) {
        PacketLossMaterialStatic = PacketLossMatFinder.Object;
		PacketLossParamCollection = Cast<UMaterialParameterCollection>(StaticLoadObject(UMaterialParameterCollection::StaticClass(), NULL, TEXT("'/AirSim/HUDAssets/Network/PacketLossEffectParams.PacketLossEffectParams'")));
	}
	else
		UAirBlueprintLib::LogMessageString("Cannot create packet loss material for the PIPCamera",
			"",
			LogDebugLevel::Failure);

    PrimaryActorTick.bCanEverTick = true;

    image_type_to_pixel_format_map_.Add(Utils::toNumeric(ImageType::Scene), EPixelFormat::PF_B8G8R8A8);
    image_type_to_pixel_format_map_.Add(Utils::toNumeric(ImageType::DepthPlanar), EPixelFormat::PF_DepthStencil); // not used. init_auto_format is called in setupCameraFromSettings()
    image_type_to_pixel_format_map_.Add(Utils::toNumeric(ImageType::DepthPerspective), EPixelFormat::PF_DepthStencil); // not used for same reason as above
    image_type_to_pixel_format_map_.Add(Utils::toNumeric(ImageType::DepthVis), EPixelFormat::PF_DepthStencil); // not used for same reason as above
    image_type_to_pixel_format_map_.Add(Utils::toNumeric(ImageType::DisparityNormalized), EPixelFormat::PF_DepthStencil); // not used for same reason as above
    image_type_to_pixel_format_map_.Add(Utils::toNumeric(ImageType::Segmentation), EPixelFormat::PF_B8G8R8A8);
    image_type_to_pixel_format_map_.Add(Utils::toNumeric(ImageType::SurfaceNormals), EPixelFormat::PF_B8G8R8A8);
    image_type_to_pixel_format_map_.Add(Utils::toNumeric(ImageType::Infrared), EPixelFormat::PF_B8G8R8A8);
    image_type_to_pixel_format_map_.Add(Utils::toNumeric(ImageType::OpticalFlow), EPixelFormat::PF_B8G8R8A8);
    image_type_to_pixel_format_map_.Add(Utils::toNumeric(ImageType::OpticalFlowVis), EPixelFormat::PF_B8G8R8A8);

    object_filter_ = FObjectFilter();
}

void APIPCamera::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    //CinemAirSim
    camera_ = UAirBlueprintLib::GetActorComponent<UCineCameraComponent>(this, TEXT("CameraComponent"));
    captures_.Init(nullptr, imageTypeCount());
    render_targets_.Init(nullptr, imageTypeCount());
    detections_.Init(nullptr, imageTypeCount());

    captures_[Utils::toNumeric(ImageType::Scene)] =
        UAirBlueprintLib::GetActorComponent<USceneCaptureComponent2D>(this, TEXT("SceneCaptureComponent"));
    captures_[Utils::toNumeric(ImageType::DepthPlanar)] =
        UAirBlueprintLib::GetActorComponent<USceneCaptureComponent2D>(this, TEXT("DepthPlanarCaptureComponent"));
    captures_[Utils::toNumeric(ImageType::DepthPerspective)] =
        UAirBlueprintLib::GetActorComponent<USceneCaptureComponent2D>(this, TEXT("DepthPerspectiveCaptureComponent"));
    captures_[Utils::toNumeric(ImageType::DepthVis)] =
        UAirBlueprintLib::GetActorComponent<USceneCaptureComponent2D>(this, TEXT("DepthVisCaptureComponent"));
    captures_[Utils::toNumeric(ImageType::DisparityNormalized)] =
        UAirBlueprintLib::GetActorComponent<USceneCaptureComponent2D>(this, TEXT("DisparityNormalizedCaptureComponent"));
    captures_[Utils::toNumeric(ImageType::Segmentation)] =
        UAirBlueprintLib::GetActorComponent<USceneCaptureComponent2D>(this, TEXT("SegmentationCaptureComponent"));
    captures_[Utils::toNumeric(ImageType::Infrared)] =
        UAirBlueprintLib::GetActorComponent<USceneCaptureComponent2D>(this, TEXT("InfraredCaptureComponent"));
    captures_[Utils::toNumeric(ImageType::SurfaceNormals)] =
        UAirBlueprintLib::GetActorComponent<USceneCaptureComponent2D>(this, TEXT("NormalsCaptureComponent"));
    captures_[Utils::toNumeric(ImageType::OpticalFlow)] =
        UAirBlueprintLib::GetActorComponent<USceneCaptureComponent2D>(this, TEXT("OpticalFlowCaptureComponent"));
    captures_[Utils::toNumeric(ImageType::OpticalFlowVis)] =
        UAirBlueprintLib::GetActorComponent<USceneCaptureComponent2D>(this, TEXT("OpticalFlowVisCaptureComponent"));

    for (unsigned int i = 0; i < imageTypeCount(); ++i) {
        detections_[i] = NewObject<UDetectionComponent>(this);
        if (detections_[i]) {
            detections_[i]->SetupAttachment(captures_[i]);
            detections_[i]->RegisterComponent();
            detections_[i]->Deactivate();
        }
    }
    //set initial focal length
    camera_->CurrentFocalLength = 11.9;

    
    VideoFrameTracker = GetWorld()->GetGameState<AVideoFrameTracker>();

}

void APIPCamera::BeginPlay()
{
    Super::BeginPlay();

    noise_materials_.AddZeroed(imageTypeCount() + 1);
    distortion_materials_.AddZeroed(imageTypeCount() + 1);
	PacketLossMaterials.AddZeroed(imageTypeCount() + 1);

    //by default all image types are disabled
    camera_type_enabled_.assign(imageTypeCount(), false);

    for (unsigned int image_type = 0; image_type < imageTypeCount(); ++image_type) {
        //use final color for all calculations
        captures_[image_type]->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

        render_targets_[image_type] = NewObject<UTextureRenderTarget2D>();
    }

    //We set all cameras to start as nodisplay
    //This improves performance because the capture components are no longer updating every frame and only update while requesting an image
    onViewModeChanged(true);

    gimbal_stabilization_ = 0;
    gimbald_rotator_ = this->GetActorRotation();
    this->SetActorTickEnabled(false);

    if (distortion_param_collection_)
        distortion_param_instance_ = this->GetWorld()->GetParameterCollectionInstance(distortion_param_collection_);

    if (PacketLossParamCollection)
        PacketLossParamInstance = this->GetWorld()->GetParameterCollectionInstance(PacketLossParamCollection);

    NetworkEffectManager = GetGameInstance()->GetSubsystem<UNetworkEffectManager>();
}

msr::airlib::ProjectionMatrix APIPCamera::getProjectionMatrix(const APIPCamera::ImageType image_type) const
{
    msr::airlib::ProjectionMatrix mat;

    //TODO: avoid the need to override const cast here
    const_cast<APIPCamera*>(this)->setCameraTypeEnabled(image_type, true);
    const USceneCaptureComponent2D* capture = const_cast<APIPCamera*>(this)->getCaptureComponent(image_type, false);
    if (capture) {
        FMatrix proj_mat_transpose;

        FIntPoint render_target_size(capture->TextureTarget->GetSurfaceWidth(), capture->TextureTarget->GetSurfaceHeight());
        float x_axis_multiplier = 1.0f;
        float y_axis_multiplier = render_target_size.X / (float)render_target_size.Y;

        if (render_target_size.X < render_target_size.Y) {
            // if the viewport is taller than it is wide
            x_axis_multiplier = render_target_size.Y / static_cast<float>(render_target_size.X);
            y_axis_multiplier = 1.0f;
        }

        if (capture->ProjectionType == ECameraProjectionMode::Orthographic) {
            check((int32)ERHIZBuffer::IsInverted);
            const float OrthoWidth = capture->OrthoWidth / 2.0f;
            const float OrthoHeight = capture->OrthoWidth / 2.0f * x_axis_multiplier / y_axis_multiplier;

            const float NearPlane = 0;
            const float FarPlane = WORLD_MAX / 8.0f;

            const float ZScale = 1.0f / (FarPlane - NearPlane);
            const float ZOffset = -NearPlane;

            proj_mat_transpose = FReversedZOrthoMatrix(
                OrthoWidth,
                OrthoHeight,
                ZScale,
                ZOffset);
        }
        else {
            float halfFov = Utils::degreesToRadians(capture->FOVAngle) / 2;
            if ((int32)ERHIZBuffer::IsInverted) {
                proj_mat_transpose = FReversedZPerspectiveMatrix(
                    halfFov,
                    halfFov,
                    x_axis_multiplier,
                    y_axis_multiplier,
                    GNearClippingPlane,
                    GNearClippingPlane);
            }
            else {
                //The FPerspectiveMatrix() constructor actually returns the transpose of the perspective matrix.
                proj_mat_transpose = FPerspectiveMatrix(
                    halfFov,
                    halfFov,
                    x_axis_multiplier,
                    y_axis_multiplier,
                    GNearClippingPlane,
                    GNearClippingPlane);
            }
        }

        //Takes a vector from NORTH-EAST-DOWN coordinates (AirSim) to EAST-UP-SOUTH coordinates (Unreal). Leaves W coordinate unchanged.
        FMatrix coordinateChangeTranspose = FMatrix(
            FPlane(0, 0, -1, 0),
            FPlane(1, 0, 0, 0),
            FPlane(0, -1, 0, 0),
            FPlane(0, 0, 0, 1));

        FMatrix projMatTransposeInAirSim = coordinateChangeTranspose * proj_mat_transpose;

        //Copy the result to an airlib::ProjectionMatrix while taking transpose.
        for (auto row = 0; row < 4; ++row)
            for (auto col = 0; col < 4; ++col)
                mat.matrix[col][row] = projMatTransposeInAirSim.M[row][col];
    }
    else
        mat.setTo(Utils::nan<float>());

    return mat;
}

void APIPCamera::Tick(float DeltaTime)
{
    if (gimbal_stabilization_ > 0) {
        FRotator rotator = this->GetActorRotation();
        if (!std::isnan(gimbald_rotator_.Pitch))
            rotator.Pitch = gimbald_rotator_.Pitch * gimbal_stabilization_ +
                            rotator.Pitch * (1 - gimbal_stabilization_);
        if (!std::isnan(gimbald_rotator_.Roll))
            rotator.Roll = gimbald_rotator_.Roll * gimbal_stabilization_ +
                           rotator.Roll * (1 - gimbal_stabilization_);
        if (!std::isnan(gimbald_rotator_.Yaw))
            rotator.Yaw = gimbald_rotator_.Yaw * gimbal_stabilization_ +
                          rotator.Yaw * (1 - gimbal_stabilization_);

        this->SetActorRotation(rotator);
    }

}

void APIPCamera::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Callback.RemoveAll(TargetWidget);
    if (noise_materials_.Num()) {
        for (unsigned int image_type = 0; image_type < imageTypeCount(); ++image_type) {
            if (noise_materials_[image_type + 1])
                captures_[image_type]->PostProcessSettings.RemoveBlendable(noise_materials_[image_type + 1]);
        }
        if (noise_materials_[0])
            camera_->PostProcessSettings.RemoveBlendable(noise_materials_[0]);
    }

    noise_material_static_ = nullptr;
    noise_materials_.Empty();

    if (distortion_materials_.Num()) {
        for (unsigned int image_type = 0; image_type < imageTypeCount(); ++image_type) {
            if (distortion_materials_[image_type + 1])
                captures_[image_type]->PostProcessSettings.RemoveBlendable(distortion_materials_[image_type + 1]);
        }
        if (distortion_materials_[0])
            camera_->PostProcessSettings.RemoveBlendable(distortion_materials_[0]);
    }

    distortion_material_static_ = nullptr;
    distortion_materials_.Empty();

    for (unsigned int image_type = 0; image_type < imageTypeCount(); ++image_type) {
        //use final color for all calculations
        captures_[image_type] = nullptr;
        render_targets_[image_type] = nullptr;
        detections_[image_type] = nullptr;
    }
}

unsigned int APIPCamera::imageTypeCount()
{
    return Utils::toNumeric(ImageType::Count);
}

void APIPCamera::showToScreen()
{
    camera_->SetVisibility(true);
    camera_->Activate();
    APlayerController* controller = this->GetWorld()->GetFirstPlayerController();
    controller->SetViewTarget(this);
    UAirBlueprintLib::LogMessage(TEXT("Camera: "), GetName(), LogDebugLevel::Informational);
}

void APIPCamera::disableAll()
{
    disableMain();
    disableAllPIP();
}

bool APIPCamera::getCameraTypeEnabled(ImageType type) const
{
    return camera_type_enabled_[Utils::toNumeric(type)];
}

void APIPCamera::setCameraTypeEnabled(ImageType type, bool enabled)
{
    enableCaptureComponent(type, enabled);
    int image_type = static_cast<int>(Utils::toNumeric(type));
    setPacketLossEffectMaterial(image_type, captures_[image_type], captures_[image_type]->PostProcessSettings);
}

void APIPCamera::setCaptureUpdate(USceneCaptureComponent2D* capture, bool nodisplay)
{
    capture->bCaptureEveryFrame = !nodisplay;
    capture->bCaptureOnMovement = !nodisplay;
    capture->bAlwaysPersistRenderingState = true;
}

void APIPCamera::setCameraTypeUpdate(ImageType type, bool nodisplay)
{
    USceneCaptureComponent2D* capture = getCaptureComponent(type, false);
    if (capture != nullptr)
    {
        //setCaptureUpdate(capture, nodisplay);
        capture->bCaptureEveryFrame = nodisplay;
        capture->bCaptureOnMovement = nodisplay;
        capture->bAlwaysPersistRenderingState = true;

        if (!nodisplay) // If rendering is enabled
        {
            float CaptureInterval = 1.0f / FMath::Clamp(FPS, 5.0f, 60.0f); // Avoid 0 FPS
            LastCaptureFrame = 0;
			CurrentQueueFrame = 0;
            GetWorld()->GetTimerManager().SetTimer(CaptureTimerHandle, this, &APIPCamera::CaptureFrame, CaptureInterval, false);
        }
        else // If rendering is disabled
        {
            GetWorld()->GetTimerManager().ClearTimer(CaptureTimerHandle);
            LastCaptureFrame = 0;
			CurrentQueueFrame = 0;
        }
    }
}

void APIPCamera::setCameraPose(const msr::airlib::Pose& relative_pose)
{
    FTransform pose = ned_transform_->fromRelativeNed(relative_pose);

    FVector position = pose.GetLocation();
    this->SetActorRelativeLocation(pose.GetLocation());

    FRotator rotator = pose.GetRotation().Rotator();
    if (gimbal_stabilization_ > 0) {
        gimbald_rotator_.Pitch = rotator.Pitch;
        gimbald_rotator_.Roll = rotator.Roll;
        gimbald_rotator_.Yaw = rotator.Yaw;
    }
    else {
        this->SetActorRelativeRotation(rotator);
    }
}

void APIPCamera::setCameraFoV(float fov_degrees)
{
    int image_count = static_cast<int>(Utils::toNumeric(ImageType::Count));
    for (int image_type = 0; image_type < image_count; ++image_type) {
        captures_[image_type]->FOVAngle = fov_degrees;
    }

    camera_->SetFieldOfView(fov_degrees);
}

msr::airlib::CameraInfo APIPCamera::getCameraInfo() const
{
    msr::airlib::CameraInfo camera_info;

    camera_info.pose.position = ned_transform_->toLocalNed(this->GetActorLocation());
    camera_info.pose.orientation = ned_transform_->toNed(this->GetActorRotation().Quaternion());
    camera_info.fov = camera_->FieldOfView;
    camera_info.proj_mat = getProjectionMatrix(ImageType::Scene);
    return camera_info;
}

std::vector<float> APIPCamera::getDistortionParams() const
{
    std::vector<float> param_values(5, 0.0);

    auto getParamValue = [this](const auto& name, float& val) {
        distortion_param_instance_->GetScalarParameterValue(FName(name), val);
    };

    getParamValue(TEXT("K1"), param_values[0]);
    getParamValue(TEXT("K2"), param_values[1]);
    getParamValue(TEXT("K3"), param_values[2]);
    getParamValue(TEXT("P1"), param_values[3]);
    getParamValue(TEXT("P2"), param_values[4]);

    return param_values;
}

void APIPCamera::setDistortionParam(const std::string& param_name, float value)
{
    distortion_param_instance_->SetScalarParameterValue(FName(param_name.c_str()), value);
}

void APIPCamera::SetPacketLossParam(const std::string& param_name, float value)
{
	if (PacketLossParamInstance)
		PacketLossParamInstance->SetScalarParameterValue(FName(param_name.c_str()), value);
}

void APIPCamera::setupCameraFromSettings(const APIPCamera::CameraSetting& camera_setting, const NedTransform& ned_transform)
{
    //TODO: should we be ignoring position and orientation settings here?

    //TODO: can we eliminate storing NedTransform?
    ned_transform_ = &ned_transform;

    gimbal_stabilization_ = Utils::clip(camera_setting.gimbal.stabilization, 0.0f, 1.0f);
    if (gimbal_stabilization_ > 0) {
        this->SetActorTickEnabled(true);
        gimbald_rotator_.Pitch = camera_setting.gimbal.rotation.pitch;
        gimbald_rotator_.Roll = camera_setting.gimbal.rotation.roll;
        gimbald_rotator_.Yaw = camera_setting.gimbal.rotation.yaw;
    }
    else
        this->SetActorTickEnabled(false);

    int image_count = static_cast<int>(Utils::toNumeric(ImageType::Count));
    for (int image_type = -1; image_type < image_count; ++image_type) {
        const auto& capture_setting = camera_setting.capture_settings.at(image_type);
        const auto& noise_setting = camera_setting.noise_settings.at(image_type);

        if (image_type >= 0) { //scene capture components
            auto pixel_format_override = camera_setting.ue_setting.pixel_format_override_settings.find(image_type);
            EPixelFormat pixel_format = EPixelFormat::PF_Unknown;
            if (pixel_format_override != camera_setting.ue_setting.pixel_format_override_settings.end()) {
                pixel_format = static_cast<EPixelFormat>(pixel_format_override->second.pixel_format);
            }
            pixel_format = (pixel_format == EPixelFormat::PF_Unknown ? image_type_to_pixel_format_map_[image_type] : pixel_format);

            switch (Utils::toEnum<ImageType>(image_type)) {
            case ImageType::Scene:
            case ImageType::Infrared:
                updateCaptureComponentSetting(captures_[image_type], render_targets_[image_type], false, pixel_format, capture_setting, ned_transform, false);
                break;

            case ImageType::Segmentation:
            case ImageType::SurfaceNormals:
                updateCaptureComponentSetting(captures_[image_type], render_targets_[image_type], true, pixel_format, capture_setting, ned_transform, true);
                break;

            default:
                updateCaptureComponentSetting(captures_[image_type], render_targets_[image_type], true, pixel_format, capture_setting, ned_transform, false);
                break;
            }
            setDistortionMaterial(image_type, captures_[image_type], captures_[image_type]->PostProcessSettings);
            setNoiseMaterial(image_type, captures_[image_type], captures_[image_type]->PostProcessSettings, noise_setting);
			setPacketLossEffectMaterial(image_type, captures_[image_type], captures_[image_type]->PostProcessSettings);
            copyCameraSettingsToSceneCapture(camera_, captures_[image_type]); //CinemAirSim
        }
        else { //camera component
            updateCameraSetting(camera_, capture_setting, ned_transform);
            setDistortionMaterial(image_type, camera_, camera_->PostProcessSettings);
            setNoiseMaterial(image_type, camera_, camera_->PostProcessSettings, noise_setting);
            copyCameraSettingsToAllSceneCapture(camera_); //CinemAirSim
        }
    }
}

void APIPCamera::updateCaptureComponentSetting(USceneCaptureComponent2D* capture, UTextureRenderTarget2D* render_target,
                                               bool auto_format, const EPixelFormat& pixel_format, const CaptureSetting& setting, const NedTransform& ned_transform,
                                               bool force_linear_gamma)
{
    if (auto_format) {
        render_target->InitAutoFormat(setting.width, setting.height); //256 X 144, X 480
    }
    else {
        render_target->InitCustomFormat(setting.width, setting.height, pixel_format, force_linear_gamma);
    }

    if (!std::isnan(setting.target_gamma))
        render_target->TargetGamma = setting.target_gamma;

    capture->ProjectionType = static_cast<ECameraProjectionMode::Type>(setting.projection_mode);

    if (!std::isnan(setting.fov_degrees))
        capture->FOVAngle = setting.fov_degrees;
    if (capture->ProjectionType == ECameraProjectionMode::Orthographic && !std::isnan(setting.ortho_width))
        capture->OrthoWidth = ned_transform.fromNed(setting.ortho_width);

    updateCameraPostProcessingSetting(capture->PostProcessSettings, setting);
}

//CinemAirSim
void APIPCamera::updateCameraSetting(UCineCameraComponent* camera, const CaptureSetting& setting, const NedTransform& ned_transform)
{
    //if (!std::isnan(setting.target_gamma))
    //    camera-> = setting.target_gamma;

    camera->SetProjectionMode(static_cast<ECameraProjectionMode::Type>(setting.projection_mode));

    if (!std::isnan(setting.fov_degrees))
        camera->SetFieldOfView(setting.fov_degrees);
    if (camera->ProjectionMode == ECameraProjectionMode::Orthographic && !std::isnan(setting.ortho_width))
        camera->SetOrthoWidth(ned_transform.fromNed(setting.ortho_width));

    updateCameraPostProcessingSetting(camera->PostProcessSettings, setting);
}

msr::airlib::Pose APIPCamera::getPose() const
{
    return ned_transform_->toLocalNed(this->GetActorTransform());
}

void APIPCamera::updateCameraPostProcessingSetting(FPostProcessSettings& obj, const CaptureSetting& setting)
{
    if (!std::isnan(setting.motion_blur_amount)) {
        obj.bOverride_MotionBlurAmount = 1;
        obj.MotionBlurAmount = setting.motion_blur_amount;
    }
    if (setting.auto_exposure_method >= 0) {
        obj.bOverride_AutoExposureMethod = 1;
        obj.AutoExposureMethod = Utils::toEnum<EAutoExposureMethod>(setting.auto_exposure_method);
    }
    if (!std::isnan(setting.auto_exposure_speed)) {
        obj.bOverride_AutoExposureSpeedDown = 1;
        obj.AutoExposureSpeedDown = obj.AutoExposureSpeedUp = setting.auto_exposure_speed;
    }
    if (!std::isnan(setting.auto_exposure_max_brightness)) {
        obj.bOverride_AutoExposureMaxBrightness = 1;
        obj.AutoExposureMaxBrightness = setting.auto_exposure_max_brightness;
    }
    if (!std::isnan(setting.auto_exposure_min_brightness)) {
        obj.bOverride_AutoExposureMinBrightness = 1;
        obj.AutoExposureMinBrightness = setting.auto_exposure_min_brightness;
    }
    if (!std::isnan(setting.auto_exposure_bias)) {
        obj.bOverride_AutoExposureBias = 1;
        obj.AutoExposureBias = setting.auto_exposure_bias;
    }
    if (!std::isnan(setting.auto_exposure_low_percent)) {
        obj.bOverride_AutoExposureLowPercent = 1;
        obj.AutoExposureLowPercent = setting.auto_exposure_low_percent;
    }
    if (!std::isnan(setting.auto_exposure_high_percent)) {
        obj.bOverride_AutoExposureHighPercent = 1;
        obj.AutoExposureHighPercent = setting.auto_exposure_high_percent;
    }
    if (!std::isnan(setting.auto_exposure_histogram_log_min)) {
        obj.bOverride_HistogramLogMin = 1;
        obj.HistogramLogMin = setting.auto_exposure_histogram_log_min;
    }
    if (!std::isnan(setting.auto_exposure_histogram_log_max)) {
        obj.bOverride_HistogramLogMax = 1;
        obj.HistogramLogMax = setting.auto_exposure_histogram_log_max;
    }
}

void APIPCamera::setDistortionMaterial(int image_type, UObject* outer, FPostProcessSettings& obj)
{
    UMaterialInstanceDynamic* distortion_material = UMaterialInstanceDynamic::Create(distortion_material_static_, outer);
    distortion_materials_[image_type + 1] = distortion_material;
    obj.AddBlendable(distortion_material, 1.0f);
}

void APIPCamera::setNoiseMaterial(int image_type, UObject* outer, FPostProcessSettings& obj, const NoiseSetting& settings)
{
    if (!settings.Enabled)
        return;

    UMaterialInstanceDynamic* noise_material = UMaterialInstanceDynamic::Create(noise_material_static_, outer);
    noise_materials_[image_type + 1] = noise_material;

    noise_material->SetScalarParameterValue("HorzWaveStrength", settings.HorzWaveStrength);
    noise_material->SetScalarParameterValue("RandSpeed", settings.RandSpeed);
    noise_material->SetScalarParameterValue("RandSize", settings.RandSize);
    noise_material->SetScalarParameterValue("RandDensity", settings.RandDensity);
    noise_material->SetScalarParameterValue("RandContrib", settings.RandContrib);
    noise_material->SetScalarParameterValue("HorzWaveContrib", settings.HorzWaveContrib);
    noise_material->SetScalarParameterValue("HorzWaveVertSize", settings.HorzWaveVertSize);
    noise_material->SetScalarParameterValue("HorzWaveScreenSize", settings.HorzWaveScreenSize);
    noise_material->SetScalarParameterValue("HorzNoiseLinesContrib", settings.HorzNoiseLinesContrib);
    noise_material->SetScalarParameterValue("HorzNoiseLinesDensityY", settings.HorzNoiseLinesDensityY);
    noise_material->SetScalarParameterValue("HorzNoiseLinesDensityXY", settings.HorzNoiseLinesDensityXY);
    noise_material->SetScalarParameterValue("HorzDistortionStrength", settings.HorzDistortionStrength);
    noise_material->SetScalarParameterValue("HorzDistortionContrib", settings.HorzDistortionContrib);

    obj.AddBlendable(noise_material, 1.0f);
}

void APIPCamera::setPacketLossEffectMaterial(int image_type, UObject* outer, FPostProcessSettings& obj)
{
    UMaterialInstanceDynamic* PacketLossMaterial = UMaterialInstanceDynamic::Create(PacketLossMaterialStatic, outer);
    PacketLossMaterials[image_type] = PacketLossMaterial;

	PacketLossMaterial->SetScalarParameterValue("PacketLossRate", 0.0f);
    PacketLossMaterial->SetScalarParameterValue("BlockSize", 100.0f);
    PacketLossMaterial->SetScalarParameterValue("TimeStep", 1.0f);

	//PacketLossParamInstance->SetScalarParameterValue(FName("PacketLossRate"), 0.0f);
	//PacketLossParamInstance->SetScalarParameterValue(FName("BlockSize"), 100.0f);
	//PacketLossParamInstance->SetScalarParameterValue(FName("TimeStep"), 1.0f);
    obj.AddBlendable(PacketLossMaterial, 1.0f);
}

void APIPCamera::enableCaptureComponent(const APIPCamera::ImageType type, bool is_enabled)
{
    USceneCaptureComponent2D* capture = getCaptureComponent(type, false);
    if (capture != nullptr && capture->IsValidLowLevel()) {
        UDetectionComponent* detection = getDetectionComponent(type, false);
        if (is_enabled) {
            // Do not make unnecessary calls to Activate() which otherwise causes crash in Unreal
            if (!capture->IsActive() || capture->TextureTarget == nullptr) {
                capture->TextureTarget = getRenderTarget(type, false);
                if (capture->TextureTarget != nullptr) {
                    capture->Activate();
                    if (detection != nullptr && detection->IsValidLowLevel()) {
                        detection->texture_target_ = capture->TextureTarget;
                        detection->Activate();
                    }
                }
            }
        }
        else {
            if (capture->IsActive() || capture->TextureTarget != nullptr) {
                capture->Deactivate();
                capture->TextureTarget = nullptr;
                if (detection != nullptr && detection->IsValidLowLevel()) {
                    detection->Deactivate();
                    detection->texture_target_ = nullptr;
                }
            }
        }
        camera_type_enabled_[Utils::toNumeric(type)] = is_enabled;
    }
    // Else nothing to enable
}

UTextureRenderTarget2D* APIPCamera::getRenderTarget(const APIPCamera::ImageType type, bool if_active)
{
    unsigned int image_type = Utils::toNumeric(type);

    if (!if_active || camera_type_enabled_[image_type])
        return render_targets_[image_type];
    return nullptr;
}

UDetectionComponent* APIPCamera::getDetectionComponent(const ImageType type, bool if_active) const
{
    unsigned int image_type = Utils::toNumeric(type);

    if (!if_active || camera_type_enabled_[image_type])
        return detections_[image_type];
    return nullptr;
}

USceneCaptureComponent2D* APIPCamera::getCaptureComponent(const APIPCamera::ImageType type, bool if_active)
{
    unsigned int image_type = Utils::toNumeric(type);

    if (!if_active || camera_type_enabled_[image_type])
        return captures_[image_type];
    return nullptr;
}

UTextureRenderTarget2D* APIPCamera::getRenderTarget(int32 type, bool if_active)
{
    ImageType Type = static_cast<ImageType>(type);
    return getRenderTarget(Type, if_active);
}

void APIPCamera::setCameraTypeEnabled(int32 type, bool enabled)
{
    ImageType Type = static_cast<ImageType>(type);
    return setCameraTypeEnabled(Type, enabled);
}

void APIPCamera::setCameraTypeUpdate(int32 type, bool nodisplay, bool isZMQ = true)
{
	bUsingZMQ = isZMQ;
    ImageType Type = static_cast<ImageType>(type);
    return setCameraTypeUpdate(Type, nodisplay);
}

void APIPCamera::CaptureRenderTargetToFile(UTextureRenderTarget2D* RenderTarget, FString UAVName)
{
    if (!RenderTarget)
    {
        UE_LOG(LogTemp, Error, TEXT("Render Target is NULL!"));
        return;
    }

    FRenderTarget* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
    TArray<FColor> Bitmap;
    RenderTargetResource->ReadPixels(Bitmap);

    if (Bitmap.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to capture pixels from Render Target."));
        return;
    }

    // Convert to PNG
    TArray<uint8> CompressedBitmap;
    FImageUtils::CompressImageArray(RenderTarget->SizeX, RenderTarget->SizeY, Bitmap, CompressedBitmap);

    // Save to file
    FString SaveFolder = FPaths::ProjectDir() + TEXT("Saved/Images/");
    // Ensure the directory exists
    if (!FPaths::DirectoryExists(SaveFolder)) {
        IFileManager::Get().MakeDirectory(*SaveFolder, true);
    }
    // Construct the full save path with filename
    FString Timestamp = FDateTime::Now().ToString(TEXT("%Y-%m-%d_%H-%M-%S_%s"));
    FString SavePath = SaveFolder + UAVName + TEXT("_") + Timestamp + TEXT(".png");
    if (FFileHelper::SaveArrayToFile(CompressedBitmap, *SavePath))
    {
        UE_LOG(LogTemp, Log, TEXT("Saved Render Target to: %s"), *SavePath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save PNG file."));
    }
}

void APIPCamera::SetDesiredCaptureFPS(float NewFPS)
{
    FPS = FMath::Clamp(NewFPS, 5.0f, 60.0f);
}

void APIPCamera::disableAllPIP()
{
    for (unsigned int image_type = 0; image_type < imageTypeCount(); ++image_type) {
        enableCaptureComponent(Utils::toEnum<ImageType>(image_type), false);
    }
}

void APIPCamera::disableMain()
{
    camera_->Deactivate();
    camera_->SetVisibility(false);
    //APlayerController* controller = this->GetWorld()->GetFirstPlayerController();
    //if (controller && controller->GetViewTarget() == this)
    //    controller->SetViewTarget(nullptr);
}

void APIPCamera::onViewModeChanged(bool nodisplay)
{
    for (unsigned int image_type = 0; image_type < imageTypeCount(); ++image_type) {
        USceneCaptureComponent2D* capture = getCaptureComponent(static_cast<ImageType>(image_type), false);
        if (capture) {
            setCaptureUpdate(capture, nodisplay);
        }
    }
}

//CinemAirSim methods
std::vector<std::string> APIPCamera::getPresetLensSettings() const
{
    std::vector<std::string> vector;
    const TArray<FNamedLensPreset> lens_presets = camera_->GetLensPresets();
    for (const FNamedLensPreset& preset : lens_presets) {
        std::ostringstream current_lens_string;
        std::string name = (TCHAR_TO_UTF8(*preset.Name));

        current_lens_string << "Name: " << name << ";\n\t MinFocalLength: " << preset.LensSettings.MinFocalLength << "; \t MaxFocalLength: " << preset.LensSettings.MaxFocalLength;
        current_lens_string << "\n\t Min FStop: " << preset.LensSettings.MinFStop << "; \t Max Fstop: " << preset.LensSettings.MaxFStop;
        vector.push_back(current_lens_string.str());
    }
    return vector;
}

std::string APIPCamera::getLensSettings() const
{
    const FCameraLensSettings current_lens_params = camera_->LensSettings;

    std::ostringstream current_lens_string;

    const FString lens_preset_name = camera_->GetLensPresetName();
    std::string name = (TCHAR_TO_UTF8(*lens_preset_name));

    current_lens_string << "Name: " << name;
    current_lens_string << ";\n\t MinFocalLength: " << current_lens_params.MinFocalLength;
    current_lens_string << "; \t MaxFocalLength: " << current_lens_params.MaxFocalLength;
    current_lens_string << "\n\t Min FStop: " << current_lens_params.MinFStop;
    current_lens_string << "; \t Max Fstop: " << current_lens_params.MaxFStop;
    current_lens_string << "\n\t Diaphragm Blade Count: " << current_lens_params.DiaphragmBladeCount;
    current_lens_string << "\n\t Minimum focus distance: " << current_lens_params.MinimumFocusDistance;

    return current_lens_string.str();
}

void APIPCamera::setPresetLensSettings(std::string preset_string)
{
    const FString preset(preset_string.c_str());
    camera_->SetLensPresetByName(preset);
    copyCameraSettingsToAllSceneCapture(camera_);
}

std::vector<std::string> APIPCamera::getPresetFilmbackSettings() const
{
    std::vector<std::string> vector_all_presets;
    TArray<FNamedFilmbackPreset> lens_presets = camera_->GetFilmbackPresets();
    for (const FNamedFilmbackPreset& preset : lens_presets) {
        std::ostringstream preset_string;
        std::string name = (TCHAR_TO_UTF8(*preset.Name));

        preset_string << "Name: " << name << ";\n\t Sensor Width: " << preset.FilmbackSettings.SensorWidth << "; \t Sensor Height: " << preset.FilmbackSettings.SensorHeight;
        preset_string << "\n\t Sensor Aspect Ratio: " << preset.FilmbackSettings.SensorAspectRatio;
        vector_all_presets.push_back(preset_string.str());
    }
    return vector_all_presets;
}

void APIPCamera::setPresetFilmbackSettings(std::string preset_string)
{
    const FString preset(preset_string.c_str());
    camera_->SetFilmbackPresetByName(preset);
    copyCameraSettingsToAllSceneCapture(camera_);
}

std::string APIPCamera::getFilmbackSettings() const
{
    FCameraFilmbackSettings current_filmback_settings = camera_->Filmback;

    const FString filmback_present_name = camera_->GetFilmbackPresetName();
    std::ostringstream current_filmback_string;
    std::string name = (TCHAR_TO_UTF8(*filmback_present_name));

    current_filmback_string << "Name: " << name << ";\n\t Sensor Width: " << current_filmback_settings.SensorWidth << "; \t Sensor Height: " << current_filmback_settings.SensorHeight;
    current_filmback_string << "\n\t Sensor Aspect Ratio: " << current_filmback_settings.SensorAspectRatio;
    return current_filmback_string.str();
}

float APIPCamera::setFilmbackSettings(float sensor_width, float sensor_height)
{
    camera_->Filmback.SensorWidth = sensor_width;
    camera_->Filmback.SensorHeight = sensor_height;

    copyCameraSettingsToAllSceneCapture(camera_);

    return camera_->Filmback.SensorAspectRatio;
}

float APIPCamera::getFocalLength() const
{
    return camera_->CurrentFocalLength;
}

void APIPCamera::setFocalLength(float focal_length)
{
    camera_->CurrentFocalLength = focal_length;
    copyCameraSettingsToAllSceneCapture(camera_);
}

void APIPCamera::enableManualFocus(bool enable)
{
    if (enable) {
        camera_->FocusSettings.FocusMethod = ECameraFocusMethod::Manual;
    }
    else {
        camera_->FocusSettings.FocusMethod = ECameraFocusMethod::Disable;
    }
    copyCameraSettingsToAllSceneCapture(camera_);
}

float APIPCamera::getFocusDistance() const
{
    return camera_->FocusSettings.ManualFocusDistance;
}

void APIPCamera::setFocusDistance(float focus_distance)
{
    camera_->FocusSettings.ManualFocusDistance = focus_distance;
    copyCameraSettingsToAllSceneCapture(camera_);
}

float APIPCamera::getFocusAperture() const
{
    return camera_->CurrentAperture;
}

void APIPCamera::setFocusAperture(float focus_aperture)
{
    camera_->CurrentAperture = focus_aperture;
    copyCameraSettingsToAllSceneCapture(camera_);
}

void APIPCamera::enableFocusPlane(bool enable)
{
    camera_->FocusSettings.bDrawDebugFocusPlane = enable;
}

std::string APIPCamera::getCurrentFieldOfView() const
{
    std::ostringstream field_of_view_string;
    field_of_view_string << "Current Field Of View:\n\tHorizontal Field Of View: " << camera_->GetHorizontalFieldOfView() << ";\n\t Vertical Field Of View: " << camera_->GetVerticalFieldOfView();
    return field_of_view_string.str();
}

void APIPCamera::copyCameraSettingsToAllSceneCapture(UCameraComponent* camera)
{
    int image_count = static_cast<int>(Utils::toNumeric(ImageType::Count));
    for (int image_type = image_count - 1; image_type >= 0; image_type--) {
        copyCameraSettingsToSceneCapture(camera_, captures_[image_type]);
    }
}

void APIPCamera::copyCameraSettingsToSceneCapture(UCameraComponent* src, USceneCaptureComponent2D* dst)
{
    if (src && dst) {
        dst->SetWorldLocationAndRotation(src->GetComponentLocation(), src->GetComponentRotation());

        FMinimalViewInfo camera_view_info;
        src->GetCameraView(/*DeltaTime =*/0.0f, camera_view_info);

        const FPostProcessSettings& src_pp_settings = camera_view_info.PostProcessSettings;
        FPostProcessSettings& dst_pp_settings = dst->PostProcessSettings;

        FWeightedBlendables dst_weighted_blendables = dst_pp_settings.WeightedBlendables;

        // Copy all of the post processing settings
        dst_pp_settings = src_pp_settings;

        // But restore the original blendables
        dst_pp_settings.WeightedBlendables = dst_weighted_blendables;
    }
}

void APIPCamera::CaptureFrame()
{
    CurrentQueueFrame++;

    float CaptureInterval = 1.0f / FMath::Clamp(FPS, 5.0f, 60.0f);
    GetWorld()->GetTimerManager().SetTimer(CaptureTimerHandle, this, &APIPCamera::CaptureFrame, CaptureInterval, false);

    //FString TimeStamp = FDateTime::Now().ToString();
    //UE_LOG(LogTemp, Log, TEXT("Start Capture Request for frame %d at time %s"), CurrentQueueFrame, *TimeStamp);

    //// Log capture interval
    //UE_LOG(LogTemp, Verbose, TEXT("Capture Interval: %.2f ms"), CaptureInterval * 1000.0f);

    if (!NetworkEffectManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("NetworkEffectManager not found! Cannot apply network effects."));
        NetworkEffectManager = GetGameInstance()->GetSubsystem<UNetworkEffectManager>();
        return;
    }

    if (!TargetWidget)
    {
        return;
    }

    int64 ReturnFrame = CurrentQueueFrame;

    if (LastCaptureFrame < ReturnFrame)
    {
        /*FString LogLastFrame = "Time: " + TimeStamp + " Last Frame: " + LexToString(LastCaptureFrame);
        UE_LOG(LogTemp, Log, TEXT("%s"), *LogLastFrame);

        UE_LOG(LogTemp, Log, TEXT("Processing %d camera types:"), camera_type_enabled_.size());*/

        for (size_t i = 0; i < camera_type_enabled_.size(); ++i)
        {
            if (camera_type_enabled_[i])
            {
                ImageType type = static_cast<ImageType>(i);
                USceneCaptureComponent2D* capture = getCaptureComponent(type, false);

                //UE_LOG(LogTemp, Log, TEXT("Camera Type %d (enum: %d): Enabled"), i, (int32)type);

                if (capture && capture->TextureTarget)
                {
                    capture->CaptureScene();

                    if (VideoFrameTracker)
                    {
						std::chrono::nanoseconds TimeStamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
                        FVideoFrameTrack& NewFrame = VideoFrameTracker->AddOrGetFrame(ReturnFrame);
                        NewFrame.UavCaptureTimestamp = TimeStamp.count();
                        NewFrame.UavPosition = GetActorLocation();
						NewFrame.UavYaw = GetActorRotation().Yaw;
                    }

                    if (bUsingZMQ)
                    {
                        Callback.AddUObject(TargetWidget, &UCameraView::UpdateDisplayTexture);
                        /*NetworkEffectManager->QueueDelayedRenderTarget(
                            capture->TextureTarget,
                            FlowId,
                            Callback,
                            ReturnFrame
                        );*/

                        NetworkEffectManager->QueueDelayedRenderTarget(
                            capture->TextureTarget,
                            1,
                            Callback,
                            ReturnFrame
                        );
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Capture Component or TextureTarget is null"));
                }
            }
            else
            {
                UE_LOG(LogTemp, Verbose, TEXT("Camera Type %d: Disabled"), i);
            }
        }

        LastCaptureFrame = ReturnFrame;
        /*FString LogNewFrame = "Time: " + TimeStamp + " New Captured Frame: " + LexToString(LastCaptureFrame);
        UE_LOG(LogTemp, Log, TEXT("%s"), *LogNewFrame);*/
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Frame %d skipped | Already captured"), CurrentQueueFrame);
    }
}

UTexture2D* APIPCamera::RenderTargetToTexture2D(UTextureRenderTarget2D* RenderTarget)
{
    if (!RenderTarget)
    {
        UE_LOG(LogTemp, Error, TEXT("RenderTargetToTexture2D: RenderTarget is null!"));
        return nullptr;
    }

    const int32 Width = RenderTarget->GetSurfaceWidth();
    const int32 Height = RenderTarget->GetSurfaceHeight();

    UTexture2D* Result = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
    if (!Result)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create transient Texture2D!"));
        return nullptr;
    }

    Result->SRGB = RenderTarget->SRGB;
    Result->AddToRoot();
    Result->UpdateResource();

    // Allocate shared buffer
    TSharedRef<TArray<FColor>, ESPMode::ThreadSafe> PixelBuffer = MakeShared<TArray<FColor>, ESPMode::ThreadSafe>();
    PixelBuffer->SetNumUninitialized(Width * Height);

    ENQUEUE_RENDER_COMMAND(CopyRenderTargetCmd)([RenderTarget, Result, Width, Height, PixelBuffer](FRHICommandListImmediate& RHICmdList)
        {
            FTextureResource* Res = RenderTarget->GetResource();
            if (!Res)
            {
                UE_LOG(LogTemp, Error, TEXT("RenderTarget has no resource!"));
                return;
            }

            RHICmdList.ReadSurfaceData(
                Res->GetTexture2DRHI(),
                FIntRect(0, 0, Width, Height),
                *PixelBuffer,
                FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX)
            );

            FTexture2DResource* TextureResource = Result->GetResource()->GetTexture2DResource();
            if (!TextureResource)
            {
                UE_LOG(LogTemp, Error, TEXT("TextureResource is null!"));
                return;
            }

            const int32 BytesPerPixel = GPixelFormats[PF_B8G8R8A8].BlockBytes;
            const uint32 SrcPitch = Width * BytesPerPixel;

            FUpdateTextureRegion2D Region(0, 0, 0, 0, Width, Height);
            RHIUpdateTexture2D(
                TextureResource->GetTexture2DRHI(),
                0,
                Region,
                SrcPitch,
                reinterpret_cast<uint8*>(PixelBuffer->GetData())
            );
        });

    return Result;
}



void APIPCamera::SetRenderTargetResolution(int32 Width, int32 Height, int32 type)
{
    if (!captures_[type])
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid ImageType or Scene Capture Component is NULL"));
        return;
    }

    USceneCaptureComponent2D* CaptureComp = captures_[type];
    UTextureRenderTarget2D* RenderTarget = render_targets_[type];

    if (!RenderTarget)
    {
        UE_LOG(LogTemp, Error, TEXT("Render Target is NULL for ImageType %d"), static_cast<int>(type));
        return;
    }

    // Keep the same Pixel Format and other settings
    EPixelFormat PixelFormat = RenderTarget->GetFormat();
    bool bForceLinearGamma = RenderTarget->bForceLinearGamma;

    // Reinitialize the render target with new resolution but same settings
    RenderTarget->InitCustomFormat(Width, Height, PixelFormat, bForceLinearGamma);
    RenderTarget->UpdateResource();

    // Assign updated render target back to Scene Capture Component
    CaptureComp->TextureTarget = RenderTarget;

    UE_LOG(LogTemp, Log, TEXT("Updated Render Target Resolution: %d x %d for ImageType %d"), Width, Height, static_cast<int>(type));
}

void APIPCamera::ClearCaptureTimer()
{
    GetWorld()->GetTimerManager().ClearTimer(CaptureTimerHandle);
}


//end CinemAirSim methods
