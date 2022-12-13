// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);


/**
 * 
 */

UCLASS()
class MP_COURSE_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void ReceivedPlayer() override;
	void CheckPing(float DeltaSeconds);

	FORCEINLINE float GetWarmupTime() const	{return WarmupTime ;}
	FORCEINLINE FName GetBlasterMatchState() const {return BlasterMatchState ;}


	void SetHudHealth(float Health, float MaxHealth);
	void SetHudShields(float Shields, float MaxShields);
	void SetHudScore(float Score);
	void SetHudDefeats(int32 Defeats);
	void SetHudWeaponAmmo(int32 Ammo);
	void SetHudCarriedAmmo(int32 Ammo);
	void SetHuDCountdownTime(float CountdownTime);
	void SetHudGrenades(int32 Grenades);
	void SetHUDAnnouncementCountdown(float Time);
	void OnPossess(APawn* InPawn) override;

	void OnMatchStateSet(FName State);
	void RemoveOverlay();
	void HandleMatchHasStarted();
	void HandleCooldown();

	
	// Synced with server world clock
	virtual float GetServerTime();

	UPROPERTY()
	class ABlasterHud* BlasterHud{};

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	
	UPROPERTY()
	float MatchTime = 0.f;
	UPROPERTY()
	float WarmupTime = 0.f;
	UPROPERTY()
	float LevelStartingTime = 0.f;
	UPROPERTY()
	float CoolDownTime = 0.f;
	UPROPERTY()
	uint32 CountDownInt=0;
	
	UPROPERTY()
	class ABlasterGameMode* BlasterGameMode{};
	UPROPERTY()
	ABlasterPlayerController* BlasterPlayerController{};

	UPROPERTY()
	float SingleTripTime = 0;

	FHighPingDelegate HighPingDelegate;

	void BroadcastEliminated(APlayerState* Attacker, APlayerState* Victim);
	
protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupInputComponent() override;
	
	void SetHudTime();
	void PollInit();
	
	// Sync time between Client and Server :

	// Requests Current server time, passing in client's time when request was sent 
	UFUNCTION(Server, Reliable)
	void Server_RequestServerTime(float TimeOfClientRequest);
	// Reports current server time to the client in reponse to Server_RequestServerTime
	UFUNCTION(Client, Reliable)
	void Client_ReportServerTime(float TimeOfClientRequest, float TimeServerRecievedRequest);
	// Difference Between Client and Server Time
	UPROPERTY()
	float ClientServerDelta = 0.f;
	// How often client and server times should sync
	UPROPERTY(EditAnywhere, Category= "Time")
	float TimeSyncFrequency = 5.f;
	UPROPERTY()
	float TimeSyncRunningTime = 0.f;
	UFUNCTION()
	void CheckTimeSync(float DeltaTime);
	
	
	UFUNCTION(Server, Reliable)
	void Server_CheckMatchState();
	UFUNCTION(Client, Reliable)
	void Client_JoinMidGame(FName StateOfMatch, float Warmup, float Match, float StartingTime, float Cooldown);

	void HighPingWarning(int32 WarningPing);
	void StopHighPingWarning();

	void ShowReturnToMainMenu();

	UFUNCTION(Client, Reliable)
	void Client_ElimAnnouncement(APlayerState* Attacker, APlayerState* Victim);

private:

	UPROPERTY(EditAnywhere, Category="HUD");
	TSubclassOf<class UUserWidget> ReturnToMainMenuWidget;

	UPROPERTY()
	class UMenuReturnWidget* ReturnWidget{};

	bool bMenuWidgetOpen = false;
	
	
	UPROPERTY(ReplicatedUsing=OnRep_MatchState)
	FName BlasterMatchState{};
	
	UFUNCTION()
	void OnRep_MatchState();
	UPROPERTY()
	bool bInitializeCharacterOverlay = false;
	UPROPERTY()
	float HudHealth{};
	UPROPERTY()
	float HudMaxHealth{};
	UPROPERTY()
	float HudShield{};
	UPROPERTY()
	float HudMaxShield{};
	
	UPROPERTY()
	float HudScore;
	UPROPERTY()
	int32 HudDefeats;

	UPROPERTY()
	int32 HudGrenades = 4;

	FTimerHandle RemoveOverlayTimer;

	UPROPERTY()
	bool bHealthInitialized = false; 
	UPROPERTY()
	bool bScoreInitialized = false; 
	UPROPERTY()
	bool bDefeatsInitialized = false;
	UPROPERTY()
	bool bGrenadesInitialized = false;
	UPROPERTY()
	bool bShieldInitialized = false;
	UPROPERTY()
	bool bHudCarriedAmmoInitialized = false;
	UPROPERTY()
	bool bHudWeaponAmmoInitizalized = false;
	UPROPERTY()
	float HudCarriedAmmo;
	UPROPERTY()
	float HudWeaponAmmo;


	float HighPingRunningTime = 10.f;
	UPROPERTY(EditAnywhere)
	float HighPingDuration = 5.f;
	UPROPERTY(EditAnywhere)
	float PingAnimationRunningTime = 0.f;
	UPROPERTY(EditAnywhere)
	float CheckPingFrequency = 20.f;
	UPROPERTY(EditAnywhere)
	float HighPingThreshold = 50.f;

	UFUNCTION(Server, Reliable)
	void Server_ReportPingStatus(bool bHighPing);
	
};
