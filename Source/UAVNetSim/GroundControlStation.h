
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "vehicles/multirotor/api/MultirotorRpcLibClient.hpp"
#include "PIPCamera.h"
#include "common/common_utils/UniqueValueMap.hpp"
#include "Network/NetworkStateInstance.h"
#include "Network/NetworkEffectManager.h"
#include "Delegates/Delegate.h"
#include "DataStruct/Telemetry.h"
#include "Zmq/ZmqPublisher.h"
#include "Zmq/ZmqSubscriber.h"
#include "Zmq/ZmqAIFeedback.h"
#include "Components/ChildActorComponent.h"

#include "GroundControlStation.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZmqComponentReady, bool, IsReady);

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

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnZmqComponentReady OnZmqComponentReady;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Drone Control Settings")
	//float DefaultAltitude = 1.8f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Drone Data")
	TArray<FTelemetryData> ListTelemetryData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Drone Data")
	TArray<FString> ListUAVName = { TEXT("uav1") };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ZMQ", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* Ns3SubscriberComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ZMQ", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* AISubscriberComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ZMQ", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* Ns3PublisherComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ZMQ", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* AIPublisherComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ZMQ")
	USceneComponent* ZMQRootComponent;

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
	void MoveToLocation(FString UAVName, float DestinationYaw, float UAVYaw, FVector Location, float Velocity, float Timeout);

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void MoveByPath(FString UAVName, const TArray<FVector>& Path, float Velocity, float Timeout);

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void MoveByVelocitySameZ(FString UAVName, FVector2D VelocityXY, float Z, float Timeout);

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void RotateByYawRate(FString UAVName, float YawRate, float Duration);

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void RotateToYaw(FString UAVName, float Yaw, float Margin, float Timeout);

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void MoveToZ(FString UAVName, float Z, float Velocity, float Timeout);

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void CircleAroundPoint(FString UAVName, FVector TargetLocation, float Radius, float Velocity, float Duration);

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void SaveLogCsv(FString FilePath, TArray<FString> Data);

	UFUNCTION(BlueprintCallable, Category = "Drone Control")
	void RestartLevel();

	void HandleVideoFrame(const FString& UAVName, UTexture2D* VideoTexture);

	UFUNCTION(BlueprintCallable, Category = "Debug")
	static void ClearDebugMessages();


private:
	// AirSim client
	std::unique_ptr<msr::airlib::MultirotorRpcLibClient> AirSimClient;

	FTimerHandle ConnectionCheckTimer;

	float OffsetZ;

	bool bShuttingDown = false;

	UNetworkEffectManager* NetworkEffectManager;

	UNetworkStateInstance* NetworkStateInstance;

	//void SimulateNetworkRequest(int32 FlowId, TFunction<void()> RequestFunction);

	FString StatLogFilePath;

	void CheckConnection();

	void NotifyZmqPublisherReady();

	void InitStatLogging();

	void LogFrameStat();
	/*UTexture2D* ConvertImageToTexture(const cv::Mat& Image);*/
};
