// Fill out your copyright notice in the Description page of Project Settings.


#include "GroundControlStation.h"
#include "Engine/Engine.h"
#include "NetworkStateInstance.h"
#include "Async/Async.h"
#include "Vehicles/Multirotor/FlyingPawn.h"
#include "Kismet/GameplayStatics.h"
#include <opencv2/imgcodecs.hpp>
#include <fstream>

// Sets default values
AGroundControlStation::AGroundControlStation()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AirSimClient = std::make_unique<msr::airlib::MultirotorRpcLibClient>();
	bIsConnected = false;
	bShuttingDown = false;
	bTaskResult = false;
	OffsetZ = -1212;
	for (FString UAVName : ListUAVName) {
		AllLandedStates.Add(UAVName, msr::airlib::LandedState::Landed);
	}
}

// Called when the game starts or when spawned
void AGroundControlStation::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("BeginPlay called."));
	
	GetWorldTimerManager().SetTimer(ConnectionCheckTimer, this, &AGroundControlStation::CheckConnection, 1.0f, true);
	// Confirm connection with AirSim
	/*AirSimClient->confirmConnection();
	*/
}

// Called every frame
void AGroundControlStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bIsConnected) return;
}

// Called to bind functionality to input
void AGroundControlStation::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


// ==================================== DRONE CONTROL FUNCTIONS ====================================

void AGroundControlStation::CheckConnection()
{
	if (bShuttingDown || IsEngineExitRequested() || IsGarbageCollecting()) return;

	try {
		AirSimClient->enableApiControl(true);
		if (!bIsConnected) {
			bIsConnected = true;
			UE_LOG(LogTemp, Log, TEXT("Connected to AirSim!"));
		}
	}
	catch (const std::exception& e) {
		UE_LOG(LogTemp, Error, TEXT("Connection error: %s"), UTF8_TO_TCHAR(e.what()));
		bIsConnected = false;
	}
}

void AGroundControlStation::ArmDrone(FString UAVName)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	try {
		if (AirSimClient->isApiControlEnabled(TCHAR_TO_UTF8(*UAVName))) {
			AirSimClient->armDisarm(true, TCHAR_TO_UTF8(*UAVName));
			UE_LOG(LogTemp, Log, TEXT("Drone Armed: %s"), *UAVName);
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("API control not enabled for drone: %s"), *UAVName);
		}
	}
	catch (const std::exception& e) {
		UE_LOG(LogTemp, Error, TEXT("Arm error: %s"), UTF8_TO_TCHAR(e.what()));
	}
}

void AGroundControlStation::DisarmDrone(FString UAVName)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	try {
		if (AirSimClient->isApiControlEnabled(TCHAR_TO_UTF8(*UAVName))) {
			AirSimClient->armDisarm(false, TCHAR_TO_UTF8(*UAVName));
			UE_LOG(LogTemp, Log, TEXT("Drone Disarmed: %s"), *UAVName);
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("API control not enabled for drone: %s"), *UAVName);
		}
	}
	catch (const std::exception& e) {
		UE_LOG(LogTemp, Error, TEXT("Disarm error: %s"), UTF8_TO_TCHAR(e.what()));
	}
}

void AGroundControlStation::Takeoff(FString UAVName)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, UAVName]()
		{
			try {
				if (AirSimClient->isApiControlEnabled(TCHAR_TO_UTF8(*UAVName))) {
					if (AirSimClient->armDisarm(true, TCHAR_TO_UTF8(*UAVName))) {
						UE_LOG(LogTemp, Log, TEXT("Drone Armed: %s"), *UAVName);
						AirSimClient->takeoffAsync(10.0f, TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&bTaskResult, 10.0f);
						UE_LOG(LogTemp, Log, TEXT("Drone Taking Off: %s"), *UAVName);

						if (bTaskResult)
						{
							AllLandedStates.Add(UAVName, msr::airlib::LandedState::Flying);
							UE_LOG(LogTemp, Log, TEXT("Drone Takeoff Successful: %s"), *UAVName);
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("Drone Takeoff Failed: %s"), *UAVName);
						}
					}
					else {
						UE_LOG(LogTemp, Error, TEXT("Drone not armed: %s"), *UAVName);
					}
				}
				else {
					UE_LOG(LogTemp, Error, TEXT("API control not enabled for drone: %s"), *UAVName);
				}
			}
			catch (const std::exception& e) {
				UE_LOG(LogTemp, Error, TEXT("Takeoff error: %s"), UTF8_TO_TCHAR(e.what()));
			}
		});
}

