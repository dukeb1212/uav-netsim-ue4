// Fill out your copyright notice in the Description page of Project Settings.


#include "DetectionBox.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include <Components/Border.h>
#include "Blueprint/WidgetTree.h"
#include <Components/Image.h>

void UDetectionBox::UpdateBoxes(const TArray<FDetectedBox>& Boxes)
{
    if (!RootCanvas) return;
    RootCanvas->ClearChildren();

    // Reference size assumed in your detection system
    const FVector2D ReferenceResolution(1920.0f, 1080.0f);

    // Get actual canvas size
    FVector2D ActualCanvasSize = RootCanvas->GetCachedGeometry().GetLocalSize();
    FVector2D ScaleFactor = ActualCanvasSize / ReferenceResolution;

    const float LineThickness = 3.f;
    const FLinearColor LineColor = FLinearColor::Green;

    for (const FDetectedBox& Box : Boxes)
    {
        // Scale box positions to current viewport
        const FVector2D TL = Box.TopLeft * ScaleFactor;
        const FVector2D BR = Box.BottomRight * ScaleFactor;
        const FVector2D Size = BR - TL;
        const FVector2D TR(BR.X, TL.Y);
        const FVector2D BL(TL.X, BR.Y);

        auto AddLine = [&](const FVector2D& Pos, const FVector2D& Size)
            {
                UImage* LineImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
                if (!LineImage) return;

                FSlateBrush Brush;
                Brush.TintColor = FSlateColor(LineColor);
                Brush.ImageSize = Size;
                LineImage->SetBrush(Brush);

                UCanvasPanelSlot* Slot = RootCanvas->AddChildToCanvas(LineImage);
                if (!Slot) return;

                Slot->SetPosition(Pos);
                Slot->SetSize(Size);
                Slot->SetAutoSize(false);
                Slot->SetAlignment(FVector2D(0.f, 0.f));
            };

        // Top line
        AddLine(TL, FVector2D(Size.X, LineThickness));
        // Bottom line
        AddLine(FVector2D(BL.X, BL.Y - LineThickness), FVector2D(Size.X, LineThickness));
        // Left line
        AddLine(TL, FVector2D(LineThickness, Size.Y));
        // Right line
        AddLine(FVector2D(TR.X - LineThickness, TR.Y), FVector2D(LineThickness, Size.Y));

        // Confidence label
        UTextBlock* ConfidenceText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
        if (!ConfidenceText) continue;

        FString ConfidenceStr = FString::Printf(TEXT("Person: %.2f"), Box.Confidence);
        ConfidenceText->SetText(FText::FromString(ConfidenceStr));

        // Set text color
        ConfidenceText->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));

        // Set justification
        ConfidenceText->SetJustification(ETextJustify::Left);

        // Set font size (e.g., 14)
        FSlateFontInfo FontInfo = ConfidenceText->Font;
        FontInfo.Size = 14;
        ConfidenceText->SetFont(FontInfo);

        UCanvasPanelSlot* TextSlot = RootCanvas->AddChildToCanvas(ConfidenceText);
        if (!TextSlot) continue;

        const FVector2D LabelOffset(0.f, -15.f);
        TextSlot->SetPosition(TL + LabelOffset);
        TextSlot->SetAutoSize(true);
        TextSlot->SetAlignment(FVector2D(0.f, 1.f));
    }
}




