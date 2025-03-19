// Fill out your copyright notice in the Description page of Project Settings.


#include "GroundControlStation.h"
#include "Engine/Engine.h"
#include "NetworkStateInstance.h"
#include "Async/Async.h"
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
	for (FString UAVName : ListUAVName)
	{
		AllLandedStates.Add(UAVName, msr::airlib::LandedState::Landed);
	}
}

// Called when the game starts or when spawned
void AGroundControlStation::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(ConnectionCheckTimer, this, &AGroundControlStation::CheckConnection, 0.01f, true);
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

	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this]()
		{
			if (bShuttingDown || IsEngineExitRequested() || IsGarbageCollecting()) return;
			try
			{
				AirSimClient->enableApiControl(true);
				if (!bIsConnected)
				{
					bIsConnected = true;
					AsyncTask(ENamedThreads::GameThread, []()
						{
							UE_LOG(LogTemp, Log, TEXT("AirSim successfully connected!"));
						});
				}
			}
			catch (const std::exception&)
			{
				if (bIsConnected)
				{
					bIsConnected = false;
					AsyncTask(ENamedThreads::GameThread, []()
						{
							UE_LOG(LogTemp, Warning, TEXT("Lost connection to AirSim! Retrying..."));
						});

					AirSimClient = std::make_unique<msr::airlib::MultirotorRpcLibClient>();
				}
			}
		});
}

void AGroundControlStation::ArmDrone(FString UAVName)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	TWeakObjectPtr<AGroundControlStation> WeakThis = this;
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis, UAVName]()
		{
			if (!WeakThis.IsValid() || WeakThis.Get()->bShuttingDown || IsEngineExitRequested() || IsGarbageCollecting()) return;

			try 
			{
				WeakThis.Get()->AirSimClient->cancelLastTask(TCHAR_TO_UTF8(*UAVName));
				WeakThis.Get()->AirSimClient->armDisarm(true, TCHAR_TO_UTF8(*UAVName));
			}
			catch (const std::exception& e)
			{
				UE_LOG(LogTemp, Error, TEXT("Exception in ArmDrone: %s"), UTF8_TO_TCHAR(e.what()));
				WeakThis.Get()->bIsConnected = false;
			}
			
			UE_LOG(LogTemp, Log, TEXT("Drone Armed"));
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Drone Armed"));
		});
}

void AGroundControlStation::DisarmDrone(FString UAVName)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}
	TWeakObjectPtr<AGroundControlStation> WeakThis = this;
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis, UAVName]()
		{
			if (!WeakThis.IsValid() || WeakThis.Get()->bShuttingDown || IsEngineExitRequested() || IsGarbageCollecting()) return;

			try
			{
				WeakThis.Get()->AirSimClient->cancelLastTask(TCHAR_TO_UTF8(*UAVName));
				WeakThis.Get()->AirSimClient->armDisarm(false, TCHAR_TO_UTF8(*UAVName));
			}
			catch (const std::exception& e)
			{
				UE_LOG(LogTemp, Error, TEXT("Exception in ArmDrone: %s"), UTF8_TO_TCHAR(e.what()));
				WeakThis.Get()->bIsConnected = false;
			}

			UE_LOG(LogTemp, Log, TEXT("Drone Disarmed"));
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Drone Disarmed"));
		});
}

