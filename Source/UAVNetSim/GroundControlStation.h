
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "vehicles/multirotor/api/MultirotorRpcLibClient.hpp"
#include "PIPCamera.h"
#include "common/common_utils/UniqueValueMap.hpp"
#include "NetworkStateInstance.h"
#include "Network/NetworkEffectManager.h"
#include "Delegates/Delegate.h"
#include "DataStruct/Telemetry.h"

#include "GroundControlStation.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVideoFrameReceived, const FString&, UAVName, UTextureRenderTarget2D*, VideoRenderTarget);

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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	

	UPROPERTY(BlueprintReadOnly)
	bool bIsConnected = false;

	UPROPERTY(BlueprintReadOnly)
	bool bTaskResult;

	TMap<FString, msr::airlib::LandedState> AllLandedStates;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Drone Control Settings")
	//float DefaultAltitude = 1.8f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Drone Data")
	TArray<FTelemetryData> ListTelemetryData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Drone Data")
	TArray<FString> ListUAVName = { TEXT("uav1") };

	UPROPERTY(BlueprintAssignable, Category = "Video Streaming")
	FOnVideoFrameReceived OnVideoFrameReceived;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Function to arm the drone
	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void ArmDrone(FString UAVName);

	// Function to disarm the drone
	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void DisarmDrone(FString UAVName);

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void Takeoff(FString UAVName);

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void Land(FString UAVName);

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void Hover(FString UAVName);

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	int64 GetLandedState(FString UAVName);

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void GetAllTelemetryData();

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void GetTelemetryDataByName(FString UAVName, int32 FlowId);

	/*UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void GetLastestVideoFrame(FString UAVName, bool IsCapture = false, FString FilePath = "");*/

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void MoveToLocation(FString UAVName, FVector Location, float Velocity, float Timeout);

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void MoveByPath(FString UAVName, const TArray<FVector>& Path, float Velocity, float Timeout);

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void MoveByVelocitySameZ(FString UAVName, FVector2D VelocityXY, float Z, float Timeout);

	void HandleVideoFrame(const FString& UAVName, UTexture2D* VideoTexture);

private:
	// AirSim client
	std::unique_ptr<msr::airlib::MultirotorRpcLibClient> AirSimClient;

	FTimerHandle ConnectionCheckTimer;

	float OffsetZ;

	bool bShuttingDown = false;

	UNetworkEffectManager* NetworkEffectManager;

	//void SimulateNetworkRequest(int32 FlowId, TFunction<void()> RequestFunction);

	void CheckConnection();

	/*UTexture2D* ConvertImageToTexture(const cv::Mat& Image);*/
};
