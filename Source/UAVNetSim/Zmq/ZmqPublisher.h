// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <zmq.hpp>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Engine/TextureRenderTarget2D.h>
#include "ZmqPublisher.generated.h"


UCLASS()
class UAVNETSIM_API AZmqPublisher : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AZmqPublisher();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    // Configuration properties

    // Publish address
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZMQ Settings")
    FString ConnectionAddress = "tcp://*:5555";

    // Publish every x second
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZMQ Settings")
    float PublishInterval = 1.0f;

    // ZMQ topics (if many)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZMQ Settings")
    TArray<FString> Topics = { TEXT("ns3") };

    // Adding topic
    UFUNCTION(BlueprintCallable, Category = "ZMQ Settings")
    void AddTopic();

    // Array for every class need to be published
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZMQ Settings")
    TArray<TSubclassOf<AActor>> ActorClasses;  // Now accepts multiple classes

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZMQ Settings")
    FString DefaultTopic = "ns3";

    // If more details than only position
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZMQ Settings")
    TMap<FString, FString> ExtraFields;

    // Location to sync the coordinate between UE and ns-3
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZMQ Settings")
    FVector Ns3Root;

    // Dynamic message publishing
    UFUNCTION(BlueprintCallable, Category = "ZMQ")
    void PublishString(const FString& Topic, const FString& Message);

    // New publish interval
    UFUNCTION(BlueprintCallable, Category = "ZMQ")
    void UpdatePublishInterval(float NewInterval);

    // Publish all actors data
    UFUNCTION(BlueprintCallable, Category = "ZMQ", meta = (AdvancedDisplay = "OverrideActorClass, OverrideTopic, OverrideExtraFields"))
    void PublishAllActors(
        const TArray<TSubclassOf<AActor>>& OverrideActorClasses,
        FString OverrideTopic,
        const TMap<FString, FString>& OverrideExtraFields);

    UFUNCTION(BlueprintCallable, Category = "ZMQ")
    void PublishRawImage(UTextureRenderTarget2D* RenderTarget);

    void CompressWithTurboJPEG(const TArray<uint8>& RGBData, int32 Width, int32 Height, TArray<uint8>& OutJPEG);

private:
    zmq::context_t Context;
    zmq::socket_t Socket;
    FTimerHandle TimerHandle;
    FCriticalSection ZmqMutex;

    TArray<FColor> PixelBuffer_Back;
    TArray<uint8> RGBBuffer_Back;
    std::atomic<bool> bBufferInUse = false;

    // Auto publish to check health
    void SendHeartbeat();

    // Start ZMQ
    void InitializeZmq();
    void SafeShutdown();
};
