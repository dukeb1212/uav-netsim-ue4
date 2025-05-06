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

void UNetworkEffectManager::QueueDelayedTexture(UTexture2D* SourceTexture, int32 FlowId, const FOnTextureProcessed& Callback)
{
	if (!SourceTexture || !SourceTexture->GetResource())
	{
		UE_LOG(LogNetworkEffect, Warning, TEXT("SourceTexture is invalid or has no resource."));
		return;
	}

	// Simulate network effects
	float Delay = 0.0f;
	bool bCanSkip = false;

	if (NetworkStateInstance && NetworkStateInstance->ContainsFlowId(FlowId))
	{
		const FFlowData& FlowData = *NetworkStateInstance->GetFlowDataById(FlowId);
		Delay = CalculateDelay(FlowData.MeanDelay, FlowData.MeanJitter);
		bCanSkip = (FMath::FRandRange(0.0f, 1.0f) < CalculatePacketLossRate(FlowData.PacketLossL3, FlowData.TxPackets));
	}

	// Create a new transient texture to store the delayed frame
	UTexture2D* NewFrameTexture = UTexture2D::CreateTransient(
		SourceTexture->GetSizeX(),
		SourceTexture->GetSizeY(),
		SourceTexture->GetPixelFormat()
	);
	if (!NewFrameTexture)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create transient texture for delayed frame."));
		return;
	}

	NewFrameTexture->SRGB = SourceTexture->SRGB;
	NewFrameTexture->MipGenSettings = TMGS_NoMipmaps;
	NewFrameTexture->UpdateResource();

	// Ensure texture is valid before copying
	NewFrameTexture->AddToRoot();

	ENQUEUE_RENDER_COMMAND(CopyTextureCmd)([SourceTexture, NewFrameTexture, Delay, bCanSkip, Callback, this](FRHICommandListImmediate& RHICmdList)
		{
			// Get source and destination RHI
			FTextureResource* SourceRes = SourceTexture->GetResource();
			FTextureResource* DestRes = NewFrameTexture->GetResource();
			if (!SourceRes || !DestRes)
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to get RHI for texture copy."));
				return;
			}

			// Get RHI references
			FTexture2DRHIRef SourceRHI = SourceRes->GetTexture2DRHI();
			FTexture2DRHIRef DestRHI = DestRes->GetTexture2DRHI();
			if (!SourceRHI.IsValid() || !DestRHI.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("Invalid RHI for texture copy."));
				return;
			}

			// Get dimensions
			const int32 Width = SourceTexture->GetSizeX();
			const int32 Height = SourceTexture->GetSizeY();

			// Step 1: Read source texture to CPU buffer
			TArray<FColor> PixelBuffer;
			FIntRect Rect(0, 0, Width, Height);
			FUpdateTextureRegion2D Region(0, 0, 0, 0, Width, Height);
			FReadSurfaceDataFlags Flags(RCM_UNorm, CubeFace_MAX);
			RHICmdList.ReadSurfaceData(SourceRHI, Rect, PixelBuffer, Flags);

			// Step 2: Upload buffer to new texture
			if (PixelBuffer.Num() == Width * Height)
			{
				uint32 SrcPitch = Width * sizeof(FColor);
				RHIUpdateTexture2D(
					DestRHI,
					0,
					Region,
					SrcPitch,
					reinterpret_cast<uint8*>(PixelBuffer.GetData())
				);
			}

			// Schedule frame in game thread
			AsyncTask(ENamedThreads::GameThread, [this, NewFrameTexture, Delay, bCanSkip, Callback]()
				{
					FDelayedFrame NewFrame;
					NewFrame.Texture = NewFrameTexture;
					NewFrame.RemainingDelay = Delay;
					NewFrame.bCanSkip = bCanSkip;
					NewFrame.Callback = Callback;

					FrameQueue.Add(NewFrame);
				});
		});
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
			UE_LOG(LogNetworkEffect, Log, TEXT("Processing Telemetry | Delayed %.2fms"), DeltaTime);
			TelemetryQueue[i].Callback.ExecuteIfBound(TelemetryQueue[i].Data);
			TelemetryQueue.RemoveAt(i);
		}
	}

	// Process Frame Queue
	for (int32 i = FrameQueue.Num() - 1; i >= 0; --i)
	{
		FDelayedFrame& Frame = FrameQueue[i];

		UE_LOG(LogNetworkEffect, Verbose, TEXT("Processing Frame | RemainingDelay: %.2f"), Frame.RemainingDelay);

		Frame.RemainingDelay -= DeltaTime;

		if (Frame.RemainingDelay <= 0)
		{
			if (Frame.bCanSkip)
			{
				UE_LOG(LogNetworkEffect, Log, TEXT("Skipping Frame | Packet Loss"));
			}
			else if (Frame.Callback.IsBound())
			{
				UE_LOG(LogNetworkEffect, Log, TEXT("Displaying Frame | Texture: %p"), Frame.Texture);
				Frame.Callback.Broadcast(Frame.Texture);
			}

			FrameQueue.RemoveAt(i);
		}
	}

	return true;
}

