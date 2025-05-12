// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkEffectManager.h"
#include "../DataStruct/Flow.h"

DEFINE_LOG_CATEGORY_STATIC(LogNetworkEffect, Log, All);

void UNetworkEffectManager::Initialize(FSubsystemCollectionBase& Collection)
{
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UNetworkEffectManager::Tick));
	Super::Initialize(Collection);

	NetworkStateInstance = Cast<UNetworkStateInstance>(GetGameInstance());
	if (!NetworkStateInstance)
		UE_LOG(LogTemp, Error, TEXT("No Network Instance Found!"));
}

void UNetworkEffectManager::Deinitialize()
{
	FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
	Super::Deinitialize();
}

void UNetworkEffectManager::QueueTelemetryUpdate(const FTelemetryData& TelemetryData, int32 FlowId, FDelayedTelemetryCallback Callback)
{
	float Delay = 0.0f;
	float PacketLossRate = 0.0f;
	if (NetworkStateInstance && NetworkStateInstance->ContainsFlowId(FlowId)) {
		const FFlowData& FlowData = *NetworkStateInstance->GetFlowDataById(FlowId);
		Delay = CalculateDelay(FlowData.MeanDelay, FlowData.MeanJitter);
		PacketLossRate = CalculatePacketLossRate(FlowData.PacketLossL3, FlowData.TxPackets);
	}
	FDelayedTelemetry NewItem;
	NewItem.Data = TelemetryData;
	NewItem.RemainingDelay = Delay;
	FMath::FRandRange(0.0f, 1.0f) < PacketLossRate ? NewItem.bCanSkip = true : NewItem.bCanSkip = false;
	NewItem.Callback = Callback;
	TelemetryQueue.Add(NewItem);
}


void UNetworkEffectManager::QueueCommandExecute(const int32& FlowId, FDelayExecuteCallback Callback)
{
	float Delay = 0.0f;
	float PacketLossRate = 0.0f;
	if (NetworkStateInstance && NetworkStateInstance->ContainsFlowId(FlowId)) {
		const FFlowData& FlowData = *NetworkStateInstance->GetFlowDataById(FlowId);
		Delay = CalculateDelay(FlowData.MeanDelay, FlowData.MeanJitter);
		PacketLossRate = CalculatePacketLossRate(FlowData.PacketLossL3, FlowData.TxPackets);
	}
	FDelayExecute NewItem;
	NewItem.FlowId = FlowId;
	NewItem.RemainingDelay = Delay;
	FMath::FRandRange(0.0f, 1.0f) < PacketLossRate ? NewItem.bCanSkip = true : NewItem.bCanSkip = false;
	NewItem.Callback = Callback;
	CommandQueue.Add(NewItem);
}

void UNetworkEffectManager::QueueDelayedRenderTarget(UTextureRenderTarget2D* SourceRenderTarget, int32 FlowId, const FOnRenderTargetProcessed& Callback)
{
	if (!SourceRenderTarget)
	{
		UE_LOG(LogNetworkEffect, Warning, TEXT("SourceRenderTarget is invalid."));
		return;
	}

	float Delay = 0.0f;
	bool bCanSkip = false;
	if (NetworkStateInstance && NetworkStateInstance->ContainsFlowId(FlowId))
	{
		const FFlowData& FlowData = *NetworkStateInstance->GetFlowDataById(FlowId);
		Delay = CalculateDelay(FlowData.MeanDelay, FlowData.MeanJitter);
		bCanSkip = (FMath::FRandRange(0.0f, 1.0f) < CalculatePacketLossRate(FlowData.PacketLossL3, FlowData.TxPackets));
	}

	// Create or reuse a GPU render target for delayed frame
	UTextureRenderTarget2D* DelayedTarget = DuplicateObject(SourceRenderTarget, this);
	DelayedTarget->ClearColor = FLinearColor::Black;
	DelayedTarget->UpdateResourceImmediate(true);

	ENQUEUE_RENDER_COMMAND(CopyRenderTargetCmd)([SourceRenderTarget, DelayedTarget](FRHICommandListImmediate& RHICmdList)
		{
			FRHITexture2D* Src = SourceRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture();
			FRHITexture2D* Dst = DelayedTarget->GetRenderTargetResource()->GetRenderTargetTexture();
			if (Src && Dst)
			{
				FRHICopyTextureInfo CopyInfo;
				RHICmdList.CopyTexture(Src, Dst, CopyInfo);
			}
		});

	// Add to delay queue (game thread)
	FDelayedRenderTargetFrame Frame;
	Frame.RenderTarget = DelayedTarget;
	Frame.RemainingDelay = Delay;
	Frame.bCanSkip = bCanSkip;
	Frame.Callback = Callback;
	RenderTargetQueue.Add(Frame);
}

float UNetworkEffectManager::CalculateDelay(float MeanDelay, float MeanJitter)
{
	return FMath::Max(FMath::FRandRange((MeanDelay - MeanJitter) / 1000000, (MeanDelay + MeanJitter) / 1000000), 0.0f);
}

float UNetworkEffectManager::CalculatePacketLossRate(float PacketLoss, float TxPackets)
{
	return PacketLoss / FMath::Max(TxPackets, 1.0f);
}

void UNetworkEffectManager::ClearAllQueues()
{
	TelemetryQueue.Empty();
	CommandQueue.Empty();
	RenderTargetQueue.Empty();
}

bool UNetworkEffectManager::Tick(float DeltaTime)
{
	// Process Command Queue
	for (int32 i = CommandQueue.Num() - 1; i >= 0; --i) {
		if (CommandQueue[i].bCanSkip) {
			UE_LOG(LogNetworkEffect, Log, TEXT("Skipping Command [FlowId: %d] | Packet Loss"), CommandQueue[i].FlowId);
			CommandQueue.RemoveAt(i);
			continue;
		}
		CommandQueue[i].RemainingDelay -= DeltaTime;
		if (CommandQueue[i].RemainingDelay <= 0) {
			UE_LOG(LogNetworkEffect, Log, TEXT("Executing Command [FlowId: %d] | Delayed %.2fms"), CommandQueue[i].FlowId, DeltaTime);
			CommandQueue[i].Callback.ExecuteIfBound(CommandQueue[i].FlowId);
			CommandQueue.RemoveAt(i);
		}
	}

	// Process Telemetry Queue
	for (int32 i = TelemetryQueue.Num() - 1; i >= 0; --i) {
		if (TelemetryQueue[i].bCanSkip) {
			UE_LOG(LogNetworkEffect, Log, TEXT("Skipping Telemetry | Packet Loss"));
			TelemetryQueue.RemoveAt(i);
			continue;
		}
		TelemetryQueue[i].RemainingDelay -= DeltaTime;
		if (TelemetryQueue[i].RemainingDelay <= 0) {
			TelemetryQueue[i].Callback.ExecuteIfBound(TelemetryQueue[i].Data);
			TelemetryQueue.RemoveAt(i);
		}
	}

	for (int32 i = RenderTargetQueue.Num() - 1; i >= 0; --i)
	{
		FDelayedRenderTargetFrame& Frame = RenderTargetQueue[i];
		Frame.RemainingDelay -= DeltaTime;
		if (Frame.RemainingDelay <= 0.0f)
		{
			if (!Frame.bCanSkip)
			{
				Frame.Callback.Broadcast(Frame.RenderTarget);
			}
			RenderTargetQueue.RemoveAt(i);
		}
	}

	return true;
}

