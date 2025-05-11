// Fill out your copyright notice in the Description page of Project Settings.


#include "GroundControlStation.h"
#include "Engine/Engine.h"
#include "Async/Async.h"
#include "Vehicles/Multirotor/FlyingPawn.h"
#include "Kismet/GameplayStatics.h"
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

	Ns3SubscriberComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Ns3SubscriberComponent"));
	Ns3SubscriberComponent->SetChildActorClass(AZmqSubscriber::StaticClass());

	Ns3PublisherComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Ns3PublisherComponent"));
	Ns3PublisherComponent->SetChildActorClass(AZmqPublisher::StaticClass());

	AISubscriberComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("AISubscriberComponent"));
	AISubscriberComponent->SetChildActorClass(AZmqAIFeedback::StaticClass());

	AIPublisherComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("AIPublisherComponent"));
	AIPublisherComponent->SetChildActorClass(AZmqPublisher::StaticClass());
}

// Called when the game starts or when spawned
void AGroundControlStation::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("BeginPlay called."));

	NetworkEffectManager = GetGameInstance()->GetSubsystem<UNetworkEffectManager>();

	if (!NetworkEffectManager)
		UE_LOG(LogTemp, Error, TEXT("No Network Component Found!"));

	NetworkStateInstance = Cast<UNetworkStateInstance>(GetGameInstance());
	
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

	NetworkEffectManager->QueueCommandExecute(1, FDelayExecuteCallback::CreateLambda([this, UAVName](const float& CheckId) {
		if (CheckId == 1) {
			bTaskResult = false;
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
		}));
}

void AGroundControlStation::DisarmDrone(FString UAVName)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	NetworkEffectManager->QueueCommandExecute(1, FDelayExecuteCallback::CreateLambda([this, UAVName](const float& CheckId) {
		if (CheckId == 1) {
			bTaskResult = false;
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
		}));
}

void AGroundControlStation::Takeoff(FString UAVName)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	NetworkEffectManager->QueueCommandExecute(1, FDelayExecuteCallback::CreateLambda([this, UAVName](const float& CheckId) {
		if (CheckId == 1) {
			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, UAVName]()
				{
					bTaskResult = false;
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
		}));
}

void AGroundControlStation::Land(FString UAVName)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	NetworkEffectManager->QueueCommandExecute(1, FDelayExecuteCallback::CreateLambda([this, UAVName](const float& CheckId) {
		if (CheckId == 1) {
			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, UAVName]()
				{
					bTaskResult = false;
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
		}));
}

void AGroundControlStation::Hover(FString UAVName)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	NetworkEffectManager->QueueCommandExecute(1, FDelayExecuteCallback::CreateLambda([this, UAVName](const float& CheckId) {
		if (CheckId == 1) {
			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, UAVName]()
				{
					bTaskResult = false;
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
		}));
}

int64 AGroundControlStation::GetLandedState(FString UAVName)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return -1;
	}

	int64 state = -1;

	NetworkEffectManager->QueueCommandExecute(1, FDelayExecuteCallback::CreateLambda([this, UAVName, &state](const float& CheckId) {
		if (CheckId == 1) {
			try {
				AllLandedStates.Add(UAVName, AirSimClient->getMultirotorState(TCHAR_TO_UTF8(*UAVName)).landed_state);
				state = (int64)AllLandedStates[UAVName];
			}
			catch (const std::exception& e) {
				UE_LOG(LogTemp, Error, TEXT("Get Landed State Failed: %s"), UTF8_TO_TCHAR(e.what()));
				state = -1;
			}
		}
		else state = -1;
		}));

	return state;
}

