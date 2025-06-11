// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "../Zmq/ZmqPublisher.h"
#include "../DataStruct/Flow.h"
#include "NetworkEventManager.generated.h"

/**
 * 
 */
class AGroundControlStation;

UCLASS()
class UAVNETSIM_API UNetworkEventManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ZMQ")
    AZmqPublisher* ZmqPublisher;

    // Start a network application in NS-3
    UFUNCTION(BlueprintCallable, Category = "Network|Events")
    int32 StartNetworkApplication(ENetworkAppType AppType, const FString& AppConfig = "");

    // Stop a network application in NS-3
    UFUNCTION(BlueprintCallable, Category = "Network|Events")
    void StopNetworkApplication(int32 FlowId);

    // Get current active flows
    UFUNCTION(BlueprintCallable, Category = "Network|Events")
    TArray<FNetworkFlow> GetActiveFlows() const;

    // Find flow by type
    UFUNCTION(BlueprintCallable, Category = "Network|Events")
    int32 FindFlowIdByType(ENetworkAppType AppType) const;

    UFUNCTION(BlueprintCallable, Category = "Network|Events")
    void ResetLocalId();

// Variables
private:
    TMap<int32, FNetworkFlow> ActiveFlows;
    int32 LastAssignedLocalId = 1;

// Methods
private:
	AGroundControlStation* GroundControlStation;

    void OnFlowDataUpdated(int32 Ns3FlowId, ENetworkAppType AppType);

    void BindToZmqPublisher(const UWorld::FActorsInitializedParams& Params);

    UFUNCTION()
    void OnZmqPublisherReady(bool IsReady);
};
