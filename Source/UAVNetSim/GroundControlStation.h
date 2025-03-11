
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "vehicles/multirotor/api/MultirotorRpcLibClient.hpp"
#include "GroundControlStation.generated.h"

UCLASS()
class UAVNETSIM_API AGroundControlStation : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGroundControlStation();

	virtual ~AGroundControlStation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// AirSim client
	msr::airlib::MultirotorRpcLibClient* AirSimClient;

public:	

	UPROPERTY(BlueprintReadOnly)
	bool bIsConnected;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Drone Control Settings")
	float DefaultAltitude = 1.8f;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Function to arm the drone
	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void ArmDrone();

	// Function to disarm the drone
	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void DisarmDrone();

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void TakeOffToHeight(float TargetHeight);

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void Takeoff();

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void CheckConnection();

private:
	FTimerHandle ConnectionCheckTimer;

};
