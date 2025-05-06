// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/Image.h>
#include <Components/TextBlock.h>
#include "CameraView.generated.h"

/**
 * 
 */
UCLASS()
class UAVNETSIM_API UCameraView : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(meta = (BindWidget))
    UImage* DisplayImage;

    UFUNCTION(BlueprintCallable, Category = "Texture")
    void UpdateDisplayTexture(UTexture2D* Texture);
};
