// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "../DataStruct/Telemetry.h"
#include "NetworkStateInstance.h"
#include "RHICommandList.h"
#include "RenderGraphUtils.h"
#include "RenderTargetPool.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Delegates/DelegateCombinations.h"
#include "UObject/UnrealType.h"
#include "NetworkEffectManager.generated.h"

DECLARE_DELEGATE_OneParam(FDelayedTelemetryCallback, const FTelemetryData&);
DECLARE_DELEGATE_OneParam(FDelayExecuteCallback, const float&);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTextureProcessed, UTexture2D*, Texture);
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

USTRUCT()
struct FDelayedFrame {
	GENERATED_BODY()
	UTexture2D* Texture;
	float RemainingDelay;
	bool bCanSkip;
	FOnTextureProcessed Callback;
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

	void QueueDelayedTexture(UTexture2D* SourceTexture, int32 FlowId, const FOnTextureProcessed& Callback);

	void QueueCommandExecute(const int32& FlowId, FDelayExecuteCallback Callback);

	float CalculateDelay(float MeanDelay, float MeanJitter);

	float CalculatePacketLossRate(float PacketLoss, float TxPackets);

	UFUNCTION(BlueprintCallable, Category = "Network Effect")
	UTexture2D* GetDelayedFrameTexture() const { return DelayedFrameTexture; }


private:
	UPROPERTY()
	UTexture2D* DelayedFrameTexture;

	bool Tick(float DeltaTime);

	FDelegateHandle TickDelegateHandle;

	TArray<FDelayedTelemetry> TelemetryQueue;
	
	TArray<FDelayedFrame> FrameQueue;

	TArray<FDelayExecute> CommandQueue;

	UNetworkStateInstance* NetworkStateInstance = nullptr;
};
