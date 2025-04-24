// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/CanvasPanel.h>
#include "DetectionBox.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FDetectedBox
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FVector2D TopLeft;

    UPROPERTY(BlueprintReadWrite)
    FVector2D BottomRight;

    UPROPERTY(BlueprintReadWrite)
    float Confidence;
};

UCLASS()
class UAVNETSIM_API UDetectionBox : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UCanvasPanel* RootCanvas;

    UFUNCTION(BlueprintCallable, Category = "Overlay")
    void UpdateBoxes(const TArray<FDetectedBox>& Boxes);
};
