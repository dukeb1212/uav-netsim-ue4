// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkEffectManager.h"
#include "../DataStruct/Flow.h"

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

//void UNetworkEffectManager::SetNetworkParameters(float Delay)
//{
//	CurrentDelay = FMath::Max(Delay, 0.0f);
//}

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

void UNetworkEffectManager::QueueVideoUpdate(const int32& FlowId, FDelayExecuteCallback Callback)
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
	NewItem.PacketLossRate = PacketLossRate;
	NewItem.bCanSkip = false; // Video updates are not skipped
	NewItem.Callback = Callback;
	VideoFrameQueue.Add(NewItem);
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

float UNetworkEffectManager::CalculateDelay(float MeanDelay, float MeanJitter)
{
	return FMath::Max(FMath::FRandRange((MeanDelay - MeanJitter) / 1000000, (MeanDelay + MeanJitter) / 1000000), 0.0f);
}

float UNetworkEffectManager::CalculatePacketLossRate(float PacketLoss, float TxPackets)
{
	return PacketLoss / FMath::Max(TxPackets, 1.0f);
}

bool UNetworkEffectManager::Tick(float DeltaTime)
{
	for (int32 i = TelemetryQueue.Num() - 1; i >= 0; --i) {
		if (TelemetryQueue[i].bCanSkip) {
			TelemetryQueue.RemoveAt(i);
			continue;
		}
		TelemetryQueue[i].RemainingDelay -= DeltaTime;
		if (TelemetryQueue[i].RemainingDelay <= 0) {
			TelemetryQueue[i].Callback.ExecuteIfBound(TelemetryQueue[i].Data);
			TelemetryQueue.RemoveAt(i);
		}
	}

	for (int32 i = VideoFrameQueue.Num() - 1; i >= 0; --i) {
		VideoFrameQueue[i].RemainingDelay -= DeltaTime;
		if (VideoFrameQueue[i].RemainingDelay <= 0) {
			VideoFrameQueue[i].Callback.ExecuteIfBound(VideoFrameQueue[i].PacketLossRate);
			VideoFrameQueue.RemoveAt(i);
		}
	}

	for (int32 i = CommandQueue.Num() - 1; i >= 0; --i) {
		if (CommandQueue[i].bCanSkip) {
			CommandQueue.RemoveAt(i);
			continue;
		}
		CommandQueue[i].RemainingDelay -= DeltaTime;
		if (CommandQueue[i].RemainingDelay <= 0) {
			CommandQueue[i].Callback.ExecuteIfBound(CommandQueue[i].FlowId);
			CommandQueue.RemoveAt(i);
		}
	}
	return true;
}

