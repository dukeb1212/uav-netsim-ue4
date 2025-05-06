// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraView.h"

void UCameraView::UpdateDisplayTexture(UTexture2D* Texture)
{
    UE_LOG(LogTemp, Log, TEXT("UCameraView::UpdateDisplayTexture called | Texture: %p"), Texture);

    if (!DisplayImage)
    {
        UE_LOG(LogTemp, Error, TEXT("DisplayImage is null! Check Blueprint binding."));
        return;
    }

    if (!Texture)
    {
        UE_LOG(LogTemp, Error, TEXT("Texture is null!"));
        return;
    }

    DisplayImage->SetBrushFromTexture(Texture);
}
