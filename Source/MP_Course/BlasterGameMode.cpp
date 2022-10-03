// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "BlasterCharacter.h"
#include "BlasterPlayerController.h"
#include "BlasterPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

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
}

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimPlayer, ABlasterPlayerController* ElimController,
                                        ABlasterPlayerController* InstigController)
{
	ABlasterPlayerState* AttackerPlayerState = InstigController ? Cast<ABlasterPlayerState>(InstigController->PlayerState) : nullptr;
	ABlasterPlayerState* VictimPlayerState = ElimController ? Cast<ABlasterPlayerState>(ElimController->PlayerState) : nullptr ;

	if(AttackerPlayerState && VictimPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1);
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