void AGroundControlStation::Takeoff(FString UAVName)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	TWeakObjectPtr<AGroundControlStation> WeakThis = this;
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis, UAVName]()
		{
			if (!WeakThis.IsValid() || WeakThis.Get()->bShuttingDown || IsEngineExitRequested() || IsGarbageCollecting()) return;

			AGroundControlStation* GroundControlStation = WeakThis.Get();
			
			// Execute takeoff async (Timeout after 5 seconds)
			try
			{
				GroundControlStation->AirSimClient->cancelLastTask(TCHAR_TO_UTF8(*UAVName));
				if (GroundControlStation->AirSimClient->armDisarm(true, TCHAR_TO_UTF8(*UAVName)))
				{
					UE_LOG(LogTemp, Log, TEXT("Drone Armed"));
					/*AsyncTask(ENamedThreads::GameThread, []() {
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Drone Armed"));
						});*/
				}
				GroundControlStation->AirSimClient->takeoffAsync(10.0f, TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&GroundControlStation->bTaskResult, 10.0f);
				UE_LOG(LogTemp, Log, TEXT("Drone Taking Off"));
			}
			catch (const std::exception&)
			{
				UE_LOG(LogTemp, Error, TEXT("Drone Takeoff Failed"));
				/*AsyncTask(ENamedThreads::GameThread, []() {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Drone Takeoff Failed"));
					});*/
				return;
			}
			//GroundControlStation->AirSimClient->takeoffAsync(10.0f, TCHAR_TO_UTF8(*UAVName));
			
			/*AsyncTask(ENamedThreads::GameThread, []() {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Drone Taking Off"));
				});*/

			// Check takeoff result
			if (GroundControlStation->bTaskResult)
			{
				GroundControlStation->AllLandedStates.Add(UAVName, msr::airlib::LandedState::Flying);
				UE_LOG(LogTemp, Log, TEXT("Drone Takeoff Successful"));
				/*AsyncTask(ENamedThreads::GameThread, []() {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Drone Takeoff Successful"));
					});*/
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Drone Takeoff Failed"));
				/*AsyncTask(ENamedThreads::GameThread, []() {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Drone Takeoff Failed"));
					});*/
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

	TWeakObjectPtr<AGroundControlStation> WeakThis = this;
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis, UAVName]()
		{
			if (!WeakThis.IsValid() || WeakThis.Get()->bShuttingDown || IsEngineExitRequested() || IsGarbageCollecting()) return;

			AGroundControlStation* GroundControlStation = WeakThis.Get();
			// Execute land async (Timeout after 5 seconds)
			//GroundControlStation->AirSimClient->landAsync(10.0f, TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&GroundControlStation->bTaskResult, 5.0f);
			try
			{
				GroundControlStation->AirSimClient->cancelLastTask(TCHAR_TO_UTF8(*UAVName));
				GroundControlStation->AirSimClient->landAsync(10.0f, TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&GroundControlStation->bTaskResult, 10.0f);
				UE_LOG(LogTemp, Log, TEXT("Drone Landing"));
			}
			catch (const std::exception&)
			{
				UE_LOG(LogTemp, Error, TEXT("Drone Landing Failed"));
				/*AsyncTask(ENamedThreads::GameThread, []() {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Drone Takeoff Failed"));
					});*/
				return;
			}

			
			/*AsyncTask(ENamedThreads::GameThread, []() {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Drone Landing"));
				});*/
			// Check land result
			if (GroundControlStation->bTaskResult)
			{
				GroundControlStation->AllLandedStates.Add(UAVName, msr::airlib::LandedState::Landed);
				UE_LOG(LogTemp, Log, TEXT("Drone Landing Successful"));
				/*AsyncTask(ENamedThreads::GameThread, []() {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Drone Landing Successful"));
					});*/
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Drone Landing Failed"));
				/*AsyncTask(ENamedThreads::GameThread, []() {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Drone Landing Failed"));
					});*/
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

	TWeakObjectPtr<AGroundControlStation> WeakThis = this;
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis, UAVName]()
		{
			if (!WeakThis.IsValid() || WeakThis.Get()->bShuttingDown || IsEngineExitRequested() || IsGarbageCollecting()) return;

			AGroundControlStation* GroundControlStation = WeakThis.Get();

			try
			{
				GroundControlStation->AirSimClient->cancelLastTask(TCHAR_TO_UTF8(*UAVName));
				GroundControlStation->AirSimClient->hoverAsync(TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&GroundControlStation->bTaskResult, 5.0f);
				UE_LOG(LogTemp, Log, TEXT("Drone Hovering"));
			}
			catch (const std::exception&)
			{
				UE_LOG(LogTemp, Error, TEXT("Drone Hovering Failed"));
				/*AsyncTask(ENamedThreads::GameThread, []() {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Drone Takeoff Failed"));
					});*/
				return;
			}

			/*AsyncTask(ENamedThreads::GameThread, []() {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Drone Hovering"));
				});*/
		});
}

int64 AGroundControlStation::GetLandedState(FString UAVName)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return -1;
	}

	TWeakObjectPtr<AGroundControlStation> WeakThis = this;
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis, UAVName]()
		{
			if (!WeakThis.IsValid() || WeakThis.Get()->bShuttingDown || IsEngineExitRequested() || IsGarbageCollecting()) return;

			AGroundControlStation* GroundControlStation = WeakThis.Get();

			try
			{
				GroundControlStation->AllLandedStates.Add(UAVName, GroundControlStation->AirSimClient->getMultirotorState(TCHAR_TO_UTF8(*UAVName)).landed_state);
			}
			catch (const std::exception&)
			{
				UE_LOG(LogTemp, Error, TEXT("Get Landed State Failed"));
				/*AsyncTask(ENamedThreads::GameThread, []() {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Drone Takeoff Failed"));
					});*/
				return;
			}
			
		});
	return (int64)AllLandedStates[UAVName];
}

