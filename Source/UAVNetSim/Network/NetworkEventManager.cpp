// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkEventManager.h"
#include "NetworkStateInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UAVNetSim/GroundControlStation.h"

void UNetworkEventManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UNetworkEventManager::BindToZmqPublisher);
}

void UNetworkEventManager::BindToZmqPublisher(const UWorld::FActorsInitializedParams& Params)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is null, cannot bind to ZMQ Subscriber!"));
		return;
	}

	AGroundControlStation* GroundControlStation = Cast<AGroundControlStation>(UGameplayStatics::GetActorOfClass(World, AGroundControlStation::StaticClass()));
	if (GroundControlStation)
	{
		ZmqPublisher = Cast<AZmqPublisher>(GroundControlStation->Ns3PublisherComponent->GetChildActor());

		if (ZmqPublisher)
		{
			UE_LOG(LogTemp, Warning, TEXT("Successfully bound to ZMQ publisher!"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ZMQ Publisher not found!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Ground Control Station not found!"));
		return;
	}
}

int32 UNetworkEventManager::StartNetworkApplication(ENetworkAppType AppType, const FString& AppConfig)
{
	if (!ZmqPublisher) return -1;

	int32 LocalId = ++LastAssignedLocalId;

	// Temp flow data
	FNetworkFlow NewFlow;
	if (AppType == ENetworkAppType::VideoStream) NewFlow.FlowId = ++LocalId;
	else NewFlow.FlowId = LocalId;
	NewFlow.AppType = AppType;
	NewFlow.StartTime = FDateTime::Now();
	ActiveFlows.Add(LocalId, NewFlow);

	TSharedPtr<FJsonObject> EventData = MakeShareable(new FJsonObject());
	EventData->SetStringField("event_type", "start");
	EventData->SetStringField("app_type", StaticEnum<ENetworkAppType>()->GetNameStringByValue((int64)AppType));
	EventData->SetStringField("config", AppConfig);
	EventData->SetNumberField("local_id", LocalId);

	// Convert to JSON string
	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(EventData.ToSharedRef(), Writer);

	// Publish start event
	ZmqPublisher->PublishString("network_events", JsonString);
	UE_LOG(LogTemp, Warning, TEXT("Publish event: %s"), *JsonString);

	FFlowData FlowData = Flow::Create();
	auto* NetworkState = Cast<UNetworkStateInstance>(GetGameInstance());

	if (NetworkState) {
		NetworkState->InsertOrAssign(LocalId, FlowData);
	}

	return LocalId;
}

void UNetworkEventManager::StopNetworkApplication(int32 FlowId)
{
	if (!ZmqPublisher || !ActiveFlows.Contains(FlowId)) return;

	// Publish stop event
	TSharedPtr<FJsonObject> EventData = MakeShareable(new FJsonObject());
	EventData->SetStringField("event_type", "stop");
	EventData->SetNumberField("flow_id", ActiveFlows[FlowId].FlowId);
	EventData->SetStringField("app_type", StaticEnum<ENetworkAppType>()->GetNameStringByValue((int64)ActiveFlows[FlowId].AppType));

	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(EventData.ToSharedRef(), Writer);

	ZmqPublisher->PublishString("network_events", JsonString);
	UE_LOG(LogTemp, Warning, TEXT("Publish event: %s"), *JsonString);

	// Remove from active flows
	ActiveFlows.Remove(FlowId);
}

TArray<FNetworkFlow> UNetworkEventManager::GetActiveFlows() const
{
	TArray<FNetworkFlow> Flows;
	ActiveFlows.GenerateValueArray(Flows);
	return Flows;
}

int32 UNetworkEventManager::FindFlowIdByType(ENetworkAppType AppType) const
{
	for (const auto& Pair : ActiveFlows)
	{
		if (Pair.Value.AppType == AppType)
		{
			return Pair.Value.FlowId;
		}
	}
	return -1;
}

void UNetworkEventManager::OnFlowDataUpdated(int32 Ns3FlowId, ENetworkAppType AppType)
{
	for (auto& Pair : ActiveFlows)
	{
		if (Pair.Value.AppType == AppType && Pair.Value.FlowId < 1000) // Temporary local ID
		{
			Pair.Value.FlowId = Ns3FlowId;
			break;
		}
	}
}



