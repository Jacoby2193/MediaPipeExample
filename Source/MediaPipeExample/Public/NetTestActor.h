// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetTestActor.generated.h"

//
//UENUM(BlueprintType)
//enum class EQTEType : uint8
//{
//	THROW
//	,EATING
//	,CLEAN
//	,GIVE
//	,POUR
//};

//USTRUCT(BlueprintType)
//struct FPyName
//{
//	GENERATED_BODY()
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
//	EQTEType type;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
//	FString pyName;
//
//	FPyName() : type(EQTEType::THROW), pyName(TEXT("")) {}
//};
//
//USTRUCT(BlueprintType)
//struct FUrlName
//{
//	GENERATED_BODY()
//	
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
//	EQTEType type;
//	
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
//	FString urlName;
//
//	FUrlName() : type(EQTEType::THROW), urlName(TEXT("")) {}
//};



UCLASS()
class MEDIAPIPEEXAMPLE_API ANetTestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANetTestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FString serverIP = TEXT("127.0.0.1");
	//FString serverIP = TEXT("192.168.0.53");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	int32 serverPort = 23456;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FString pyDefaultPath;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	//TArray<FPyName> pyNames;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	//TArray<FUrlName> urlNames;

	FSocket* ClientSocket;

	TSharedPtr<FInternetAddr> ServerAddr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	int32 getData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Objects")
	class APawn* player;

	//UFUNCTION(BlueprintCallable)
	//void RunQTEScript(EQTEType type);

	//UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	//void RunPythonScript(const FString& path);

	//UFUNCTION(BlueprintCallable)
	//void RunAsyncScript(EQTEType type);

	//UFUNCTION(BlueprintCallable)
	//void RunAsyncPythonScript(const FString& path);

	void TestRecieveData();

	UFUNCTION(BlueprintCallable)
    void CreateClient(FString ip, int32 port);
	
	UFUNCTION(BlueprintCallable)
    void ReceiveData();
	
	UFUNCTION(BlueprintCallable)
	void Disconnect();


	uint32 Size;
	int32 headerSize = 4;

	TArray<uint8> headerData;
	TArray<uint8> ReceivedData;

	// 데이터 수신
	int32 BytesRead = 0;
	int32 packetSize = 0;
	int32 cmd = 0;
	// 헤더 분석
	int checkRead = headerSize;

};
