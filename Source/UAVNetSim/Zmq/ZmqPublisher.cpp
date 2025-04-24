// Fill out your copyright notice in the Description page of Project Settings.


#include "ZmqPublisher.h"
#include "Async/Async.h"
#include "Kismet/GameplayStatics.h"
#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include <ImageUtils.h>
#include "turbojpeg.h"

// Sets default values
AZmqPublisher::AZmqPublisher()
	: Context(1), Socket(Context, ZMQ_PUB)
{
	PrimaryActorTick.bCanEverTick = true;
	Ns3Root = FVector(0, 0, 0);
}

// Called when the game starts or when spawned
void AZmqPublisher::BeginPlay()
{
	Super::BeginPlay();
	InitializeZmq();
}

// Called every frame
void AZmqPublisher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AZmqPublisher::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SafeShutdown();
	Super::EndPlay(EndPlayReason);
}

void AZmqPublisher::InitializeZmq()
{
	try {
		// Lock to avoid changing socket
		FScopeLock Lock(&ZmqMutex);

		// Start binding socket
		Socket.bind(TCHAR_TO_UTF8(*ConnectionAddress));

		// Publish health check every x second
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AZmqPublisher::SendHeartbeat, PublishInterval, true);
	}
	catch (const zmq::error_t& e) {
		UE_LOG(LogTemp, Error, TEXT("ZMQ bind failed: %s"), UTF8_TO_TCHAR(e.what()));
	}
}

void AZmqPublisher::SafeShutdown()
{
	FScopeLock Lock(&ZmqMutex);
	try {
		Socket.close();
		Context.close();
	}
	catch (...) {
		UE_LOG(LogTemp, Warning, TEXT("Error during ZMQ shutdown"));
	}
	GetWorldTimerManager().ClearTimer(TimerHandle);
}

void AZmqPublisher::PublishString(const FString& Topic, const FString& Message)
{
    //// Create weak link to avoid accessing deleted instance
    //TWeakObjectPtr<AZmqPublisher> WeakThis(this);

    //// Run on background thread to avoid blocking main game thread
    //AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis, Topic, Message]() {
    //    if (!WeakThis.IsValid()) return;

    //    // Get the current instance
    //    AZmqPublisher* Publisher = WeakThis.Get();
    //    
    //    });

    FScopeLock Lock(&ZmqMutex);
    try {
        std::string fullMessage = TCHAR_TO_UTF8(*(Topic + " " + Message));

        // Send with flag dont wait -> if socket not ready, drop the msg
        Socket.send(zmq::buffer(fullMessage), zmq::send_flags::dontwait);
        //UE_LOG(LogTemp, Warning, TEXT("Published string!!!"));
    }
    catch (const zmq::error_t& e) {
        UE_LOG(LogTemp, Error, TEXT("ZMQ send error: %s"), UTF8_TO_TCHAR(e.what()));
    }
}

void AZmqPublisher::AddTopic()
{
    int32 NewIndex = Topics.Num();
    FString NewTopic = FString::Printf(TEXT("uav%d"), NewIndex);
    Topics.Add(NewTopic);
}

void AZmqPublisher::UpdatePublishInterval(float NewInterval)
{
    GetWorldTimerManager().SetTimer(TimerHandle, this, &AZmqPublisher::SendHeartbeat, NewInterval, true);
}

void AZmqPublisher::PublishAllActors(const TArray<TSubclassOf<AActor>>& OverrideActorClasses = TArray<TSubclassOf<AActor>>(), FString OverrideTopic = "", const TMap<FString, FString>& OverrideExtraFields = TMap<FString, FString>())
{
    // Check for blueprint override
    const TArray<TSubclassOf<AActor>>& FinalActorClasses = OverrideActorClasses.Num() > 0 ? OverrideActorClasses : ActorClasses;
    FString FinalTopic = !OverrideTopic.IsEmpty() ? OverrideTopic : DefaultTopic;
    TMap<FString, FString> FinalExtraFields = OverrideExtraFields.Num() > 0 ? OverrideExtraFields : ExtraFields;

    // Avoid duplicated actor classes
    TSet<AActor*> UniqueActors;
    for (const TSubclassOf<AActor>& ActorClass : FinalActorClasses)
    {
        TArray<AActor*> ClassActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ActorClass, ClassActors);
        UniqueActors.Append(ClassActors);
    }

    // Convert back to array
    TArray<AActor*> AllActors = UniqueActors.Array();

    // JSON object to publish
    TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject());
    // Time stamping
    RootObject->SetStringField("timestamp", FDateTime::Now().ToIso8601());

    // Add data for all actors
    TArray<TSharedPtr<FJsonValue>> ActorArray;
    for (AActor* Actor : AllActors)
    {
        if (!Actor) continue;
        TSharedPtr<FJsonObject> ActorObject = MakeShareable(new FJsonObject());

        // Convert to ns-3 coordinate
        FVector Location = Actor->GetActorLocation();
        float Ns3X = (Location.X - Ns3Root.X) / 100;
        float Ns3Y = (Ns3Root.Y - Location.Y) / 100;
        float Ns3Z = (Location.Z - Ns3Root.Z) / 100;

        ActorObject->SetStringField("id", Actor->GetName());
        ActorObject->SetNumberField("x", Ns3X);
        ActorObject->SetNumberField("y", Ns3Y);
        ActorObject->SetNumberField("z", Ns3Z);

        // Add any extra fields
        for (const TPair<FString, FString>& Pair : FinalExtraFields)
        {
            ActorObject->SetStringField(Pair.Key, Pair.Value);
        }

        ActorArray.Add(MakeShareable(new FJsonValueObject(ActorObject)));
    }
    RootObject->SetArrayField("actors", ActorArray);

    // Convert JSON to string
    FString JsonString;
    TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> Writer =
        TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&JsonString);

    // Serialize and handle failure
    if (FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer))
    {
        Writer->Close(); // Explicit close
        PublishString(FinalTopic, JsonString);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to serialize JSON! Check data types."));
    }
}

