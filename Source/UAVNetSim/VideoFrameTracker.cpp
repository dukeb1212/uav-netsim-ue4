// Fill out your copyright notice in the Description page of Project Settings.


#include "VideoFrameTracker.h"

AVideoFrameTracker::AVideoFrameTracker()
{
    FString DirectoryPath = FPaths::ProjectSavedDir() + "\\VideoTracking";
   
    if(!FPaths::DirectoryExists(DirectoryPath))
        IFileManager::Get().MakeDirectory(*DirectoryPath, true);

	CsvFilePath = DirectoryPath + "\\" + FDateTime::Now().ToString() + ".csv";
}

void AVideoFrameTracker::BeginPlay()
{
	Super::BeginPlay();
	// Initialize the VideoFrameTracks array
	VideoFrameTracks.Empty();
}

void AVideoFrameTracker::Tick(float DeltaTime)
{
}

void AVideoFrameTracker::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	WriteToCsv(CsvFilePath);

	// Clean up the VideoFrameTracks array
	VideoFrameTracks.Empty();
}

void AVideoFrameTracker::WriteToCsv(const FString& FilePath)
{
    if (VideoFrameTracks.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("VideoFrameTracks is empty, nothing to write."));
        return;
    }

    FString CsvContent;

    // Header line
    CsvContent += TEXT("FrameNumber,UavCaptureTime,GcsReceiveTime,AIComputingLatency,GcsReceiveAIFeedbackTime,DetectionBoxDrawnTime,UavPositionX,UavPositionY,UavPositionZ,UavYaw,BoundingBoxes,Confidence\n");

    TArray<int64> SortedKeys;
    VideoFrameTracks.GetKeys(SortedKeys);
    SortedKeys.Sort();

	int64 BaseTime = VideoFrameTracks[SortedKeys[0]].UavCaptureTimestamp;

    for (int64 FrameNum : SortedKeys)
    {
        const FVideoFrameTrack& Frame = VideoFrameTracks[FrameNum];
        // Ghi CSV

        auto FormatTimestamp = [BaseTime](int64 Timestamp) -> FString
            {
                if (Timestamp < BaseTime) return FString::Printf(TEXT("null"));
                // Convert nanosecond to second with 9 digits precision
                double Seconds = (Timestamp - BaseTime) / 1e9;
                return FString::Printf(TEXT("%.9f"), Seconds);
            };

        // Format Bounding Boxes as a string
        FString BBoxStr;
        for (int32 i = 0; i < Frame.BoundingBox.Num(); ++i)
        {
            const FVector4& Box = Frame.BoundingBox[i];
            FString BoxStr = FString::Printf(TEXT("[%.2f %.2f %.2f %.2f]"), Box.X, Box.Y, Box.Z, Box.W);
            BBoxStr += BoxStr;
            if (i < Frame.BoundingBox.Num() - 1)
            {
                BBoxStr += TEXT(";");
            }
        }

        double latency = Frame.AIComputingLatency / 1e9;

        FString Line = FString::Printf(
            TEXT("%lld,%s,%s,%.9f,%s,%s,%.2f,%.2f,%.2f,%.2f,\"%s\",%.3f\n"),
            FrameNum,
            *FormatTimestamp(Frame.UavCaptureTimestamp),
            *FormatTimestamp(Frame.GcsReceiveTimestamp),
            latency,
            *FormatTimestamp(Frame.GcsReceiveAIFeedbackTimestamp),
            *FormatTimestamp(Frame.DetectionBoxDrawnTimestamp),
            Frame.UavPosition.X,
            Frame.UavPosition.Y,
            Frame.UavPosition.Z,
            Frame.UavYaw,
            *BBoxStr,
            Frame.Confidence
        );

        CsvContent += Line;
    }

    // Write to file
    if (!FFileHelper::SaveStringToFile(CsvContent, *FilePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to write video frame track CSV to: %s"), *FilePath);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("CSV file successfully written to: %s"), *FilePath);
    }
}

FVideoFrameTrack& AVideoFrameTracker::AddOrGetFrame(int64 FrameNumber)
{
    return VideoFrameTracks.FindOrAdd(FrameNumber);
}
