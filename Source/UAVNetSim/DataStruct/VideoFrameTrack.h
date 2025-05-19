// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VideoFrameTrack.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FVideoFrameTrack
{
    GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int64 UavCaptureTimestamp;

    UPROPERTY(BlueprintReadWrite)
	int64 GcsReceiveTimestamp;

	UPROPERTY(BlueprintReadWrite)
	int64 AIComputingLatency;

	UPROPERTY(BlueprintReadWrite)
	int64 GcsReceiveAIFeedbackTimestamp;

	UPROPERTY(BlueprintReadWrite)
	int64 DetectionBoxDrawnTimestamp;

	UPROPERTY(BlueprintReadWrite)
	float Confidence;

	UPROPERTY(BlueprintReadWrite)
	TArray<FVector4> BoundingBox;

	UPROPERTY(BlueprintReadWrite)
	FVector UavPosition;

	UPROPERTY(BlueprintReadWrite)
	float UavYaw;
};

class UAVNETSIM_API VideoFrameTrack
{
public:
	VideoFrameTrack();
	~VideoFrameTrack();
};