void AGroundControlStation::MoveToLocation(FString UAVName, float DestinationYaw, float UAVYaw, FVector Location, float Velocity, float Timeout)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	NetworkEffectManager->QueueCommandExecute(1, FDelayExecuteCallback::CreateLambda([this, UAVName, DestinationYaw, UAVYaw, Location, Velocity, Timeout](const float& CheckId) {
		if (CheckId == 1) {
			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, UAVName, DestinationYaw, UAVYaw, Location, Velocity, Timeout]()
				{
					bTaskResult = false;
					try {
						float TargetZ = OffsetZ - Location.Z/100 - 1;
						AirSimClient->cancelLastTask(TCHAR_TO_UTF8(*UAVName));
						AirSimClient->moveToPositionAsync(Location.X/100, Location.Y/100, TargetZ, Velocity, Timeout,
							msr::airlib::DrivetrainType::MaxDegreeOfFreedom, msr::airlib::YawMode(),
							-1, 1, TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&bTaskResult, Timeout);

						if (UAVYaw <= DestinationYaw - 5 || UAVYaw >= DestinationYaw + 5)
						{
							AirSimClient->rotateToYawAsync(DestinationYaw, Timeout, 2.f, TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&bTaskResult, Timeout);
							AirSimClient->hoverAsync(TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&bTaskResult, Timeout);
						}
						else Hover(UAVName);
						UE_LOG(LogTemp, Log, TEXT("Drone Moving to Location %f %f %f: %s from Z = %f"), Location.X, Location.Y, Location.Z, *UAVName, OffsetZ);
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
		}));
}

void AGroundControlStation::MoveByPath(FString UAVName, const TArray<FVector>& Path, float Velocity, float Timeout)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	NetworkEffectManager->QueueCommandExecute(1, FDelayExecuteCallback::CreateLambda([this, UAVName, Path, Velocity, Timeout](const float& CheckId) {
		if (CheckId == 1) {
			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, UAVName, Path, Velocity, Timeout]()
				{
					bTaskResult = false;
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

						/*for (const FVector& location : Path)
						{
							MoveToLocation(UAVName, location, Velocity, Timeout);
						}*/
					}
					catch (const std::exception& e) {
						UE_LOG(LogTemp, Error, TEXT("Move by path error: %s"), UTF8_TO_TCHAR(e.what()));
					}
				});
		}
		}));
}

void AGroundControlStation::MoveByVelocitySameZ(FString UAVName, FVector2D VelocityXY, float Z, float Timeout)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	NetworkEffectManager->QueueCommandExecute(1, FDelayExecuteCallback::CreateLambda([this, UAVName, VelocityXY, Z, Timeout](const float& CheckId) {
		if (CheckId == 1) {
			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, UAVName, VelocityXY, Z, Timeout]()
				{
					bTaskResult = false;
					try {
						// Go to target Z first
						//AirSimClient->moveToZAsync(Z, 1.0f, Z, msr::airlib::YawMode(), -1, 1, TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&bTaskResult, Z * 2);
						AirSimClient->cancelLastTask(TCHAR_TO_UTF8(*UAVName));
						// Move by velocity with same Z
						AirSimClient->moveByVelocityZBodyFrameAsync(VelocityXY.X, VelocityXY.Y, -Z, Timeout, msr::airlib::DrivetrainType::MaxDegreeOfFreedom, msr::airlib::YawMode(),
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
		}));
}

void AGroundControlStation::RotateByYawRate(FString UAVName, float YawRate, float Duration)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	NetworkEffectManager->QueueCommandExecute(1, FDelayExecuteCallback::CreateLambda([this, UAVName, YawRate, Duration](const float& CheckId) {
		if (CheckId == 1) {
			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, UAVName, YawRate, Duration]()
				{
					bTaskResult = false;
					try {
						// Go to target Z first
						//AirSimClient->moveToZAsync(Z, 1.0f, Z, msr::airlib::YawMode(), -1, 1, TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&bTaskResult, Z * 2);
						AirSimClient->cancelLastTask(TCHAR_TO_UTF8(*UAVName));
						// Move by velocity with same Z
						AirSimClient->rotateByYawRateAsync(YawRate, Duration, TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&bTaskResult);
						UE_LOG(LogTemp, Log, TEXT("Drone Rotate by YawRate: %s"), *UAVName);
						if (bTaskResult)
						{
							UE_LOG(LogTemp, Log, TEXT("Drone Rotate by YawRate Successful: %s"), *UAVName);
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("Drone Rotate by YawRate Failed: %s"), *UAVName);
						}
					}
					catch (const std::exception& e) {
						UE_LOG(LogTemp, Error, TEXT("Rotate by YawRate error: %s"), UTF8_TO_TCHAR(e.what()));
					}
				});
		}
		}));
}

