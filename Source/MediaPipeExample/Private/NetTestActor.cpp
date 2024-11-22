// Fill out your copyright notice in the Description page of Project Settings.


#include "NetTestActor.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "Runtime/Networking/Public/Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h" 
#include "cstdlib"
#include "cstdlib"
#include "GameFramework/Controller.h"
#include "MyPlayerController.h"

ANetTestActor::ANetTestActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ANetTestActor::BeginPlay()
{
	Super::BeginPlay();
}

void ANetTestActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	Disconnect();
}

void ANetTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ReceiveData();
}

void ANetTestActor::CreateClient(FString ip, int32 port)
{
	// 클라이언트 소켓 생성
	ClientSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("TCP Client"), false);
	ServerAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	// 서버 IP와 포트 설정
	FIPv4Address Ip;
	FIPv4Address::Parse(ip, Ip);
	ServerAddr->SetIp(Ip.Value);
	ServerAddr->SetPort(port);

	// 서버에 연결
	FString connStr = ClientSocket->Connect(*ServerAddr) ? TEXT("서버에 연결되었습니다.") : TEXT("서버 연결 실패.");

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("%s"), *connStr));
}

void ANetTestActor::ReceiveData()
{
	uint32 pendingSize;
	int32 headerSize = 4;

	TArray<uint8> headerData;
	TArray<uint8> bodyData;

	// 데이터 수신
	int32 bytesRead = 0;
	int32 packetSize = 0;
	int32 cmd = 0;
	// 헤더 분석
	int checkRead = headerSize;

	if (nullptr == ClientSocket ||
		ClientSocket->GetConnectionState() != SCS_Connected || 
		!ClientSocket->HasPendingData(pendingSize))
	{
		return;
	}

	headerData.Empty(headerSize);
	headerData.SetNum(headerSize);
	// 데이터 수신
	bytesRead = 0;
	packetSize = 0;
	cmd = 0;
	// 헤더 분석
	ClientSocket->Recv(headerData.GetData(), headerData.Num(), bytesRead);
	cmd = headerData[2];
	packetSize = headerData[3];

	// 바디 분석
	bodyData.Empty(packetSize);
	bodyData.SetNum(packetSize);
	checkRead = packetSize;
	ClientSocket->Recv(bodyData.GetData(), bodyData.Num(), bytesRead);
	int32 f1 = bodyData[0]<<24|bodyData[1]<<16|bodyData[2]<<8|bodyData[3];
	int32 f2 = bodyData[4]<<24|bodyData[5]<<16|bodyData[6]<<8|bodyData[7];
	FString ReceivedString = FString::Printf(TEXT("f1 : %d, f2 : %d"), f1, f2);

	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("BytesRead : %d, Received: %s"), bytesRead, *ReceivedString));
}

void ANetTestActor::Disconnect()
{
	if (ClientSocket)
	{
		ClientSocket->Close();
		ClientSocket = nullptr;
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("서버와 연결 종료"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("소켓 없음"));
	}

}

