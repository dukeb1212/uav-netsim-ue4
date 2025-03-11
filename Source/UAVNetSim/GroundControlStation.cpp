// Fill out your copyright notice in the Description page of Project Settings.


#include "GroundControlStation.h"
#include "Engine/Engine.h"
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
	
	GetWorldTimerManager().SetTimer(ConnectionCheckTimer, this, &AGroundControlStation::CheckConnection, 2.0f, true);
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
	AirSimClient->armDisarm(true);
	UE_LOG(LogTemp, Log, TEXT("Drone Armed"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Drone Armed"));
	
	AirSimClient->takeoffAsync()->waitOnLastTask();
	UE_LOG(LogTemp, Log, TEXT("Drone Taking Off"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Drone Taking Off"));

	float TargetZ = -TargetHeight; // AirSim uses NED coordinates (negative Z is up)
	float Velocity = 0.2f; // m/s climb rate

	AirSimClient->moveToZAsync(TargetZ, Velocity, 5.0f);
	UE_LOG(LogTemp, Log, TEXT("Reached target height: %.2f meters"), TargetHeight);
}

void AGroundControlStation::Takeoff()
{
	TakeOffToHeight(DefaultAltitude);
}

void AGroundControlStation::CheckConnection()
{
	try
	{
		auto home_point = AirSimClient->getHomeGeoPoint(); // Try to confirm connection
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
			UE_LOG(LogTemp, Warning, TEXT("Lost connection to AirSim!"));
		}
	}
}

// Cleanup
AGroundControlStation::~AGroundControlStation()
{

}