void AGroundControlStation::RotateToYaw(FString UAVName, float Yaw, float Margin, float Timeout)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	NetworkEffectManager->QueueCommandExecute(1, FDelayExecuteCallback::CreateLambda([this, UAVName, Yaw, Margin, Timeout](const float& CheckId) {
		if (CheckId == 1) {
			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, UAVName, Yaw, Margin, Timeout]()
				{
					bTaskResult = false;
					try {
						// Go to target Z first
						//AirSimClient->moveToZAsync(Z, 1.0f, Z, msr::airlib::YawMode(), -1, 1, TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&bTaskResult, Z * 2);
						AirSimClient->cancelLastTask(TCHAR_TO_UTF8(*UAVName));
						// Move by velocity with same Z
						AirSimClient->rotateToYawAsync(Yaw, Timeout, Margin, TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&bTaskResult, Timeout * 1.5f);
						UE_LOG(LogTemp, Log, TEXT("Drone Rotate to Yaw: %s"), *UAVName);
						if (bTaskResult)
						{
							UE_LOG(LogTemp, Log, TEXT("Drone Rotate to Yaw Successful: %s"), *UAVName);
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("Drone Rotate to Yaw Failed: %s"), *UAVName);
						}
					}
					catch (const std::exception& e) {
						UE_LOG(LogTemp, Error, TEXT("Rotate to Yaw error: %s"), UTF8_TO_TCHAR(e.what()));
					}
				});
		}
		}));
}

void AGroundControlStation::MoveToZ(FString UAVName, float Z, float Velocity, float Timeout)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	NetworkEffectManager->QueueCommandExecute(1, FDelayExecuteCallback::CreateLambda([this, UAVName, Z, Velocity, Timeout](const float& CheckId) {
		if (CheckId == 1) {
			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, UAVName, Z, Velocity, Timeout]()
				{
					bTaskResult = false;
					try {
						// Go to target Z first
						//AirSimClient->moveToZAsync(Z, 1.0f, Z, msr::airlib::YawMode(), -1, 1, TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&bTaskResult, Z * 2);
						AirSimClient->cancelLastTask(TCHAR_TO_UTF8(*UAVName));
						// Move by velocity with same Z
						AirSimClient->moveToZAsync(Z, Velocity, Timeout, msr::airlib::YawMode(), -1, 1, TCHAR_TO_UTF8(*UAVName))->waitOnLastTask(&bTaskResult, Timeout * 1.5f);
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
		}));
}

void AGroundControlStation::CircleAroundPoint(FString UAVName, FVector TargetLocation, float Radius, float Velocity, float Duration)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

	NetworkEffectManager->QueueCommandExecute(1, FDelayExecuteCallback::CreateLambda([this, UAVName, TargetLocation, Radius, Velocity, Duration](const float& CheckId) {
		if (CheckId == 1) {
			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, UAVName, TargetLocation, Radius, Velocity, Duration]()
				{
					bTaskResult = false;

					try {
						// Ensure API control is enabled for the drone
						if (!AirSimClient->isApiControlEnabled(TCHAR_TO_UTF8(*UAVName))) {
							UE_LOG(LogTemp, Error, TEXT("API control not enabled for drone: %s"), *UAVName);
							return;
						}

						// Cancel any previous task
						AirSimClient->cancelLastTask(TCHAR_TO_UTF8(*UAVName));

						// Calculate the total time for one complete circle
						const float CircleCircumference = 2.0f * PI * Radius;
						const float TimeForOneCircle = CircleCircumference / Velocity;

						// Determine the number of steps based on the duration
						const int32 Steps = FMath::CeilToInt(Duration / TimeForOneCircle);

						// Start the circular flight
						float CurrentAngle = 0.0f;
						const float AngleStep = (2.0f * PI) / Steps; // Incremental angle per step

						for (int32 i = 0; i < Steps; ++i) {
							// Calculate the current position on the circle
							float TargetZ = OffsetZ - TargetLocation.Z / 100 - 1;
							float X = TargetLocation.X + Radius * FMath::Cos(CurrentAngle);
							float Y = TargetLocation.Y + Radius * FMath::Sin(CurrentAngle);
							float Z = TargetLocation.Z; // Maintain the same altitude as the target

							// Move the UAV to the calculated position
							FVector Position(X, Y, TargetZ);
							AirSimClient->moveToPositionAsync(
								Position.X / 100, Position.Y / 100, Position.Z,
								Velocity,
								Duration / Steps+10,
								msr::airlib::DrivetrainType::MaxDegreeOfFreedom,
								msr::airlib::YawMode(true, 0), // Keep yaw facing the target
								-1, 1, TCHAR_TO_UTF8(*UAVName)
							)->waitOnLastTask(&bTaskResult, Duration / Steps+10);

							// Update the angle for the next step
							CurrentAngle += AngleStep;

							// Check if the task was successful
							if (!bTaskResult) {
								UE_LOG(LogTemp, Error, TEXT("Circular flight failed at step %d"), i);
								break;
							}
						}

						UE_LOG(LogTemp, Log, TEXT("Drone Circular Flight Completed: %s"), *UAVName);

					}
					catch (const std::exception& e) {
						UE_LOG(LogTemp, Error, TEXT("Circular flight error: %s"), UTF8_TO_TCHAR(e.what()));
					}
				});
		}
		}));
}

