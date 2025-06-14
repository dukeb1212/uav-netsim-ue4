// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkStateInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Engine/Engine.h"
#include "UAVNetSim/GroundControlStation.h"


void UNetworkStateInstance::Init()
{
    Super::Init();

    // Bind the network to a ZMQ subscriber for data update
    FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UNetworkStateInstance::BindToZmqSubscriber);
    InsertOrAssign(1, Flow::Create());
}

void UNetworkStateInstance::Shutdown()
{
	// Unbind the network from the ZMQ subscriber
	FWorldDelegates::OnWorldInitializedActors.RemoveAll(this);
	if (ZmqSubscriberInstance)
	{
		ZmqSubscriberInstance->OnMessageReceived.RemoveDynamic(this, &UNetworkStateInstance::HandleMessage);
		ZmqSubscriberInstance = nullptr;
	}

    Super::Shutdown();
}

void UNetworkStateInstance::BindToZmqSubscriber(const UWorld::FActorsInitializedParams& Params)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("World is null, cannot bind to ZMQ Subscriber!"));
        return;
    }

    GroundControlStation = Cast<AGroundControlStation>(UGameplayStatics::GetActorOfClass(World, AGroundControlStation::StaticClass()));

    if (GroundControlStation)
    {
        GroundControlStation->OnZmqComponentReady.AddDynamic(this, &UNetworkStateInstance::OnZmqSubscriberReady);
    }
}

void UNetworkStateInstance::OnZmqSubscriberReady(bool IsReady)
{
	if (IsReady)
	{
		ZmqSubscriberInstance = Cast<AZmqSubscriber>(GroundControlStation->Ns3SubscriberComponent->GetChildActor());
		if (ZmqSubscriberInstance)
		{
			ZmqSubscriberInstance->OnMessageReceived.AddDynamic(this, &UNetworkStateInstance::HandleMessage);
			UE_LOG(LogTemp, Log, TEXT("ZMQ Subscriber is ready."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ZMQ Subscriber not found!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ZMQ Subscriber not yet available. Retrying..."));
	}
}

void UNetworkStateInstance::HandleMessage(const FString& Topic, const FString& Message)
{
    if (Topic == TEXT("network"))
    {
        //UE_LOG(LogTemp, Log, TEXT("Processing network data..."));
        ParseAndStoreNetworkData(Message);
    }
}

FFlowData* UNetworkStateInstance::GetFlowDataById(int32 FlowId)
{
    return FlowDataMap.Find(FlowId);
}

void UNetworkStateInstance::InsertOrAssign(int32 FlowId, FFlowData FlowData)
{
    FlowDataMap.FindOrAdd(FlowId) = FlowData;
}

bool UNetworkStateInstance::ContainsFlowId(int32 FlowId)
{
	return FlowDataMap.Contains(FlowId);
}

void UNetworkStateInstance::ResetFlowDataMap()
{
	FlowDataMap.Empty();
    InsertOrAssign(1, Flow::Create());
}

void UNetworkStateInstance::ParseAndStoreNetworkData(const FString& JsonMessage)
{
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonMessage);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        //// Ensure "conditions" exists
        //const TSharedPtr<FJsonObject>* Conditions;
        //if (!JsonObject->TryGetObjectField(TEXT("conditions"), Conditions))
        //{
        //    UE_LOG(LogTemp, Error, TEXT("JSON parsing error: 'conditions' field missing!"));
        //    return;
        //}

        //TSharedPtr<FJsonObject> ValidConditions = *Conditions;
        //// Ensure "flows" exists
        //const TSharedPtr<FJsonObject>* Flows;
        //if (!ValidConditions->TryGetObjectField(TEXT("flows"), Flows))
        //{
        //    UE_LOG(LogTemp, Error, TEXT("JSON parsing error: 'flows' field missing!"));
        //    return;
        //}

        //TSharedPtr<FJsonObject> ValidFlows = *Flows;

        //// Iterate through flows
        //for (const auto& FlowPair : ValidFlows->Values)
        //{
        //    int32 FlowID = FCString::Atoi(*FlowPair.Key);
        //    TSharedPtr<FJsonObject> FlowDataJson = FlowPair.Value->AsObject();

        //    if (FlowDataJson.IsValid())
        //    {
        //        FFlowData FlowData;
        //        FlowData.SourceIPAddress = FlowDataJson->GetStringField("src");
        //        FlowData.DestinationIPAddress = FlowDataJson->GetStringField("dst");
        //        FlowData.MeanDelay = FlowDataJson->GetNumberField("meanDelay");
        //        FlowData.MeanJitter = FlowDataJson->GetNumberField("meanJitter");
        //        FlowData.PacketLossL3 = FlowDataJson->GetNumberField("packetLossL3");
        //        FlowData.RxBytes = FlowDataJson->GetIntegerField("rxBytes");
        //        FlowData.RxPackets = FlowDataJson->GetIntegerField("rxPackets");
        //        FlowData.TxBytes = FlowDataJson->GetIntegerField("txBytes");
        //        FlowData.TxPackets = FlowDataJson->GetIntegerField("txPackets");

        //        // Store or update flow data
        //        FlowDataMap.FindOrAdd(FlowID) = FlowData;
        //    }
        //}

        //// Extract and print time
        //double Time;
        //if (!ValidConditions->TryGetNumberField(TEXT("time"), Time))
        //{
        //    UE_LOG(LogTemp, Error, TEXT("Failed to retrieve 'time' field from JSON"));
        //}

        FFlowData FlowData;
        FlowData.MeanDelay = JsonObject->GetNumberField("meanDelay");
        FlowData.MeanJitter = JsonObject->GetNumberField("meanJitter");
        FlowData.PacketLossL3 = JsonObject->GetNumberField("packetLoss");
        FlowData.RxBytes = 0;
        FlowData.RxPackets = 0;
        FlowData.TxBytes = 0;
        FlowData.TxPackets = 0;

        // Store or update flow data
        FlowDataMap.FindOrAdd(1) = FlowData;


        // Print flow data
        for (const auto& Pair : FlowDataMap)
        {
            const FFlowData& Data = Pair.Value;
            FString OutputMessage = FString::Printf(TEXT("Flow ID: %d\nSourceIP: %s\nDestIP: %s\nMeanDelay: %.2f ms\nMeanJitter: %.2f ms\nPacketLoss: %.2f%%\nRxBytes: %d\nRxPackets: %d\nTxBytes: %d\nTxPackets: %d"),
                Pair.Key, *Data.SourceIPAddress, *Data.DestinationIPAddress, Data.MeanDelay, Data.MeanJitter, Data.PacketLossL3, Data.RxBytes, Data.RxPackets, Data.TxBytes, Data.TxPackets);

            UE_LOG(LogTemp, Log, TEXT("%s"), *OutputMessage);
            // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, OutputMessage);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse network JSON data!"));
    }
}
