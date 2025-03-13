// Fill out your copyright notice in the Description page of Project Settings.


#include "GroundControlStation.h"
#include "Engine/Engine.h"
#include "NetworkStateInstance.h"
#include "Async/Async.h"

// Sets default values
AGroundControlStation::AGroundControlStation()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AirSimClient = new msr::airlib::MultirotorRpcLibClient();
	bIsConnected = false;
}

// Called when the game starts or when spawned
void AGroundControlStation::BeginPlay()
{
	Super::BeginPlay();
	
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

void AGroundControlStation::ArmDrone()
{
	if (!bIsConnected) {
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Drone not connected!"));
		return;
	}
	AirSimClient->armDisarm(true);
	UE_LOG(LogTemp, Log, TEXT("Drone Armed"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Drone Armed"));
}

// Disarm the drone
void AGroundControlStation::DisarmDrone()
{
	if (!bIsConnected) {
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Drone not connected!"));
		return;
	}
	AirSimClient->armDisarm(false);
	UE_LOG(LogTemp, Log, TEXT("Drone Disarmed"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Drone Disarmed"));
}

void AGroundControlStation::TakeOffToHeight(float TargetHeight)
{
	if (!bIsConnected) {
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Drone not connected!"));
		return;
	}

	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, TargetHeight]()
		{
			if (AirSimClient->armDisarm(true))
			{
				UE_LOG(LogTemp, Log, TEXT("Drone Armed"));
				AsyncTask(ENamedThreads::GameThread, []() {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Drone Armed"));
					});
			}

			// Execute takeoff in background thread
			AirSimClient->takeoffAsync()->waitOnLastTask();

			UE_LOG(LogTemp, Log, TEXT("Drone Taking Off"));
			AsyncTask(ENamedThreads::GameThread, []() {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Drone Taking Off"));
				});
		});
}

void AGroundControlStation::Takeoff()
{
	TakeOffToHeight(1.5f);
}

void AGroundControlStation::CheckConnection()
{
	try
	{
		AirSimClient->confirmConnection();
		//auto home_point = AirSimClient->getHomeGeoPoint(); // Try to confirm connection
		AirSimClient->enableApiControl(true);
		if (!bIsConnected)
		{
			bIsConnected = true; // Mark as connected
			UE_LOG(LogTemp, Log, TEXT("AirSim successfully connected!"));
		}
	}
	catch (const std::exception&)
	{
		if (bIsConnected)
		{
			bIsConnected = false; // Mark as disconnected
			UE_LOG(LogTemp, Warning, TEXT("Lost connection to AirSim! Retrying..."));

			delete AirSimClient;
			AirSimClient = new msr::airlib::MultirotorRpcLibClient();
		}
	}
}

void AGroundControlStation::GetTelemetryData()
{
	if (!bIsConnected) return;

	TArray<FTelemetryData> NewTelemetryData;

	for (const FString& UAVName : ListUAVName)
	{
		// Get flow ID for network simulation
		int32 FlowId = 1; // Placeholder

		SimulateNetworkRequest(FlowId, [this, UAVName, &NewTelemetryData]()
			{
				AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, UAVName, &NewTelemetryData]()
					{
						try
						{
							// Get drone state from AirSim
							auto drone_state = AirSimClient->getMultirotorState(TCHAR_TO_UTF8(*UAVName));

							FTelemetryData Telemetry;
							Telemetry.Name = UAVName;
							Telemetry.Position = FVector(drone_state.getPosition().x(), drone_state.getPosition().y(), -drone_state.getPosition().z());
							Telemetry.Velocity = FVector(drone_state.kinematics_estimated.accelerations.linear.x(), drone_state.kinematics_estimated.accelerations.linear.y(), drone_state.kinematics_estimated.accelerations.linear.z());
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
						catch (const std::exception& e)
						{
							UE_LOG(LogTemp, Error, TEXT("Exception in GetLastestVideoFrame: %s"), UTF8_TO_TCHAR(e.what()));
							bIsConnected = false;
						}
						
					});
			});
	}
}

void AGroundControlStation::GetLastestVideoFrame(FString UAVName)
{
	if (!bIsConnected) return;

	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, UAVName]()
		{
			// Get flow ID for network simulation
			int32 FlowId = 1; // Placeholder
			
			SimulateNetworkRequest(FlowId, [this, UAVName]()
				{
					/*int TargetWidth = 1920;
					int TargetHeight = 1080;*/

					AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, UAVName]() {
						// Get latest video frame from AirSim
						try
						{
							std::vector<uint8_t> ImageData = AirSimClient->simGetImage("0", msr::airlib::ImageCaptureBase::ImageType::Scene, TCHAR_TO_UTF8(*UAVName));

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

							AsyncTask(ENamedThreads::GameThread, [this, UAVName, DecodedImage]()
								{
									UTexture2D* VideoFrame = ConvertImageToTexture(DecodedImage);

									if (VideoFrame)
									{
										HandleVideoFrame(UAVName, VideoFrame);
									}
								});
						}
						catch (const std::exception& e)
						{
							UE_LOG(LogTemp, Error, TEXT("Exception in GetLastestVideoFrame: %s"), UTF8_TO_TCHAR(e.what()));
							bIsConnected = false;
						}
					});
				});
		});
}

void AGroundControlStation::SimulateNetworkRequest(int32 FlowId, TFunction<void()> RequestFunction)
{
	// Get network data from the network state instance
	UNetworkStateInstance* NetworkStateInstance = Cast<UNetworkStateInstance>(GetGameInstance());
	if (!NetworkStateInstance || !NetworkStateInstance->GetFlowData().Contains(FlowId))
	{
		//UE_LOG(LogTemp, Error, TEXT("Network state instance not found or flow data not available!"));
		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, RequestFunction);
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

	// Simulate network delay
	Async(EAsyncExecution::Thread, [Delay, Jitter, RequestFunction]()
	{
		FPlatformProcess::Sleep(FMath::FRandRange((Delay - Jitter) / 1000000, (Delay + Jitter) / 1000000));
		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, RequestFunction);
	});
}

UTexture2D* AGroundControlStation::ConvertImageToTexture(const cv::Mat& Image)
{
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

	if (!Texture)
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
	if (!VideoTexture)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to handle video frame for %s"), *UAVName);
		return;
	}
	// Broadcast video frame received event
	OnVideoFrameReceived.Broadcast(UAVName, VideoTexture);
}

// Cleanup
AGroundControlStation::~AGroundControlStation()
{
	if (AirSimClient) {
		delete AirSimClient;
		AirSimClient = nullptr;
	}
}