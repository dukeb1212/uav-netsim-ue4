// Fill out your copyright notice in the Description page of Project Settings.


#include "ZmqPublisher.h"
#include "Async/Async.h"
#include "Kismet/GameplayStatics.h"
#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"

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
    // Create weak link to avoid accessing deleted instance
    TWeakObjectPtr<AZmqPublisher> WeakThis(this);

    // Run on background thread to avoid blocking main game thread
    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis, Topic, Message]() {
        if (!WeakThis.IsValid()) return;

        // Get the current instance
        AZmqPublisher* Publisher = WeakThis.Get();
        FScopeLock Lock(&(Publisher->ZmqMutex));
        try {
            std::string fullMessage = TCHAR_TO_UTF8(*(Topic + " " + Message));

            // Send with flag dont wait -> if socket not ready, drop the msg
            Publisher->Socket.send(zmq::buffer(fullMessage), zmq::send_flags::dontwait);
        }
        catch (const zmq::error_t& e) {
            UE_LOG(LogTemp, Error, TEXT("ZMQ send error: %s"), UTF8_TO_TCHAR(e.what()));
        }
        });
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

void AZmqPublisher::SendHeartbeat()
{
    // Health check topic is hearbeat
    PublishString("heartbeat", FDateTime::Now().ToString());
}