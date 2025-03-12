// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Flow.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FFlowData
{
    GENERATED_BODY()

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

class UAVNETSIM_API Flow
{
public:
	Flow();
	~Flow();
};


