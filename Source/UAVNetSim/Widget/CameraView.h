// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/Image.h>
#include <Components/TextBlock.h>
#include <UAVNetSim/VideoFrameTracker.h>
#include <UAVNetSim/Zmq/ZmqPublisher.h>
#include "CameraView.generated.h"

/**
 * 
 */
UCLASS()
class UAVNETSIM_API UCameraView : public UUserWidget
{
	GENERATED_BODY()
	
public:

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* DisplayImage;

    UPROPERTY(BlueprintReadWrite, Transient)
	AVideoFrameTracker* VideoFrameTracker;

    UPROPERTY(BlueprintReadWrite, Transient)
	AZmqPublisher* ZmqPublisher;

    UPROPERTY(BlueprintReadWrite, Transient)
    UMaterialInstanceDynamic* DynamicMaterial;

    void UpdateDisplayTexture(UTextureRenderTarget2D* RenderTarget, int64 FrameNumber);
};
