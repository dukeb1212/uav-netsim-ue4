


#include "ZmqSubscriber.h"
#include "Async/Async.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AZmqSubscriber::AZmqSubscriber()
    : Context(1), Socket(Context, ZMQ_SUB)
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AZmqSubscriber::BeginPlay()
{
    Super::BeginPlay();

    // Start at beginning
    StartZmqSubscribe();
    StartListening();
}

// End the subscribe thread
AZmqSubscriber::~AZmqSubscriber()
{
    bRunning = false;
    if (SubscriberThread.joinable())
    {
        SubscriberThread.join();
    }
}

// Called every frame
void AZmqSubscriber::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void AZmqSubscriber::StartListening()
{
    bRunning = true;

    SubscriberThread = std::thread([this]() {
        while (bRunning)
        {
            zmq::message_t receivedMessage;

            try {
                // Lock to avoid changing context and socket
                FScopeLock Lock(&ZmqMutex);

                zmq::recv_result_t result = Socket.recv(receivedMessage, zmq::recv_flags::dontwait);
                if (!result) {
                    FPlatformProcess::Sleep(0.01f); // avoid busy loop
                    continue;
                }
                
                // Parse zmq message to string
                std::string receivedStr(static_cast<char*>(receivedMessage.data()), receivedMessage.size());
                FString ReceivedFString = UTF8_TO_TCHAR(receivedStr.c_str());

                FString Topic, Message;

                // Split the topic and data
                if (ReceivedFString.Split(TEXT(" "), &Topic, &Message))
                {
                    //UE_LOG(LogTemp, Log, TEXT("Received Topic: %s, Message: %s"), *Topic, *Message);

                    // Broadcast the message for others to handle
                    AsyncTask(ENamedThreads::GameThread, [this, Topic, Message]() {
                        if (OnMessageReceived.IsBound())
                        {
                            OnMessageReceived.Broadcast(Topic, Message);
                        }
                        });
                }
            }
            catch (const zmq::error_t& e) {
                UE_LOG(LogTemp, Error, TEXT("ZMQ receive error: %s"), UTF8_TO_TCHAR(e.what()));
            }
        }
        });

    // Detach when destroy
    SubscriberThread.detach();
}

bool AZmqSubscriber::IsValidTcpAddress(const FString& Address)
{
    const FRegexPattern Pattern(TEXT("^tcp:\\/\\/((25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]?\\d)(\\.(?!$)|$)){4}:\\d{1,5}$"));
    FRegexMatcher Matcher(Pattern, Address);
    return Matcher.FindNext();
}

void AZmqSubscriber::ChangeAddress(const FString& NewAddress)
{
    FScopeLock Lock(&ZmqMutex);
    bRunning = false; // Signal thread to stop
    Socket.close();

    if (SubscriberThread.joinable())
    {
        SubscriberThread.join();
    }

    Socket = zmq::socket_t(Context, ZMQ_SUB);

    RebindZmqSubscriber(NewAddress);
}


void AZmqSubscriber::StartZmqSubscribe()
{
    try {
        // Initialize the socket
        FScopeLock Lock(&ZmqMutex);
        Socket.connect(TCHAR_TO_UTF8(*ConnectionAddress));
        Socket.setsockopt(ZMQ_SUBSCRIBE, TCHAR_TO_UTF8(*DefaultTopic), 0);
        UE_LOG(LogTemp, Warning, TEXT("ZMQ subscriber connect good"));
    }
    catch (const zmq::error_t& e) {
        UE_LOG(LogTemp, Error, TEXT("ZMQ subscriber connect failed: %s"), UTF8_TO_TCHAR(e.what()));
		// Retry connection
		FTimerHandle RetryHandle;
		GetWorldTimerManager().SetTimer(RetryHandle, this, &AZmqSubscriber::StartZmqSubscribe, 1.0f, false);
    }
}

void AZmqSubscriber::RebindZmqSubscriber(const FString& NewAddress)
{
    ConnectionAddress = NewAddress;
    StartZmqSubscribe();
    StartListening();
}


