


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

void AZmqSubscriber::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    StopListening();

    FScopeLock Lock(&ZmqMutex);
    try {
        Socket.close();
        Context.close();
    }
    catch (...) {
        UE_LOG(LogTemp, Warning, TEXT("Error during ZMQ shutdown"));
    }
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AZmqSubscriber::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void AZmqSubscriber::StartListening()
{
    std::lock_guard<std::mutex> ThreadLock(ThreadControlMutex);
    bRunning = true;
    bThreadStopped = false;

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

        std::lock_guard<std::mutex> Lock(ThreadControlMutex);
        bThreadStopped = true;
        ThreadStoppedCV.notify_one();
        });

    // Detach when destroy
    SubscriberThread.detach();
}

void AZmqSubscriber::StopListening()
{
    {
        std::lock_guard<std::mutex> ThreadLock(ThreadControlMutex);
        bRunning = false;
    }

    // Đợi thread thoát ra
    std::unique_lock<std::mutex> WaitLock(ThreadControlMutex);
    ThreadStoppedCV.wait(WaitLock, [this]() { return bThreadStopped; });
}

bool AZmqSubscriber::IsValidTcpAddress(const FString& Address)
{
    UE_LOG(LogTemp, Warning, TEXT("Checking Valide Tcp Address for %s"), *Address);
    const FRegexPattern Pattern(TEXT("^tcp:\\/\\/(\\d{1,3}\\.){3}\\d{1,3}:\\d{1,5}$"));
    FRegexMatcher Matcher(Pattern, Address);

    if (!Matcher.FindNext()) return false;

    // Tách IP và port để kiểm tra giá trị hợp lệ
    FString IpPort = Address.Mid(6); // bỏ "tcp://"
    FString Ip, PortStr;
    if (!IpPort.Split(":", &Ip, &PortStr)) return false;

    TArray<FString> Octets;
    Ip.ParseIntoArray(Octets, TEXT("."));
    if (Octets.Num() != 4) return false;

    for (const FString& Octet : Octets)
    {
        int32 Val = FCString::Atoi(*Octet);
        if (Val < 0 || Val > 255) return false;
    }

    int32 Port = FCString::Atoi(*PortStr);
    if (Port < 1 || Port > 65535) return false;

    return true;
}

void AZmqSubscriber::ChangeAddress(const FString& NewAddress)
{
    UE_LOG(LogTemp, Warning, TEXT("ZMQ Subscriber rebinding..."));

    StopListening();

    FScopeLock Lock(&ZmqMutex);
    
    try {
        Socket.close();
    }
    catch (...) {
        UE_LOG(LogTemp, Warning, TEXT("Error closing ZMQ socket"));
    }


    try {
        Socket = zmq::socket_t(Context, ZMQ_SUB);
    }
    catch (const zmq::error_t& e) {
        UE_LOG(LogTemp, Error, TEXT("Socket recreate failed: %s"), UTF8_TO_TCHAR(e.what()));
    }

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
    UE_LOG(LogTemp, Warning, TEXT("ZMQ Subscriber rebinding to %s"), *ConnectionAddress);
    StartZmqSubscribe();
    StartListening();
}