// ==================================== DATA COLLECTOR FUNCTIONS ====================================

void AGroundControlStation::GetAllTelemetryData()
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
		//if (NetworkStateInstance && NetworkStateInstance->GetFlowData().Contains(FlowId))
		//{
		//	const FFlowData& FlowData = NetworkStateInstance->GetFlowData()[FlowId];

		//	float Delay = FlowData.MeanDelay / 1000000; // microseconds -> seconds
		//	NetworkEffectManager->SetNetworkParameters(Delay);
		//}
		//else {
		//	float Delay = 0.001; // Default to 1 ms
		//	NetworkEffectManager->SetNetworkParameters(Delay);
		//}

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

				NetworkEffectManager->QueueTelemetryUpdate(Telemetry, 1, FDelayedTelemetryCallback::CreateLambda([this](const FTelemetryData& DelayedTelemetry) {
					// Push telemetry data to the main thread
					AsyncTask(ENamedThreads::GameThread, [this, DelayedTelemetry]()
						{
							// Check if telemetry data for this UAV already exists
							for (FTelemetryData& ExistingTelemetry : ListTelemetryData)
							{
								if (ExistingTelemetry.Name == DelayedTelemetry.Name)
								{
									// Update existing telemetry entry
									ExistingTelemetry = DelayedTelemetry;
									//UE_LOG(LogTemp, Log, TEXT("Telemetry updated for %s"), *Telemetry.Name);
									return;
								}
							}
							// If not found, add as a new entry
							ListTelemetryData.Add(DelayedTelemetry);
							//UE_LOG(LogTemp, Log, TEXT("Telemetry added for %s"), *Telemetry.Name);
						});
					}));
				
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