void AGroundControlStation::MoveToLocation(FString UAVName, FVector Location, float Velocity, float Timeout) 
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	TWeakObjectPtr<AGroundControlStation> WeakThis = this;
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis, UAVName, Location, Velocity, Timeout]()
		{
			if (!WeakThis.IsValid() || WeakThis.Get()->bShuttingDown || IsEngineExitRequested() || IsGarbageCollecting()) return;

			AGroundControlStation* GroundControlStation = WeakThis.Get();

			float TargetZ = GroundControlStation->OffsetZ - Location.Z;
			// Execute move to location async (Timeout after 5 seconds)
			/*GroundControlStation->AirSimClient->moveToPositionAsync(Location.X, Location.Y, TargetZ, Velocity, Timeout,
																	msr::airlib::DrivetrainType::MaxDegreeOfFreedom, msr::airlib::YawMode(),
																	-1, 1, TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&GroundControlStation->bTaskResult, Timeout);*/

			try
			{
				GroundControlStation->AirSimClient->cancelLastTask(TCHAR_TO_UTF8(*UAVName));
				GroundControlStation->AirSimClient->moveToPositionAsync(Location.X, Location.Y, TargetZ, Velocity, Timeout,
					msr::airlib::DrivetrainType::MaxDegreeOfFreedom, msr::airlib::YawMode(),
					-1, 1, TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&GroundControlStation->bTaskResult, Timeout);
				UE_LOG(LogTemp, Log, TEXT("Drone Moving to Location %f %f %f"), Location.X, Location.Y, Location.Z);
			}
			catch (const std::exception&)
			{
				UE_LOG(LogTemp, Error, TEXT("Drone Move to Location Failed"));
				/*AsyncTask(ENamedThreads::GameThread, []() {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Drone Takeoff Failed"));
					});*/
				return;
			}
			
			/*AsyncTask(ENamedThreads::GameThread, []() {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Drone Moving to target location"));
				});*/
			// Check move to location result
			if (GroundControlStation->bTaskResult)
			{
				UE_LOG(LogTemp, Log, TEXT("Drone Move to Location Successful"));
				/*AsyncTask(ENamedThreads::GameThread, []() {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Drone Move to Location Successful"));
					});*/
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Drone Move to Location Failed"));
				/*AsyncTask(ENamedThreads::GameThread, []() {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Drone Move to Location Failed"));
					});*/
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

	TWeakObjectPtr<AGroundControlStation> WeakThis = this;
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis, UAVName, Path, Velocity, Timeout]()
		{
			if (!WeakThis.IsValid() || WeakThis.Get()->bShuttingDown || IsEngineExitRequested() || IsGarbageCollecting()) return;

			AGroundControlStation* GroundControlStation = WeakThis.Get();
			
			std::vector<msr::airlib::Vector3r> Waypoints;
			for (FVector Point : Path)
			{
				Waypoints.push_back(msr::airlib::Vector3r(Point.X, Point.Y, Point.Z));
			}

			try
			{
				GroundControlStation->AirSimClient->cancelLastTask(TCHAR_TO_UTF8(*UAVName));
				GroundControlStation->AirSimClient->moveOnPathAsync(Waypoints, Velocity, Timeout, msr::airlib::DrivetrainType::MaxDegreeOfFreedom, msr::airlib::YawMode(),
					-1, 1, TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&GroundControlStation->bTaskResult, Timeout);

				UE_LOG(LogTemp, Log, TEXT("Drone Moving by Path"));
			}
			catch (const std::exception&)
			{
				UE_LOG(LogTemp, Error, TEXT("Drone Move by Path Failed"));
				/*AsyncTask(ENamedThreads::GameThread, []() {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Drone Takeoff Failed"));
					});*/
				return;
			}

			
			/*AsyncTask(ENamedThreads::GameThread, []() {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Drone Moving by Path"));
				});*/
			// Check move by path result
			if (GroundControlStation->bTaskResult)
			{
				UE_LOG(LogTemp, Log, TEXT("Drone Move by Path Successful"));
				/*AsyncTask(ENamedThreads::GameThread, []() {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Drone Move by Path Successful"));
					});*/
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Drone Move by Path Failed"));
				/*AsyncTask(ENamedThreads::GameThread, []() {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Drone Move by Path Failed"));
					});*/
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

	TWeakObjectPtr<AGroundControlStation> WeakThis = this;
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis, UAVName, VelocityXY, Z, Timeout]()
		{
			if (!WeakThis.IsValid() || WeakThis.Get()->bShuttingDown || IsEngineExitRequested() || IsGarbageCollecting()) return;

			AGroundControlStation* GroundControlStation = WeakThis.Get();

			try
			{
				// Go to target Z first
				/*GroundControlStation->AirSimClient->moveToZAsync(Z, 1.0f, Z, msr::airlib::YawMode(), -1, 1,
					TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&GroundControlStation->bTaskResult, Z*2);*/

				GroundControlStation->AirSimClient->cancelLastTask(TCHAR_TO_UTF8(*UAVName));
				// Move by velocity with same Z
				GroundControlStation->AirSimClient->moveByVelocityZAsync(VelocityXY.X, VelocityXY.Y, -Z, Timeout, msr::airlib::DrivetrainType::MaxDegreeOfFreedom, msr::airlib::YawMode(),
					TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&GroundControlStation->bTaskResult, Timeout*1.5);

				UE_LOG(LogTemp, Log, TEXT("Drone Moving by Velocity with same Z"));
			}
			catch (const std::exception&)
			{
				UE_LOG(LogTemp, Error, TEXT("Drone Move by Velocity with same Z Failed"));
				/*AsyncTask(ENamedThreads::GameThread, []() {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Drone Takeoff Failed"));
					});*/
				return;
			}


			/*AsyncTask(ENamedThreads::GameThread, []() {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Drone Moving by Path"));
				});*/
				// Check move by path result
			if (GroundControlStation->bTaskResult)
			{
				UE_LOG(LogTemp, Log, TEXT("Drone Move by Velocity with same Z Successful"));
				/*AsyncTask(ENamedThreads::GameThread, []() {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Drone Move by Path Successful"));
					});*/
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Drone Move by Velocity with same Z Failed"));
				/*AsyncTask(ENamedThreads::GameThread, []() {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Drone Move by Path Failed"));
					});*/
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

		SimulateNetworkRequest(FlowId, [this, UAVName]()
			{
				TWeakObjectPtr<AGroundControlStation> WeakThis = this;
				AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis, UAVName]()
					{
						if (!WeakThis.IsValid() || WeakThis.Get()->bShuttingDown || IsEngineExitRequested() || IsGarbageCollecting()) return;
						AGroundControlStation* GroundControlStation = WeakThis.Get();
						try
						{
							if (GroundControlStation) {
								// Get drone state from AirSim
								auto drone_state = GroundControlStation->AirSimClient->getMultirotorState(TCHAR_TO_UTF8(*UAVName));
								if (GroundControlStation->OffsetZ == -1212) GroundControlStation->OffsetZ = drone_state.getPosition().z();
								float CurrentZ = GroundControlStation->OffsetZ - drone_state.getPosition().z();
								float CurrenAltitude = GroundControlStation->OffsetZ - drone_state.gps_location.altitude;

								FTelemetryData Telemetry;
								Telemetry.Timestamp = drone_state.timestamp;
								Telemetry.Name = UAVName;
								Telemetry.Position = FVector(drone_state.getPosition().x(), drone_state.getPosition().y(), CurrentZ);
								Telemetry.GPS = FVector(drone_state.gps_location.latitude, drone_state.gps_location.longitude, CurrenAltitude);
								Telemetry.Velocity = FVector(drone_state.kinematics_estimated.twist.linear.x(), drone_state.kinematics_estimated.twist.linear.y(), drone_state.kinematics_estimated.twist.linear.z());
								Telemetry.BatteryLevel = 100.0f;  // Placeholder
								Telemetry.IsConnected = GroundControlStation->AirSimClient->isApiControlEnabled(TCHAR_TO_UTF8(*UAVName));

								// Push telemetry data to the main thread
								AsyncTask(ENamedThreads::GameThread, [WeakThis, Telemetry]()
									{
										if (!WeakThis.IsValid() || WeakThis.Get()->bShuttingDown || IsEngineExitRequested()) return;

										AGroundControlStation* GroundControlStation = WeakThis.Get();
										// Check if telemetry data for this UAV already exists
										for (FTelemetryData& ExistingTelemetry : GroundControlStation->ListTelemetryData)
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
										GroundControlStation->ListTelemetryData.Add(Telemetry);
										//UE_LOG(LogTemp, Log, TEXT("Telemetry added for %s"), *Telemetry.Name);
									});
							}
						}
						catch (const std::exception& e)
						{
							UE_LOG(LogTemp, Error, TEXT("Exception in GetLastestVideoFrame: %s"), UTF8_TO_TCHAR(e.what()));
							WeakThis.Get()->bIsConnected = false;
						}
						
						
					});
			});
	}
}

