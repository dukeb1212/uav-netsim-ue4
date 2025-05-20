// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraView.h"
#include <chrono>

void UCameraView::UpdateDisplayTexture(UTextureRenderTarget2D* RenderTarget, int64 FrameNumber)
{
	if (VideoFrameTracker)
	{
		FVideoFrameTrack& Frame = VideoFrameTracker->AddOrGetFrame(FrameNumber);
		auto TimeStamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
		Frame.GcsReceiveTimestamp = TimeStamp.count();
	}

    if (RenderTarget)
    {
        if (RenderTarget->Resource)
		{
            if (DynamicMaterial)
            {
                DynamicMaterial->SetTextureParameterValue(FName("VideoTexture"), RenderTarget);
				DisplayImage->SetBrushFromMaterial(DynamicMaterial);
            }
			else UE_LOG(LogTemp, Error, TEXT("DynamicMaterial is null!"));
           
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("RenderTarget resource is null!"));
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("RenderTarget is null!"));
		return;
    }

    if (ZmqPublisher)
    {
		if (IsValid(ZmqPublisher)) ZmqPublisher->PublishRawImage(RenderTarget, FrameNumber);
		else 
			UE_LOG(LogTemp, Error, TEXT("ZmqPublisher is not valid!!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ZmqPublisher is nullptr"));
    }

	RenderTarget->RemoveFromRoot();
}
