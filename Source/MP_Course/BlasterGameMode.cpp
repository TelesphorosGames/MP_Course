// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "BlasterCharacter.h"
#include "BlasterGameState.h"
#include "BlasterPlayerController.h"
#include "BlasterPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

namespace MatchState
{
	const FName CoolDown = FName("Cooldown");
}

ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;
}

void ABlasterGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(MatchState == MatchState::WaitingToStart)
	{
		CountDownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if(CountDownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if(MatchState==MatchState::InProgress)
	{
		CountDownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if(CountDownTime <= 0.f)
		{
			SetMatchState(MatchState::CoolDown);
		}
	}
	else if(MatchState == MatchState::CoolDown)
	{
		CountDownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if(CountDownTime<=0.f)
		{
			RestartGame();
		}
	}
}

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimPlayer, ABlasterPlayerController* ElimController,
                                        ABlasterPlayerController* InstigController)
{
	ABlasterPlayerState* AttackerPlayerState = InstigController ? Cast<ABlasterPlayerState>(InstigController->PlayerState) : nullptr;
	ABlasterPlayerState* VictimPlayerState = ElimController ? Cast<ABlasterPlayerState>(ElimController->PlayerState) : nullptr ;

	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
	
	if(AttackerPlayerState && VictimPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1);
		if(BlasterGameState)
		{
			BlasterGameState->UpdateTopScore(AttackerPlayerState);
		}
		VictimPlayerState->AddToDefeats(1);
	}
	
	if(ElimPlayer)
	{
		ElimPlayer->Elim();
	}
}

void ABlasterGameMode::FindFurthestPlayerStart(ACharacter* ElimmedCharacter, AActor*& FurthestPlayerStart)
{
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
	FurthestPlayerStart = {};
	float LongestDistance{};
	for (AActor*& PlayerStart : PlayerStarts)
	{
		const float DistanceToStart = FVector::Distance(ElimmedCharacter->GetActorLocation(), PlayerStart->GetActorLocation());
		if(DistanceToStart>LongestDistance)
		{
			LongestDistance = DistanceToStart;
			FurthestPlayerStart = PlayerStart;
		}
	}
}

void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();

}

void ABlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if(BlasterPlayer)
		{
			BlasterPlayer->OnMatchStateSet(MatchState);
		}
		
	}
	
}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{

	
	if(ElimmedController && ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
		
		AActor* LongestDistancePlayerStart;
		FindFurthestPlayerStart(ElimmedCharacter, LongestDistancePlayerStart);
		RestartPlayerAtPlayerStart(ElimmedController, LongestDistancePlayerStart);
	}
}
