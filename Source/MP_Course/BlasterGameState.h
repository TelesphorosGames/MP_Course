// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BlasterGameState.generated.h"

class ABlasterPlayerState;
/**
 * 
 */
UCLASS()
class MP_COURSE_API ABlasterGameState : public AGameState
{
	GENERATED_BODY()

public:

	

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	void UpdateTopScore(ABlasterPlayerState* ScoringPlayer);
	
	UPROPERTY(Replicated)
	TArray<ABlasterPlayerState*> TopScoringPlayers;

	
	TArray<ABlasterPlayerState*> TeamOne;
	TArray<ABlasterPlayerState*> TeamTwo;

	void TeamOneScores();
	void TeamTwoScores();
	
	UPROPERTY(ReplicatedUsing = OnRep_TeamOneScore)
	float TeamOneScore = 0.f;
	UFUNCTION()
	void OnRep_TeamOneScore();
	UPROPERTY(ReplicatedUsing = OnRep_TeamTwoScore)
	float TeamTwoScore = 0.f;
	UFUNCTION()
	void OnRep_TeamTwoScore();
	
protected:

private:

	float TopScore = 0.f;
	
};