void AGroundControlStation::Land(FString UAVName)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, UAVName]()
		{
			try {
				if (AirSimClient->isApiControlEnabled(TCHAR_TO_UTF8(*UAVName))) {
					AirSimClient->cancelLastTask(TCHAR_TO_UTF8(*UAVName));
					AirSimClient->landAsync(10.0f, TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&bTaskResult, 10.0f);
					UE_LOG(LogTemp, Log, TEXT("Drone Landing: %s"), *UAVName);
					if (bTaskResult)
					{
						AllLandedStates.Add(UAVName, msr::airlib::LandedState::Landed);
						UE_LOG(LogTemp, Log, TEXT("Drone Landing Successful: %s"), *UAVName);
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("Drone Landing Failed: %s"), *UAVName);
					}
				}
				else {
					UE_LOG(LogTemp, Error, TEXT("API control not enabled for drone: %s"), *UAVName);
				}
			}
			catch (const std::exception& e) {
				UE_LOG(LogTemp, Error, TEXT("Land error: %s"), UTF8_TO_TCHAR(e.what()));
			}
		});
}

void AGroundControlStation::Hover(FString UAVName)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, UAVName]()
		{
			try {
				if (AirSimClient->isApiControlEnabled(TCHAR_TO_UTF8(*UAVName))) {
					AirSimClient->cancelLastTask(TCHAR_TO_UTF8(*UAVName));
					AirSimClient->hoverAsync(TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&bTaskResult, 5.0f);
					UE_LOG(LogTemp, Log, TEXT("Drone Hovering: %s"), *UAVName);

					if (bTaskResult)
					{
						UE_LOG(LogTemp, Log, TEXT("Drone Hovering Successful: %s"), *UAVName);
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("Drone Hovering Failed: %s"), *UAVName);
					}
				}
				else {
					UE_LOG(LogTemp, Error, TEXT("API control not enabled for drone: %s"), *UAVName);
				}
			}
			catch (const std::exception& e) {
				UE_LOG(LogTemp, Error, TEXT("Hover error: %s"), UTF8_TO_TCHAR(e.what()));
			}
		});
}

int64 AGroundControlStation::GetLandedState(FString UAVName)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return -1;
	}

	try {
		AllLandedStates.Add(UAVName, AirSimClient->getMultirotorState(TCHAR_TO_UTF8(*UAVName)).landed_state);
	}
	catch (const std::exception& e) {
		UE_LOG(LogTemp, Error, TEXT("Get Landed State Failed: %s"), UTF8_TO_TCHAR(e.what()));
		return -1;
	}
	return (int64)AllLandedStates[UAVName];
}

void AGroundControlStation::MoveToLocation(FString UAVName, FVector Location, float Velocity, float Timeout) 
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, UAVName, Location, Velocity, Timeout]()
		{
			try {
				float TargetZ = OffsetZ - Location.Z;
				AirSimClient->cancelLastTask(TCHAR_TO_UTF8(*UAVName));
				AirSimClient->moveToPositionAsync(Location.X, Location.Y, TargetZ, Velocity, Timeout,
					msr::airlib::DrivetrainType::MaxDegreeOfFreedom, msr::airlib::YawMode(),
					-1, 1, TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&bTaskResult, Timeout);
				UE_LOG(LogTemp, Log, TEXT("Drone Moving to Location %f %f %f: %s"), Location.X, Location.Y, Location.Z, *UAVName);
				if (bTaskResult)
				{
					UE_LOG(LogTemp, Log, TEXT("Drone Move to Location Successful: %s"), *UAVName);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Drone Move to Location Failed: %s"), *UAVName);
				}
			}
			catch (const std::exception& e) {
				UE_LOG(LogTemp, Error, TEXT("Move to location error: %s"), UTF8_TO_TCHAR(e.what()));
			}
		});
}

