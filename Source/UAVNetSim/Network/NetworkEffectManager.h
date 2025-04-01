// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "../DataStruct/Telemetry.h"
#include "NetworkEffectManager.generated.h"

DECLARE_DELEGATE_OneParam(FDelayedTelemetryCallback, const FTelemetryData&);
DECLARE_DELEGATE_OneParam(FDelayExecuteCallback, const bool&);
/**
 * 
 */

USTRUCT()
struct FDelayedTelemetry {
	GENERATED_BODY()
	FTelemetryData Data;
	float RemainingDelay;
	FDelayedTelemetryCallback Callback;
};

USTRUCT()
struct FDelayExecute {
	GENERATED_BODY()
	bool bCanExecute;
	float RemainingDelay;
	FDelayExecuteCallback Callback;
};

UCLASS()
class UAVNETSIM_API UNetworkEffectManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Network Effects")
	void SetNetworkParameters(float Delay);

	void QueueTelemetryUpdate(const FTelemetryData& TelemetryData, FDelayedTelemetryCallback Callback);

	void QueueExecuteUpdate(const bool& bCanExecute, FDelayExecuteCallback Callback);

	void QueueCommandExecute(const bool& bCanExecute, FDelayExecuteCallback Callback);

	float CalculateDelay(float MeanDelay, float MeanJitter);


private:
	bool Tick(float DeltaTime);

	FDelegateHandle TickDelegateHandle;

	float CurrentDelay = 0.0f;

	TArray<FDelayedTelemetry> TelemetryQueue;
	
	TArray<FDelayExecute> ExecuteQueue;

	TArray<FDelayExecute> CommandQueue;
};
