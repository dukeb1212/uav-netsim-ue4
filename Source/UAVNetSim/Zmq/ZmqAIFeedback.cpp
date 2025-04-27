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
}

void AZmqAIFeedback::HandleAIFeedback(const FString& Topic, const FString& Message)
{
    UE_LOG(LogTemp, Log, TEXT("[ZmqAIFeedback] Received on topic '%s': %s"), *Topic, *Message);

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Message);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        // Get classifier label
        FString ClassLabel = JsonObject->GetStringField("c");
        float ClassConfidence = JsonObject->GetNumberField("pc");

        UE_LOG(LogTemp, Log, TEXT("ResNet8 → Class: %s (Confidence: %.3f)"), *ClassLabel, ClassConfidence);

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

                    if ((*BoxObject)->TryGetArrayField("b", BArray) && BArray->Num() == 4)
                    {
                        float X1 = (float)(*BArray)[0]->AsNumber();
                        float Y1 = (float)(*BArray)[1]->AsNumber();
                        float X2 = (float)(*BArray)[2]->AsNumber();
                        float Y2 = (float)(*BArray)[3]->AsNumber();

                        UE_LOG(LogTemp, Log, TEXT("YOLO → Person at (%.0f, %.0f, %.0f, %.0f), Confidence: %.2f"),
                            X1, Y1, X2, Y2, Confidence);

                        FDetectedBox Box;
						Box.TopLeft = FVector2D(X1, Y1);
						Box.BottomRight = FVector2D(X2, Y2);
						Box.Confidence = Confidence;

                        DetectedBoxes.Add(Box);

                        if (OnFeedbackReceived.IsBound())
                        {
                            OnFeedbackReceived.Broadcast(ClassLabel, Confidence);
                        }
                    }
                }
            }

			if (DetectionBoxWidget)
			{
				DetectionBoxWidget->UpdateBoxes(DetectedBoxes);
			}
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse AI JSON message."));
    }
}
