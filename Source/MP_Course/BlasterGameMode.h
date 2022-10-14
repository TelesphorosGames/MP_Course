// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

class ABlasterCharacter;

/**
 * 
 */
UCLASS()
class MP_COURSE_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABlasterGameMode();

	virtual void Tick(float DeltaSeconds) override;
	virtual void RequestRespawn(class ACharacter* ElimmedCharacter, AController* ElimmedController);

	FORCEINLINE float GetWarmupTime() const {return WarmupTime ;}
	FORCEINLINE float GetLevelStartingTime() const {return LevelStartingTime ;}
	FORCEINLINE float GetMatchTime() const {return MatchTime ;}
	FORCEINLINE float GetCooldownTime() const {return CooldownTime ;}
	FORCEINLINE float GetCountdownTime() const {return CountDownTime ;}
	
	virtual void PlayerEliminated(ABlasterCharacter* ElimPlayer, class ABlasterPlayerController* ElimController, ABlasterPlayerController* InstigController);
	void FindFurthestPlayerStart(ACharacter* ElimmedCharacter, AActor*& FurthestPlayerStart);

	UFUNCTION()
	void RemoveCharacterOverlay(ABlasterPlayerController* BlasterPlayer);
protected:

	virtual void BeginPlay() override;

	virtual void OnMatchStateSet() override;


private:
	UPROPERTY()
	float CountDownTime = 0.f;
	UPROPERTY()
	float WarmupTime = 10.f;
	UPROPERTY()
	float LevelStartingTime = 0.f;
	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;
	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	void ResetTimes();
	
	FTimerHandle RestartGameTimer;
	UPROPERTY()
	float RestartGameTime = {1.f};
	UPROPERTY()
	bool bRestarted = false;
	UFUNCTION()
	void RestartBlasterGame();

	
};