void AGroundControlStation::GetTelemetryDataByName(FString UAVName, int32 FlowId)
{
	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
	{
		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
		return;
	}

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

			NetworkEffectManager->QueueTelemetryUpdate(Telemetry, FlowId, FDelayedTelemetryCallback::CreateLambda([this](const FTelemetryData& DelayedTelemetry) {
				// Push telemetry data to the main thread
				AsyncTask(ENamedThreads::GameThread, [this, DelayedTelemetry]()
					{
						// Check if telemetry data for this UAV already exists
						for (FTelemetryData& ExistingTelemetry : ListTelemetryData)
						{
							if (ExistingTelemetry.Name == DelayedTelemetry.Name)
							{
								// Update existing telemetry entry
								ExistingTelemetry = DelayedTelemetry;
								//UE_LOG(LogTemp, Log, TEXT("Telemetry updated for %s"), *Telemetry.Name);
								return;
							}
						}
						// If not found, add as a new entry
						ListTelemetryData.Add(DelayedTelemetry);
						//UE_LOG(LogTemp, Log, TEXT("Telemetry added for %s"), *Telemetry.Name);
					});
				}));

		}
		else {
			UE_LOG(LogTemp, Error, TEXT("API control not enabled for drone: %s"), *UAVName);
		}
	} catch (const std::exception& e) {
		UE_LOG(LogTemp, Error, TEXT("Get telemetry data error: %s"), UTF8_TO_TCHAR(e.what()));
	}
}

//void AGroundControlStation::GetLastestVideoFrame(FString UAVName, bool IsCapture, FString FilePath)
//{
//	if (bShuttingDown || IsEngineExitRequested() || !bIsConnected || IsGarbageCollecting())
//	{
//		UE_LOG(LogTemp, Error, TEXT("Drone not connected!"));
//		return;
//	}
//
//	try {
//		std::vector<uint8_t> ImageData = AirSimClient->simGetImage("front_center", msr::airlib::ImageCaptureBase::ImageType::Scene, TCHAR_TO_UTF8(*UAVName));
//		if (ImageData.empty())
//		{
//			UE_LOG(LogTemp, Warning, TEXT("Failed to retrieve video frame for %s"), *UAVName);
//			return;
//		}
//		cv::Mat DecodedImage = cv::imdecode(cv::Mat(ImageData), cv::IMREAD_COLOR);
//		if (DecodedImage.empty())
//		{
//			UE_LOG(LogTemp, Error, TEXT("Failed to decode image for %s"), *UAVName);
//			return;
//		}
//		if (IsCapture) {
//			// Define the save folder path
//			FString SaveFolder = FPaths::ProjectDir() + TEXT("Saved/Images/");
//			// Ensure the directory exists
//			if (!FPaths::DirectoryExists(SaveFolder)) {
//				IFileManager::Get().MakeDirectory(*SaveFolder, true);
//			}
//			// Construct the full save path with filename
//			FString SavePath = SaveFolder + UAVName + TEXT("_") + FDateTime::Now().ToString() + TEXT(".png");
//			std::string SavePathStr = TCHAR_TO_UTF8(*SavePath);
//			// Save the image using OpenCV
//			if (!cv::imwrite(SavePathStr, DecodedImage)) {
//				UE_LOG(LogTemp, Error, TEXT("Failed to save image to %s"), *SavePath);
//			}
//			else {
//				UE_LOG(LogTemp, Warning, TEXT("Image saved successfully at: %s"), *SavePath);
//			}
//		}
//		// Convert image to texture
//		UTexture2D* VideoFrame = ConvertImageToTexture(DecodedImage);
//		if (VideoFrame) {
//			HandleVideoFrame(UAVName, VideoFrame);
//		}
//	}
//	catch (const std::exception& e) {
//		UE_LOG(LogTemp, Error, TEXT("Get lastest video frame error: %s"), UTF8_TO_TCHAR(e.what()));
//	}
//}
//
//UTexture2D* AGroundControlStation::ConvertImageToTexture(const cv::Mat& Image)
//{
//	if (IsEngineExitRequested()) return nullptr;
//	if (Image.empty()) return nullptr;
//
//	cv::Mat ProcessedImage;
//	if (Image.type() == CV_8UC3)
//	{
//		cv::cvtColor(Image, ProcessedImage, cv::COLOR_BGR2BGRA);
//	}
//	else if (Image.type() == CV_8UC4)
//	{
//		ProcessedImage = Image.clone(); // Directly use if already in BGRA
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("MatToTexture2D: Unsupported Mat type. Only CV_8UC3 and CV_8UC4 are supported."));
//		return nullptr;
//	}
//
//	// Create texture
//	UTexture2D* Texture = UTexture2D::CreateTransient(ProcessedImage.cols, ProcessedImage.rows, PF_B8G8R8A8);
//
//	if (!Texture || !Texture->PlatformData)
//	{
//		UE_LOG(LogTemp, Error, TEXT("MatToTexture2D: Failed to create texture."));
//		return nullptr;
//	}
//	Texture->SRGB = 0;
//	Texture->AddToRoot();
//
//	// Copy image data to texture
//	uint8* TextureData = (uint8*)Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
//	FMemory::Memcpy(TextureData, ProcessedImage.data, ProcessedImage.total() * ProcessedImage.elemSize());
//	Texture->PlatformData->Mips[0].BulkData.Unlock();
//
//	Texture->UpdateResource();
//	return Texture;
//}


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

