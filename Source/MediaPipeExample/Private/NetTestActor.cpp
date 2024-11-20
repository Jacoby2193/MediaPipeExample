﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "NetTestActor.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "Runtime/Networking/Public/Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h" 

ANetTestActor::ANetTestActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ANetTestActor::BeginPlay()
{
	Super::BeginPlay();
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
	if (ClientSocket && ClientSocket->GetConnectionState()==SCS_Connected)
	{
		uint32 Size;
		//GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Green, FString::Printf(TEXT("연결성공 : 데이터 확인 %d"), ClientSocket->HasPendingData(Size)));
		if (ClientSocket->HasPendingData(Size))
		{
			TArray<uint8> ReceivedData;
			ReceivedData.SetNumUninitialized(FMath::Min(Size, 1024u));

			// 데이터 수신
			int32 BytesRead = 0;
			if (ClientSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), BytesRead))
			{
				if (BytesRead>0)
				{
					FString ReceivedString = FString(ReceivedData.Num(), UTF8_TO_TCHAR(ReceivedData.GetData()));

					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("BytesRead : %d, Received: %s"), BytesRead, *ReceivedString));
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("No data received."));
				}

			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Failed to receive data."));
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Red, TEXT("Socket is not connected or does not exist."));
	}
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

void ANetTestActor::TestRecieveData()
{
	if (!ClientSocket)
		return;

	// uint32 pendingSize;
	// if (!ClientSocket->HasPendingData(pendingSize) )
	// {
	// 	return;
	// }

	TArray<uint8> headerInfo;
	int32 HeaderSize = 1;
	headerInfo.AddZeroed(HeaderSize);

	int readNum = 0;

	bool bSuccess = ClientSocket->Recv(headerInfo.GetData(), HeaderSize, readNum, ESocketReceiveFlags::Type::WaitAll);

	if (readNum>0)
	{
		TArray<uint8> BodyInfo;
		int32 BodySize = headerInfo[0];
		BodyInfo.AddZeroed(HeaderSize);

		int32 Offset = 0;
		uint8* results = BodyInfo.GetData();
		while (BodySize>0)
		{
			int32 NumRead = 0;
			ClientSocket->Recv(results+Offset, BodySize, NumRead);

			if (NumRead<=0)
			{
				break;
			}

			Offset += NumRead;
			BodySize -= NumRead;
		}

		GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Green, FString::Printf(TEXT("데이터 확인 %d"), BodyInfo[0]));
	}

}

