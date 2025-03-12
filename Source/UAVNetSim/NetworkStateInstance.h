// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Zmq/ZmqSubscriber.h"
#include "DataStruct/Flow.h"
#include "NetworkStateInstance.generated.h"

/**
 * 
 */

//USTRUCT(BlueprintType)
//struct FFlowData
//{
//    GENERATED_BODY()
//
//    UPROPERTY(BlueprintReadOnly)
//    float MeanDelay;
//
//    UPROPERTY(BlueprintReadOnly)
//    float MeanJitter;
//
//    UPROPERTY(BlueprintReadOnly)
//    float PacketLossL3;
//
//    UPROPERTY(BlueprintReadOnly)
//    int32 RxBytes;
//
//    UPROPERTY(BlueprintReadOnly)
//    int32 RxPackets;
//
//    UPROPERTY(BlueprintReadOnly)
//    int32 TxBytes;
//
//    UPROPERTY(BlueprintReadOnly)
//    int32 TxPackets;
//};

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

    const TMap<int32, FFlowData>& GetFlowData() const { return FlowDataMap; }

private:
    TMap<int32, FFlowData> FlowDataMap;
    AZmqSubscriber* ZmqSubscriberInstance = nullptr;
    void BindToZmqSubscriber(const UWorld::FActorsInitializedParams& Params);
};
