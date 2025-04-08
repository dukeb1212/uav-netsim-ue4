// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Zmq/ZmqSubscriber.h"
#include "DataStruct/Flow.h"
#include "NetworkStateInstance.generated.h"


UCLASS()
class UAVNETSIM_API UNetworkStateInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    virtual void Init() override;

    // Store network data from JSON to Flow data type
    void ParseAndStoreNetworkData(const FString& JsonMessage);

    UFUNCTION()
    void HandleMessage(const FString& Topic, const FString& Message);

    const TMap<int32, FFlowData>& GetFlowDataMap() const { return FlowDataMap; }

    FFlowData* GetFlowDataById(int32 FlowId);
    void InsertOrAssign(int32 FlowId, FFlowData FlowData);

    bool ContainsFlowId(int32 FlowId);

private:
    TMap<int32, FFlowData> FlowDataMap;
    AZmqSubscriber* ZmqSubscriberInstance = nullptr;
    void BindToZmqSubscriber(const UWorld::FActorsInitializedParams& Params);
};
