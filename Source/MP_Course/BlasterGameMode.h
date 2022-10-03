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


	
	virtual void PlayerEliminated(ABlasterCharacter* ElimPlayer, class ABlasterPlayerController* ElimController, ABlasterPlayerController* InstigController);
	void FindFurthestPlayerStart(ACharacter* ElimmedCharacter, AActor*& FurthestPlayerStart);

protected:

	virtual void BeginPlay() override;


	
private:
	
	float CountDownTime = 0.f;
	float WarmupTime = 10.f;
	float LevelStartingTime = 0.f;


};
