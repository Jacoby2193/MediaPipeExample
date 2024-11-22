// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetTestActor.generated.h"


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

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FString ServerIP = TEXT("127.0.0.1");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	int32 ServerPort = 23456;

	FSocket* ClientSocket;

	TSharedPtr<FInternetAddr> ServerAddr;

	UFUNCTION(BlueprintCallable)
    void CreateClient(FString ip, int32 port);
	
	UFUNCTION(BlueprintCallable)
    void ReceiveData();
	
	UFUNCTION(BlueprintCallable)
	void Disconnect();


	

};
