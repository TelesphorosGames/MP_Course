// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

/**
 * 
 */

UCLASS()
class MP_COURSE_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	virtual void ReceivedPlayer() override;

	void SetHudHealth(float Health, float MaxHealth);
	void SetHudScore(float Score);
	void SetHudDefeats(int32 Defeats);
	void SetHudWeaponAmmo(int32 Ammo);
	void SetHudCarriedAmmo(int32 Ammo);
	void SetHuDCountdownTime(float CountdownTime);
	void OnPossess(APawn* InPawn) override;

	// Synced with server world clock
	virtual float GetServerTime();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	void SetHudTime();

	// Sync time between Client and Server :

	// Requests Current server time, passing in client's time when request was sent 
	UFUNCTION(Server, Reliable)
	void Server_RequestServerTime(float TimeOfClientRequest);
	// Reports current server time to the client in reponse to Server_RequestServerTime
	UFUNCTION(Client, Reliable)
	void Client_ReportServerTime(float TimeOfClientRequest, float TimeServerRecievedRequest);
	// Difference Between Client and Server Time
	float ClientServerDelta = 0.f;
	// How often client and server times should sync
	UPROPERTY(EditAnywhere, Category= "Time")
	float TimeSyncFrequency = 5.f;
	float TimeSyncRunningTime = 0.f;

	void CheckTimeSync(float DeltaTime);
	

	
	
	
private:
	UPROPERTY()
    	class ABlasterHud* BlasterHud{};

	float MatchTime = 120.f;

	uint32 CountDownInt=0;
	
};
