

#pragma once

#include "CoreMinimal.h"
#include <zmq.hpp>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
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

	// Event tracker for updating network events.
	UPROPERTY(BlueprintAssignable, Category = "ZMQ")
	FOnZmqMessageReceived OnMessageReceived;

	// Start queueing messages
	UFUNCTION(BlueprintCallable, Category = "ZMQ")
	void StartListening();

	UFUNCTION(BlueprintCallable, Category = "ZMQ")
	bool IsValidTcpAddress(const FString& Address);

	UFUNCTION(BlueprintCallable, Category = "ZMQ")
	void ChangeAddress(const FString& NewAddress);

	UPROPERTY(BlueprintReadOnly, Category = "ZMQ Settings")
	FString ConnectionAddress = "tcp://127.0.0.1:555";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZMQ Settings")
	FString DefaultTopic = "network";

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void StopListening();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	zmq::context_t Context;
	zmq::socket_t Socket;

	std::thread SubscriberThread;
	std::atomic<bool> bRunning = false;
	bool bThreadStopped = true;
	FCriticalSection ZmqMutex;
	std::mutex ThreadControlMutex;
	std::condition_variable ThreadStoppedCV;
	FString LastMessage;

	void StartZmqSubscribe();
	
	void RebindZmqSubscriber(const FString& NewAddress);
};