void AGroundControlStation::MoveByPath(FString UAVName, const TArray<FVector>& Path, float Velocity, float Timeout)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, UAVName, Path, Velocity, Timeout]()
		{
			try {
				std::vector<msr::airlib::Vector3r> Waypoints;
				for (FVector Point : Path)
				{
					Waypoints.push_back(msr::airlib::Vector3r(Point.X, Point.Y, Point.Z));
				}
				AirSimClient->cancelLastTask(TCHAR_TO_UTF8(*UAVName));
				AirSimClient->moveOnPathAsync(Waypoints, Velocity, Timeout, msr::airlib::DrivetrainType::MaxDegreeOfFreedom, msr::airlib::YawMode(),
					-1, 1, TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&bTaskResult, Timeout);
				UE_LOG(LogTemp, Log, TEXT("Drone Moving by Path: %s"), *UAVName);
				if (bTaskResult)
				{
					UE_LOG(LogTemp, Log, TEXT("Drone Move by Path Successful: %s"), *UAVName);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Drone Move by Path Failed: %s"), *UAVName);
				}
			}
			catch (const std::exception& e) {
				UE_LOG(LogTemp, Error, TEXT("Move by path error: %s"), UTF8_TO_TCHAR(e.what()));
			}
		});
}

void AGroundControlStation::MoveByVelocitySameZ(FString UAVName, FVector2D VelocityXY, float Z, float Timeout)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, UAVName, VelocityXY, Z, Timeout]()
		{
			try {
				// Go to target Z first
				//AirSimClient->moveToZAsync(Z, 1.0f, Z, msr::airlib::YawMode(), -1, 1, TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&bTaskResult, Z * 2);
				AirSimClient->cancelLastTask(TCHAR_TO_UTF8(*UAVName));
				// Move by velocity with same Z
				AirSimClient->moveByVelocityZAsync(VelocityXY.X, VelocityXY.Y, -Z, Timeout, msr::airlib::DrivetrainType::MaxDegreeOfFreedom, msr::airlib::YawMode(),
					TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&bTaskResult, Timeout * 1.5);
				UE_LOG(LogTemp, Log, TEXT("Drone Moving by Velocity with same Z: %s"), *UAVName);
				if (bTaskResult)
				{
					UE_LOG(LogTemp, Log, TEXT("Drone Move by Velocity with same Z Successful: %s"), *UAVName);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Drone Move by Velocity with same Z Failed: %s"), *UAVName);
				}
			}
			catch (const std::exception& e) {
				UE_LOG(LogTemp, Error, TEXT("Move by velocity with same Z error: %s"), UTF8_TO_TCHAR(e.what()));
			}
		});
}

// ==================================== DATA COLLECTOR FUNCTIONS ====================================

