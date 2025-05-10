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
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* DisplayImage;

    UPROPERTY(BlueprintReadWrite)
    UMaterialInstanceDynamic* DynamicMaterial;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Texture")
    void UpdateDisplayTexture(UTextureRenderTarget2D* RenderTarget);

    void UpdateDisplayTexture_Implementation(UTextureRenderTarget2D* RenderTarget);
};
