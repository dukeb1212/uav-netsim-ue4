
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "vehicles/multirotor/api/MultirotorRpcLibClient.hpp"
#include "Delegates/Delegate.h"
#include "DataStruct/Telemetry.h"

#ifdef check
#undef check
#endif

#include "opencv2/opencv.hpp"
#include "GroundControlStation.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVideoFrameReceived, const FString&, UAVName, UTexture2D*, VideoTexture);

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

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void GetTelemetryData();

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void GetLastestVideoFrame(FString UAVName);

	void HandleVideoFrame(const FString& UAVName, UTexture2D* VideoTexture);

private:
	FTimerHandle ConnectionCheckTimer;

	void SimulateNetworkRequest(int32 FlowId, TFunction<void()> RequestFunction);

	UTexture2D* ConvertImageToTexture(const cv::Mat& Image);
};