void AGroundControlStation::GetTelemetryData()
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	for (const FString& UAVName : ListUAVName)
	{
		// Get flow ID for network simulation
		int32 FlowId = 1; // Placeholder

		try {
			if (AirSimClient->isApiControlEnabled(TCHAR_TO_UTF8(*UAVName))) {
				auto drone_state = AirSimClient->getMultirotorState(TCHAR_TO_UTF8(*UAVName));
				if (OffsetZ == -1212) OffsetZ = drone_state.getPosition().z();
				float CurrentZ = OffsetZ - drone_state.getPosition().z();
				float CurrenAltitude = OffsetZ - drone_state.gps_location.altitude;
				FTelemetryData Telemetry;
				Telemetry.Timestamp = drone_state.timestamp;
				Telemetry.Name = UAVName;
				Telemetry.Position = FVector(drone_state.getPosition().x(), drone_state.getPosition().y(), CurrentZ);
				Telemetry.GPS = FVector(drone_state.gps_location.latitude, drone_state.gps_location.longitude, CurrenAltitude);
				Telemetry.Velocity = FVector(drone_state.kinematics_estimated.twist.linear.x(), drone_state.kinematics_estimated.twist.linear.y(), drone_state.kinematics_estimated.twist.linear.z());
				Telemetry.BatteryLevel = 100.0f;  // Placeholder
				Telemetry.IsConnected = AirSimClient->isApiControlEnabled(TCHAR_TO_UTF8(*UAVName));
				// Push telemetry data to the main thread
				AsyncTask(ENamedThreads::GameThread, [this, Telemetry]()
					{
						// Check if telemetry data for this UAV already exists
						for (FTelemetryData& ExistingTelemetry : ListTelemetryData)
						{
							if (ExistingTelemetry.Name == Telemetry.Name)
							{
								// Update existing telemetry entry
								ExistingTelemetry = Telemetry;
								//UE_LOG(LogTemp, Log, TEXT("Telemetry updated for %s"), *Telemetry.Name);
								return;
							}
						}
						// If not found, add as a new entry
						ListTelemetryData.Add(Telemetry);
						//UE_LOG(LogTemp, Log, TEXT("Telemetry added for %s"), *Telemetry.Name);
					});
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("API control not enabled for drone: %s"), *UAVName);
			}
		}
		catch (const std::exception& e) {
			UE_LOG(LogTemp, Error, TEXT("Get telemetry data error: %s"), UTF8_TO_TCHAR(e.what()));
		}
	}
}

void AGroundControlStation::GetLastestVideoFrame(FString UAVName, bool IsCapture, FString FilePath)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	try {
		std::vector<uint8_t> ImageData = AirSimClient->simGetImage("front_center", msr::airlib::ImageCaptureBase::ImageType::Scene, TCHAR_TO_UTF8(*UAVName));
		if (ImageData.empty())
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to retrieve video frame for %s"), *UAVName);
			return;
		}
		cv::Mat DecodedImage = cv::imdecode(cv::Mat(ImageData), cv::IMREAD_COLOR);
		if (DecodedImage.empty())
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to decode image for %s"), *UAVName);
			return;
		}
		if (IsCapture) {
			// Define the save folder path
			FString SaveFolder = FPaths::ProjectDir() + TEXT("Saved/Images/");
			// Ensure the directory exists
			if (!FPaths::DirectoryExists(SaveFolder)) {
				IFileManager::Get().MakeDirectory(*SaveFolder, true);
			}
			// Construct the full save path with filename
			FString SavePath = SaveFolder + UAVName + TEXT("_") + FDateTime::Now().ToString() + TEXT(".png");
			std::string SavePathStr = TCHAR_TO_UTF8(*SavePath);
			// Save the image using OpenCV
			if (!cv::imwrite(SavePathStr, DecodedImage)) {
				UE_LOG(LogTemp, Error, TEXT("Failed to save image to %s"), *SavePath);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Image saved successfully at: %s"), *SavePath);
			}
		}
		// Convert image to texture
		UTexture2D* VideoFrame = ConvertImageToTexture(DecodedImage);
		if (VideoFrame) {
			HandleVideoFrame(UAVName, VideoFrame);
		}
	}
	catch (const std::exception& e) {
		UE_LOG(LogTemp, Error, TEXT("Get lastest video frame error: %s"), UTF8_TO_TCHAR(e.what()));
	}
}