void AGroundControlStation::ClearDebugMessages()
{
	if (GEngine)
	{
		GEngine->ClearOnScreenDebugMessages();
	}
}


// ==================================== NETWORK SIMULATION FUNCTIONS ====================================

//void AGroundControlStation::SimulateNetworkRequest(int32 FlowId, TFunction<void()> RequestFunction)
//{
//	// Get network data from the network state instance
//	if (!NetworkStateInstance || !NetworkStateInstance->GetFlowDataMap().Contains(FlowId))
//	{
//		//UE_LOG(LogTemp, Error, TEXT("Network state instance not found or flow data not available!"));
//		TWeakObjectPtr<AGroundControlStation> WeakThis = this;
//		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis, RequestFunction]()
//			{
//				if (!WeakThis.IsValid() || WeakThis.Get()->bShuttingDown || IsEngineExitRequested() || IsGarbageCollecting()) return;
//				RequestFunction();
//			});
//		return;
//	}
//
//	const FFlowData& FlowData = NetworkStateInstance->GetFlowDataMap()[FlowId];
//
//	float Delay = FlowData.MeanDelay; // microseconds
//	float Jitter = FlowData.MeanJitter; // microseconds
//	float PacketLoss = FlowData.PacketLossL3;
//
//	// Simulate network packet loss
//	if (FMath::FRandRange(0.0f,100.0f) < PacketLoss)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Packet loss detected!"));
//		return;
//	}
//
//	TWeakObjectPtr<AGroundControlStation> WeakThis = this;
//	// Simulate network delay
//	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis, Delay, Jitter, RequestFunction]()
//	{
//		if (!WeakThis.IsValid() || WeakThis.Get()->bShuttingDown || IsEngineExitRequested() || IsGarbageCollecting()) return;
//		FPlatformProcess::Sleep(FMath::FRandRange((Delay - Jitter) / 1000000, (Delay + Jitter) / 1000000));
//		
//		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis, RequestFunction]()
//			{
//				if (!WeakThis.IsValid() || WeakThis.Get()->bShuttingDown || IsEngineExitRequested() || IsGarbageCollecting()) return;
//				RequestFunction();
//			});
//	});
//}

void AGroundControlStation::SaveLogCsv(FString FilePath, TArray<FString> Data)
{
	FString SavePath = FilePath + FDateTime::Now().ToString() + TEXT(".csv");
	FString DataLine = FString::Join(Data, TEXT("\n"));
	FFileHelper::SaveStringToFile(DataLine, *SavePath);
	UE_LOG(LogTemp, Log, TEXT("Log saved to: %s"), *SavePath);
}

void AGroundControlStation::RestartLevel()
{
	for (FString UAVName : ListUAVName)
	{
		AirSimClient->cancelLastTask(TCHAR_TO_UTF8(*UAVName));
	}
	NetworkEffectManager->ClearAllQueues();
	NetworkStateInstance->ResetFlowDataMap();
	EndPlay(EEndPlayReason::LevelTransition);
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
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
	AllLandedStates.Empty();
	ListUAVName.Empty();
	ListTelemetryData.Empty();

	Super::EndPlay(EndPlayReason);
}
