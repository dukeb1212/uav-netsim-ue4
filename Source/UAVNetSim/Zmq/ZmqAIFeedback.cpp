// Fill out your copyright notice in the Description page of Project Settings.


#include "ZmqAIFeedback.h"


AZmqAIFeedback::AZmqAIFeedback()
{
	DefaultTopic = "ai";
}

void AZmqAIFeedback::BeginPlay()
{
	Super::BeginPlay();

	OnMessageReceived.AddDynamic(this, &AZmqAIFeedback::HandleAIFeedback);

	/*DetectionBoxWidget = CreateWidget<UDetectionBox>(GetWorld(), UDetectionBox::StaticClass());
    
	if (DetectionBoxWidget)
	{
		DetectionBoxWidget->AddToViewport();
	}*/
	// Find the VideoFrameTracker instance in the world
	VideoFrameTrackerInstance = GetWorld()->GetGameState<AVideoFrameTracker>();
    if (!VideoFrameTrackerInstance)
    {
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AZmqAIFeedback::FindVideoFrameTracker, 0.5f, false);
    }
}

void AZmqAIFeedback::FindVideoFrameTracker()
{
	VideoFrameTrackerInstance = GetWorld()->GetGameState<AVideoFrameTracker>();
	if (VideoFrameTrackerInstance)
	{
		TimerHandle.Invalidate();
		UE_LOG(LogTemp, Log, TEXT("Found VideoFrameTracker instance."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("VideoFrameTracker instance not found. Retrying..."));
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AZmqAIFeedback::FindVideoFrameTracker, 0.5f, false);
	}
}

void AZmqAIFeedback::HandleAIFeedback(const FString& Topic, const FString& Message)
{
    UE_LOG(LogTemp, Log, TEXT("[ZmqAIFeedback] Received on topic '%s': %s"), *Topic, *Message);

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Message);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        int64 FrameNumber = JsonObject->GetIntegerField("frame");
        int64 AiLatencyNs = JsonObject->GetIntegerField("ai_latency_ns");

		FVideoFrameTrack& FrameTrack = VideoFrameTrackerInstance->AddOrGetFrame(FrameNumber);
        FrameTrack.AIComputingLatency = AiLatencyNs;

        auto TimeStamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
		FrameTrack.GcsReceiveAIFeedbackTimestamp = TimeStamp.count();

        FrameTrack.BoundingBox.Empty();
        // Get classifier label
        //FString ClassLabel = JsonObject->GetStringField("c");
        //float ClassConfidence = JsonObject->GetNumberField("pc");

        //UE_LOG(LogTemp, Log, TEXT("ResNet8 → Class: %s (Confidence: %.3f)"), *ClassLabel, ClassConfidence);

        /*if (OnFeedbackReceived.IsBound())
        {
            OnFeedbackReceived.Broadcast(ClassLabel, ClassConfidence);
        }*/

        const TArray<TSharedPtr<FJsonValue>>* BoxesArray;
		TArray<FDetectedBox> DetectedBoxes;
        if (JsonObject->TryGetArrayField("boxes", BoxesArray))
        {
            for (const auto& BoxEntry : *BoxesArray)
            {
                const TSharedPtr<FJsonObject>* BoxObject;
                if (BoxEntry->TryGetObject(BoxObject))
                {
                    const TArray<TSharedPtr<FJsonValue>>* BArray;
                    float Confidence = (*BoxObject)->GetNumberField("c");

					FrameTrack.Confidence = Confidence;

                    if ((*BoxObject)->TryGetArrayField("b", BArray) && BArray->Num() == 4)
                    {
                        float X1 = (float)(*BArray)[0]->AsNumber();
                        float Y1 = (float)(*BArray)[1]->AsNumber();
                        float X2 = (float)(*BArray)[2]->AsNumber();
                        float Y2 = (float)(*BArray)[3]->AsNumber();

                        UE_LOG(LogTemp, Log, TEXT("YOLO → Person at (%.0f, %.0f, %.0f, %.0f), Confidence: %.2f"),
                            X1, Y1, X2, Y2, Confidence);

                        FVector4 BoundingBoxVec(X1, Y1, X2, Y2);
						FrameTrack.BoundingBox.Add(BoundingBoxVec);

                        FDetectedBox Box;
						Box.TopLeft = FVector2D(X1, Y1);
						Box.BottomRight = FVector2D(X2, Y2);
						Box.Confidence = Confidence;

                        DetectedBoxes.Add(Box);

                        /*if (OnFeedbackReceived.IsBound())
                        {
                            OnFeedbackReceived.Broadcast(ClassLabel, Confidence);
                        }*/
                    }
                }
            }

			if (DetectionBoxWidget)
			{
				DetectionBoxWidget->UpdateBoxes(DetectedBoxes, FrameNumber);
			}
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse AI JSON message."));
    }
}
