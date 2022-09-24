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

	virtual void PlayerEliminated(ABlasterCharacter* ElimPlayer, class ABlasterPlayerController* ElimController, ABlasterPlayerController* InsigController);
};
