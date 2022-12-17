// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TeamTypes.h"
#include "BlasterPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MP_COURSE_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;
	void AddToDefeats(int32 DefeatsAmount);
	UFUNCTION()
	virtual void OnRep_Defeats();

	void AddToScore(float ScoreAmount);
	virtual void OnRep_Score() override;

	FORCEINLINE ETeam GetTeam() const {return Team ;}
	void SetTeam(const ETeam TeamToSet);
	

protected:
	virtual void BeginPlay() override;
	

	
private:

	UPROPERTY()
	class ABlasterCharacter* Character{};
	UPROPERTY()
	class ABlasterPlayerController* BlasterPlayerController{};
	UPROPERTY(ReplicatedUsing=OnRep_Defeats)
	int32 Defeats;

	UPROPERTY(ReplicatedUsing = OnRep_Team)
	ETeam Team = ETeam::ET_TeamDefault;
	
	UFUNCTION()
	void OnRep_Team();
	
};
