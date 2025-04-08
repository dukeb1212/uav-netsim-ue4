// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "../DataStruct/Telemetry.h"
#include "../NetworkStateInstance.h"
#include "NetworkEffectManager.generated.h"

DECLARE_DELEGATE_OneParam(FDelayedTelemetryCallback, const FTelemetryData&);
DECLARE_DELEGATE_OneParam(FDelayExecuteCallback, const float&);
/**
 * 
 */

USTRUCT()
struct FDelayedTelemetry {
	GENERATED_BODY()
	FTelemetryData Data;
	float RemainingDelay;
	bool bCanSkip;
	FDelayedTelemetryCallback Callback;
};

USTRUCT()
struct FDelayExecute {
	GENERATED_BODY()
	int32 FlowId;
	float PacketLossRate;
	float RemainingDelay;
	bool bCanSkip;
	FDelayExecuteCallback Callback;
};

UCLASS()
class UAVNETSIM_API UNetworkEffectManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	/*UFUNCTION(BlueprintCallable, Category = "Network Effects")
	void SetNetworkParameters(float Delay);*/

	void QueueTelemetryUpdate(const FTelemetryData& TelemetryData, int32 FlowId, FDelayedTelemetryCallback Callback);

	void QueueVideoUpdate(const int32& FlowId, FDelayExecuteCallback Callback);

	void QueueCommandExecute(const int32& FlowId, FDelayExecuteCallback Callback);

	float CalculateDelay(float MeanDelay, float MeanJitter);

	float CalculatePacketLossRate(float PacketLoss, float TxPackets);

private:
	bool Tick(float DeltaTime);

	FDelegateHandle TickDelegateHandle;

	TArray<FDelayedTelemetry> TelemetryQueue;
	
	TArray<FDelayExecute> VideoFrameQueue;

	TArray<FDelayExecute> CommandQueue;

	UNetworkStateInstance* NetworkStateInstance = nullptr;
};
