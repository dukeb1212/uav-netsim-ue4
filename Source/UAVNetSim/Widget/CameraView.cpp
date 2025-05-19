// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraView.h"
#include <chrono>

void UCameraView::UpdateDisplayTexture_Implementation(UTextureRenderTarget2D* RenderTarget, int64 FrameNumber)
{
	if (VideoFrameTracker)
	{
		FVideoFrameTrack& Frame = VideoFrameTracker->AddOrGetFrame(FrameNumber);
		auto TimeStamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
		Frame.GcsReceiveTimestamp = TimeStamp.count();
	}

	if (ZmqPublisher)
	{
		ZmqPublisher->PublishRawImage(RenderTarget, FrameNumber);
	}
}