void AGroundControlStation::GetLastestVideoFrame(FString UAVName, bool IsCapture, FString FilePath)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, UAVName, IsCapture, FilePath]()
		{
			// Get flow ID for network simulation
			int32 FlowId = 1; // Placeholder
			
			SimulateNetworkRequest(FlowId, [this, UAVName, IsCapture, FilePath]()
				{
					/*int TargetWidth = 1920;
					int TargetHeight = 1080;*/
					TWeakObjectPtr<AGroundControlStation> WeakThis = this;

					AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis, UAVName, IsCapture, FilePath]() {
						// Get latest video frame from AirSim
						if (!WeakThis.IsValid() || WeakThis.Get()->bShuttingDown || IsEngineExitRequested() || IsGarbageCollecting()) return;
						AGroundControlStation* GroundControlStation = WeakThis.Get();

						if (!GroundControlStation->AirSimClient || !GroundControlStation->bIsConnected) {
							return;
						}

						try
						{
							std::vector<uint8_t> ImageData = GroundControlStation->AirSimClient->simGetImage("front_center", msr::airlib::ImageCaptureBase::ImageType::Scene, TCHAR_TO_UTF8(*UAVName));

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


							AsyncTask(ENamedThreads::GameThread, [WeakThis, UAVName, DecodedImage]()
								{
									if (!WeakThis.IsValid() || WeakThis.Get()->bShuttingDown || IsEngineExitRequested()) return;

									AGroundControlStation* GroundControlStation = WeakThis.Get();
									if (GroundControlStation) {
										try
										{
											// Convert image to texture
											UTexture2D* VideoFrame = GroundControlStation->ConvertImageToTexture(DecodedImage);
											if (VideoFrame) {
												GroundControlStation->HandleVideoFrame(UAVName, VideoFrame);
											}
										}
										catch (const std::exception& e)
										{
											UE_LOG(LogTemp, Error, TEXT("Exception in GetLastestVideoFrame: %s"), UTF8_TO_TCHAR(e.what()));
											if (WeakThis.IsValid()) WeakThis.Get()->bIsConnected = false;
										}
									}
								});
						}
						catch (const std::exception& e)
						{
							UE_LOG(LogTemp, Error, TEXT("Exception in GetLastestVideoFrame: %s"), UTF8_TO_TCHAR(e.what()));
							if (WeakThis.IsValid()) WeakThis.Get()->bIsConnected = false;
						}
					});
				});
		});
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
	if (IsEngineExitRequested()) return;
	if (!VideoTexture)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to handle video frame for %s"), *UAVName);
		return;
	}
	// Broadcast video frame received event
	OnVideoFrameReceived.Broadcast(UAVName, VideoTexture);
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

		//// Reset and disable API control to prevent stuck RPC calls
		//try {
		//	/*AirSimClient->reset();
		//	AirSimClient->enableApiControl(false);
		//	AirSimClient.release();*/
		//	for (FString UAVName : ListUAVName)
		//	{
		//		AirSimClient->cancelLastTask(TCHAR_TO_UTF8(*UAVName));
		//	}
		//	AirSimClient->reset();
		//}
		//catch (const std::exception& e) {
		//	UE_LOG(LogTemp, Error, TEXT("Exception during AirSim shutdown: %s"), *FString(e.what()));
		//}
		/*try
		{
			AirSimClient->reset();
		}
		catch (const std::exception& e)
		{
			UE_LOG(LogTemp, Error, TEXT("Exception during AirSim shutdown: %s"), *FString(e.what()));
		}*/
		AirSimClient = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}
