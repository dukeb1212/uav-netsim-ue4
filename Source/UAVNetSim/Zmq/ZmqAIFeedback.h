// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZmqSubscriber.h"
#include "UAVNetSim/DetectionBox.h"
#include "UAVNetSim/VideoFrameTracker.h"
#include "ZmqAIFeedback.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAIFeedback, const FString&, Class, const float&, Confidence);
/**
 * 
 */
UCLASS()
class UAVNETSIM_API AZmqAIFeedback : public AZmqSubscriber
{
	GENERATED_BODY()
	
public:
    AZmqAIFeedback();

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Detection")
    UDetectionBox* DetectionBoxWidget;

    UPROPERTY(BlueprintAssignable, Category = "ZMQ")
    FOnAIFeedback OnFeedbackReceived;
protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void HandleAIFeedback(const FString& Topic, const FString& Message);

private:
	AVideoFrameTracker* VideoFrameTrackerInstance = nullptr;

	FTimerHandle TimerHandle;

	void FindVideoFrameTracker();
};
