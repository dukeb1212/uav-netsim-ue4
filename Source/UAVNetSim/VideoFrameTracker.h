// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DataStruct/VideoFrameTrack.h"
#include "VideoFrameTracker.generated.h"

/**
 * 
 */
UCLASS()
class UAVNETSIM_API AVideoFrameTracker : public AGameStateBase
{
	GENERATED_BODY()


public:
	// Sets default values for this actor's properties
	AVideoFrameTracker();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VideoFrameTracker")
	TMap<int64, FVideoFrameTrack> VideoFrameTracks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VideoFrameTracker")
	FString CsvFilePath;

	UFUNCTION(BlueprintCallable, Category = "VideoFrameTracker")
	void WriteToCsv(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "VideoFrameTracker")
	FVideoFrameTrack& AddOrGetFrame(int64 FrameNumber);


};