UTexture2D* AGroundControlStation::ConvertImageToTexture(const cv::Mat& Image)
{
	if (IsEngineExitRequested()) return nullptr;
	if (Image.empty()) return nullptr;

	cv::Mat ProcessedImage;
	if (Image.type() == CV_8UC3)
	{
		cv::cvtColor(Image, ProcessedImage, cv::COLOR_BGR2BGRA);
	}
	else if (Image.type() == CV_8UC4)
	{
		ProcessedImage = Image.clone(); // Directly use if already in BGRA
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MatToTexture2D: Unsupported Mat type. Only CV_8UC3 and CV_8UC4 are supported."));
		return nullptr;
	}

	// Create texture
	UTexture2D* Texture = UTexture2D::CreateTransient(ProcessedImage.cols, ProcessedImage.rows, PF_B8G8R8A8);

	if (!Texture || !Texture->PlatformData)
	{
		UE_LOG(LogTemp, Error, TEXT("MatToTexture2D: Failed to create texture."));
		return nullptr;
	}
	Texture->SRGB = 0;
	Texture->AddToRoot();

	// Copy image data to texture
	uint8* TextureData = (uint8*)Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, ProcessedImage.data, ProcessedImage.total() * ProcessedImage.elemSize());
	Texture->PlatformData->Mips[0].BulkData.Unlock();

	Texture->UpdateResource();
	return Texture;
}


void AGroundControlStation::HandleVideoFrame(const FString& UAVName, UTexture2D* VideoTexture)
{
	//if (IsEngineExitRequested()) return;
	//if (!VideoTexture)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Failed to handle video frame for %s"), *UAVName);
	//	return;
	//}
	//// Broadcast video frame received event
	//OnVideoFrameReceived.Broadcast(UAVName, VideoTexture);
}


// ==================================== NETWORK SIMULATION FUNCTIONS ====================================

void AGroundControlStation::SimulateNetworkRequest(int32 FlowId, TFunction<void()> RequestFunction)
{
	// Get network data from the network state instance
	UNetworkStateInstance* NetworkStateInstance = Cast<UNetworkStateInstance>(GetGameInstance());
	if (!NetworkStateInstance || !NetworkStateInstance->GetFlowData().Contains(FlowId))
	{
		//UE_LOG(LogTemp, Error, TEXT("Network state instance not found or flow data not available!"));
		TWeakObjectPtr<AGroundControlStation> WeakThis = this;
		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis, RequestFunction]()
			{
				if (!WeakThis.IsValid() || WeakThis.Get()->bShuttingDown || IsEngineExitRequested() || IsGarbageCollecting()) return;
				RequestFunction();
			});
		return;
	}

	const FFlowData& FlowData = NetworkStateInstance->GetFlowData()[FlowId];

	float Delay = FlowData.MeanDelay; // microseconds
	float Jitter = FlowData.MeanJitter; // microseconds
	float PacketLoss = FlowData.PacketLossL3;

	// Simulate network packet loss
	if (FMath::FRandRange(0.0f,100.0f) < PacketLoss)
	{
		UE_LOG(LogTemp, Warning, TEXT("Packet loss detected!"));
		return;
	}

	TWeakObjectPtr<AGroundControlStation> WeakThis = this;
	// Simulate network delay
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis, Delay, Jitter, RequestFunction]()
	{
		if (!WeakThis.IsValid() || WeakThis.Get()->bShuttingDown || IsEngineExitRequested() || IsGarbageCollecting()) return;
		FPlatformProcess::Sleep(FMath::FRandRange((Delay - Jitter) / 1000000, (Delay + Jitter) / 1000000));
		
		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis, RequestFunction]()
			{
				if (!WeakThis.IsValid() || WeakThis.Get()->bShuttingDown || IsEngineExitRequested() || IsGarbageCollecting()) return;
				RequestFunction();
			});
	});
}

// Cleanup
AGroundControlStation::~AGroundControlStation()
{
}

void AGroundControlStation::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	bShuttingDown = true;
	GetWorldTimerManager().ClearAllTimersForObject(this);
	if (AirSimClient) {
		UE_LOG(LogTemp, Log, TEXT("Stopping all UAV operations before shutdown..."));

		AirSimClient = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}
