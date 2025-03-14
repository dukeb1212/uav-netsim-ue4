

#pragma once

#include "CoreMinimal.h"
#include <zmq.hpp>
#include <thread>
#include "Delegates/Delegate.h"
#include "GameFramework/Actor.h"
#include "ZmqSubscriber.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnZmqMessageReceived, const FString&, Topic, const FString&, Message);

UCLASS()
class UAVNETSIM_API AZmqSubscriber : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AZmqSubscriber();

	virtual ~AZmqSubscriber();

	// Event tracker for updating network events.
	UPROPERTY(BlueprintAssignable, Category = "ZMQ")
	FOnZmqMessageReceived OnMessageReceived;

	// Start queueing messages
	UFUNCTION(BlueprintCallable, Category = "ZMQ")
	void StartListening();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZMQ Settings")
	FString ConnectionAddress = "tcp://localhost:5555";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZMQ Settings")
	FString DefaultTopic = "network";

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	zmq::context_t Context;
	zmq::socket_t Socket;

	std::thread SubscriberThread;
	std::atomic<bool> bRunning = false;
	FCriticalSection ZmqMutex;
	FString LastMessage;

	void StartZmqSubscribe();
	
};
