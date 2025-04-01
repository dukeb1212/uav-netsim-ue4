// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkEffectManager.h"

void UNetworkEffectManager::Initialize(FSubsystemCollectionBase& Collection)
{
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UNetworkEffectManager::Tick));
	Super::Initialize(Collection);
}

void UNetworkEffectManager::Deinitialize()
{
	FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
	Super::Deinitialize();
}

void UNetworkEffectManager::SetNetworkParameters(float Delay)
{
	CurrentDelay = FMath::Max(Delay, 0.0f);
}

void UNetworkEffectManager::QueueTelemetryUpdate(const FTelemetryData& TelemetryData, FDelayedTelemetryCallback Callback)
{
	FDelayedTelemetry NewItem;
	NewItem.Data = TelemetryData;
	NewItem.RemainingDelay = CurrentDelay;
	NewItem.Callback = Callback;
	TelemetryQueue.Add(NewItem);
}

void UNetworkEffectManager::QueueExecuteUpdate(const bool& bCanExecute, FDelayExecuteCallback Callback)
{
	FDelayExecute NewItem;
	NewItem.bCanExecute = bCanExecute;
	NewItem.RemainingDelay = CurrentDelay;
	NewItem.Callback = Callback;
	ExecuteQueue.Add(NewItem);
}

void UNetworkEffectManager::QueueCommandExecute(const bool& bCanExecute, FDelayExecuteCallback Callback)
{
	FDelayExecute NewItem;
	NewItem.bCanExecute = bCanExecute;
	NewItem.RemainingDelay = CurrentDelay;
	NewItem.Callback = Callback;
	CommandQueue.Add(NewItem);
}

float UNetworkEffectManager::CalculateDelay(float MeanDelay, float MeanJitter)
{
	return FMath::Max(FMath::FRandRange((MeanDelay - MeanJitter) / 1000000, (MeanDelay + MeanJitter) / 1000000), 0.0f);
}

bool UNetworkEffectManager::Tick(float DeltaTime)
{
	for (int32 i = TelemetryQueue.Num() - 1; i >= 0; --i) {
		TelemetryQueue[i].RemainingDelay -= DeltaTime;
		if (TelemetryQueue[i].RemainingDelay <= 0) {
			TelemetryQueue[i].Callback.ExecuteIfBound(TelemetryQueue[i].Data);
			TelemetryQueue.RemoveAt(i);
		}
	}

	for (int32 i = ExecuteQueue.Num() - 1; i >= 0; --i) {
		ExecuteQueue[i].RemainingDelay -= DeltaTime;
		if (ExecuteQueue[i].RemainingDelay <= 0) {
			ExecuteQueue[i].bCanExecute = true;
			ExecuteQueue[i].Callback.ExecuteIfBound(ExecuteQueue[i].bCanExecute);
			ExecuteQueue.RemoveAt(i);
		}
	}

	for (int32 i = CommandQueue.Num() - 1; i >= 0; --i) {
		CommandQueue[i].RemainingDelay -= DeltaTime;
		if (CommandQueue[i].RemainingDelay <= 0) {
			CommandQueue[i].bCanExecute = true;
			CommandQueue[i].Callback.ExecuteIfBound(CommandQueue[i].bCanExecute);
			CommandQueue.RemoveAt(i);
		}
	}
	return true;
}

