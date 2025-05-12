// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UAVNetSim/Zmq/ZmqSubscriber.h"
#include "UAVNetSim/DataStruct/Flow.h"
#include "NetworkStateInstance.generated.h"

class AGroundControlStation;

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
    
    UFUNCTION(BlueprintCallable, Category = "Flow Control")
    const TMap<int32, FFlowData>& GetFlowDataMap() const { return FlowDataMap; }

    FFlowData* GetFlowDataById(int32 FlowId);

    UFUNCTION(BlueprintCallable, Category = "Flow Control")
    void InsertOrAssign(int32 FlowId, FFlowData FlowData);

    UFUNCTION(BlueprintCallable, Category = "Flow Control")
    bool ContainsFlowId(int32 FlowId);

	UFUNCTION(BlueprintCallable, Category = "Flow Control")
	void ResetFlowDataMap();

private:
    TMap<int32, FFlowData> FlowDataMap;

	AGroundControlStation* GroundControlStation = nullptr;

    AZmqSubscriber* ZmqSubscriberInstance = nullptr;

    void BindToZmqSubscriber(const UWorld::FActorsInitializedParams& Params);

    UFUNCTION()
    void OnZmqSubscriberReady(bool IsReady);
};
