// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Telemetry.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FTelemetryData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    FString Name;

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    FVector Position;

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    FVector GPS;

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    FVector Velocity;

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    int64 Timestamp;

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    float BatteryLevel;

    UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
    bool IsConnected;
};

class UAVNETSIM_API Telemetry
{
public:
	Telemetry();
	~Telemetry();
};
