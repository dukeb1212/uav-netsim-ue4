// Fill out your copyright notice in the Description page of Project Settings.


#include "GroundControlStation.h"

// Sets default values
AGroundControlStation::AGroundControlStation()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize AirSim Client
	AirSimClient = new msr::airlib::MultirotorRpcLibClient();
}

// Called when the game starts or when spawned
void AGroundControlStation::BeginPlay()
{
	Super::BeginPlay();
	
	// Confirm connection with AirSim
	AirSimClient->confirmConnection();
}

// Called every frame
void AGroundControlStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGroundControlStation::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AGroundControlStation::ArmDrone()
{
	if (AirSimClient)
	{
		AirSimClient->armDisarm(true);
		UE_LOG(LogTemp, Log, TEXT("Drone Armed"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Drone Armed"));
	}
}

// Disarm the drone
void AGroundControlStation::DisarmDrone()
{
	if (AirSimClient)
	{
		AirSimClient->armDisarm(false);
		UE_LOG(LogTemp, Log, TEXT("Drone Disarmed"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Drone Disarmed"));
	}
}

// Cleanup
AGroundControlStation::~AGroundControlStation()
{
	if (AirSimClient)
	{
		delete AirSimClient;
		AirSimClient = nullptr;
	}
}