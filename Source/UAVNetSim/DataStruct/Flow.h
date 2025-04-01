// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Flow.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ENetworkAppType : uint8
{
    Telemetry,
    VideoStream,
    ControlCommands,
    SensorData
};

UENUM(BlueprintType)
enum class EDataPriority : uint8
{
    Highest,    // Emergency stops, critical commands
    High,       // Takeoff, landing, navigation
    Medium,     // Telemetry updates
    Normal,     // Video streaming
    Low         // Non-critical data
};

USTRUCT(BlueprintType)
struct FFlowData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString SourceIPAddress;

    UPROPERTY(BlueprintReadOnly)
    FString DestinationIPAddress;

    UPROPERTY(BlueprintReadOnly)
    float MeanDelay;

    UPROPERTY(BlueprintReadOnly)
    float MeanJitter;

    UPROPERTY(BlueprintReadOnly)
    float PacketLossL3;

    UPROPERTY(BlueprintReadOnly)
    int32 RxBytes;

    UPROPERTY(BlueprintReadOnly)
    int32 RxPackets;

    UPROPERTY(BlueprintReadOnly)
    int32 TxBytes;

    UPROPERTY(BlueprintReadOnly)
    int32 TxPackets;
};

USTRUCT(BlueprintType)
struct FNetworkFlow
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere)
    int32 FlowId = -1;

    UPROPERTY(VisibleAnywhere)
    ENetworkAppType AppType;

    UPROPERTY(VisibleAnywhere)
    FDateTime StartTime;
};

class UAVNETSIM_API Flow
{
public:
	Flow();
	~Flow();

    static FFlowData Create();
};