void AZmqPublisher::PublishRawImage(UTextureRenderTarget2D* RenderTarget)
{
    if (!RenderTarget || !RenderTarget->GameThread_GetRenderTargetResource())
        return;

    FTextureRenderTargetResource* RTResource = RenderTarget->GameThread_GetRenderTargetResource();
    int32 SizeX = RenderTarget->GetSurfaceWidth(); // int32, not float!
    int32 SizeY = RenderTarget->GetSurfaceHeight();

    // Double-buffered setup
    if (bBufferInUse) return;
    bBufferInUse = true;

    PixelBuffer_Back.SetNumUninitialized(SizeX * SizeY);
    RGBBuffer_Back.SetNumUninitialized(SizeX * SizeY * 3);

    ENQUEUE_RENDER_COMMAND(CaptureCommand)(
        [RTResource, SizeX, SizeY, this](FRHICommandListImmediate& RHICmdList)
        {
            FReadSurfaceDataFlags ReadFlags(RCM_UNorm, CubeFace_MAX);
            RHICmdList.ReadSurfaceData(
                RTResource->GetRenderTargetTexture(),
                FIntRect(0, 0, SizeX, SizeY),
                PixelBuffer_Back,
                ReadFlags
            );

            Async(EAsyncExecution::ThreadPool, [this, SizeX, SizeY]()
                {
                    // Validate buffer sizes
                    if (PixelBuffer_Back.Num() != SizeX * SizeY || RGBBuffer_Back.Num() != SizeX * SizeY * 3)
                    {
                        UE_LOG(LogTemp, Error, TEXT("Buffer size mismatch!"));
                        bBufferInUse = false;
                        return;
                    }

                    // Safe ParallelFor with bounds checking
                    ParallelFor(SizeX * SizeY, [&](int32 i)
                        {
                            if (i >= 0 && i < PixelBuffer_Back.Num() && (i * 3 + 2) < RGBBuffer_Back.Num())
                            {
                                RGBBuffer_Back[i * 3 + 0] = PixelBuffer_Back[i].R;
                                RGBBuffer_Back[i * 3 + 1] = PixelBuffer_Back[i].G;
                                RGBBuffer_Back[i * 3 + 2] = PixelBuffer_Back[i].B;
                            }
                        });

                    // Compress and send
                    TArray<uint8> CompressedJPEG;
                    CompressWithTurboJPEG(RGBBuffer_Back, SizeX, SizeY, CompressedJPEG);

                    FScopeLock Lock(&ZmqMutex);
                    try {
                        zmq::message_t Msg(CompressedJPEG.Num());
                        FMemory::Memcpy(Msg.data(), CompressedJPEG.GetData(), CompressedJPEG.Num());
                        Socket.send(Msg, zmq::send_flags::dontwait);
                    }
                    catch (const zmq::error_t& e) {
                        UE_LOG(LogTemp, Error, TEXT("ZMQ send error: %s"), UTF8_TO_TCHAR(e.what()));
                    }

                    bBufferInUse = false; // Release buffer
                });
        });
}

void AZmqPublisher::CompressWithTurboJPEG(const TArray<uint8>& RGBData, int32 Width, int32 Height, TArray<uint8>& OutJPEG)
{
    tjhandle tjInstance = tjInitCompress();
    if (!tjInstance) {
        UE_LOG(LogTemp, Error, TEXT("TurboJPEG init failed: %s"), tjGetErrorStr());
        return;
    }

    int32 PixelFormat = TJPF_RGB; // Input format (RGB)
    int32 Subsampling = TJSAMP_444; // Chroma subsampling (4:4:4 = no subsampling)
    int32 JPEGQuality = 85; // Quality (0-100)
    int32 Flags = TJFLAG_FASTDCT; // Optimize for speed over quality

    // Compress to JPEG
    unsigned char* JPEGBuffer = nullptr; // Output buffer allocated by TurboJPEG
    unsigned long JPEGSize = 0;

    int Result = tjCompress2(
        tjInstance,
        RGBData.GetData(), // Input RGB buffer
        Width,
        Width * 3, // Stride (pitch) = width * 3 bytes per pixel (RGB)
        Height,
        PixelFormat,
        &JPEGBuffer, // Output buffer (allocated by TurboJPEG)
        &JPEGSize,   // Output size
        Subsampling,
        JPEGQuality,
        Flags
    );

    if (Result != 0) {
        UE_LOG(LogTemp, Error, TEXT("TurboJPEG compression failed: %s"), tjGetErrorStr());
        tjDestroy(tjInstance);
        return;
    }

    // Copy compressed data to Unreal's TArray
    OutJPEG.Append(JPEGBuffer, JPEGSize);

    // Cleanup
    tjFree(JPEGBuffer);
    tjDestroy(tjInstance);
}

void AZmqPublisher::SendHeartbeat()
{
    // Health check topic is hearbeat
    PublishString("heartbeat", FDateTime::Now().ToString());
}