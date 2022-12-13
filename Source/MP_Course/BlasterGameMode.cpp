// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"

#include "Announcement.h"
#include "BlasterCharacter.h"
#include "BlasterGameState.h"
#include "BlasterHud.h"
#include "BlasterPlayerController.h"
#include "BlasterPlayerState.h"
#include "CharacterOverlay.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"


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
	if(MatchState==MatchState::InProgress)
	{
		CountDownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if(CountDownTime <= 0.f)
		{
			SetMatchState(MatchState::WaitingPostMatch);
			
		}
	}
	if(MatchState == MatchState::WaitingPostMatch)
	{
		CountDownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if(CountDownTime<=0.f && !bRestarted)
		{
			GetWorld()->ServerTravel("/Game/VersionControlled/MyStuff/Levels/OpenWorld?Restart", false, false);
			bRestarted = true;
		}
	}
}

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimPlayer, ABlasterPlayerController* ElimController,
                                        ABlasterPlayerController* InstigController)
{
	ABlasterPlayerState* AttackerPlayerState = InstigController ? Cast<ABlasterPlayerState>(InstigController->PlayerState) : nullptr;
	ABlasterPlayerState* VictimPlayerState = ElimController ? Cast<ABlasterPlayerState>(ElimController->PlayerState) : nullptr ;

	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
	
	if(AttackerPlayerState && VictimPlayerState)
	{
		if(AttackerPlayerState != VictimPlayerState)
		{
			AttackerPlayerState->AddToScore(1);
		}
		if(BlasterGameState)
		{
			BlasterGameState->UpdateTopScore(AttackerPlayerState);
		}
		VictimPlayerState->AddToDefeats(1);
	}
	
	if(ElimPlayer)
	{
		ElimPlayer->Elim(false);
	}

	for(FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if(BlasterPlayer && AttackerPlayerState && VictimPlayerState)
		{
			BlasterPlayer->BroadcastEliminated(AttackerPlayerState, VictimPlayerState);
		}
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
	
	// for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	for(ABlasterPlayerController*  BlasterPlayer : TActorRange<ABlasterPlayerController>(GetWorld()))
	{
		// ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if(BlasterPlayer)
		{
			UE_LOG(LogTemp,Warning, TEXT("%s"), *BlasterPlayer->GetName());
			BlasterPlayer->OnMatchStateSet(MatchState);
		}
		
	}
}

void ABlasterGameMode::RemoveCharacterOverlay(ABlasterPlayerController* BlasterPlayer)
{
	if(BlasterPlayer &&
		BlasterPlayer->BlasterHud &&
		BlasterPlayer->BlasterHud->CharacterOverlay
		)
		{
		BlasterPlayer->BlasterHud->RemoveCharacterOverlay();
		}
	
	if(BlasterPlayer &&
		BlasterPlayer->BlasterHud &&
		BlasterPlayer->BlasterHud->AnnouncementWidget)
	{
		BlasterPlayer->BlasterHud->RemoveAnnouncement();
	}
}


void ABlasterGameMode::ResetTimes()
{
	CountDownTime=0.f;
	WarmupTime = 10.f;
	MatchTime = 10.f;
	CooldownTime = 10.f;
}

void ABlasterGameMode::RestartBlasterGame()
{

	UWorld* World = GetWorld();
	if(World)
	{
		for(ABlasterPlayerController*  BlasterPlayer : TActorRange<ABlasterPlayerController>(World))
		{
			if(BlasterPlayer)
			{
				RemoveCharacterOverlay(BlasterPlayer);
			}
		}
	}
	GetWorld()->ServerTravel("/Game/VersionControlled/MyStuff/Levels/OpenWorld?Restart", false, false);

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

void ABlasterGameMode::PlayerLeftGame(ABlasterPlayerState* PlayerLeftGame)
{
	if(PlayerLeftGame == nullptr)
	{
		return;
	}
	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
	if(BlasterGameState && BlasterGameState->TopScoringPlayers.Contains(PlayerLeftGame))
	{
		BlasterGameState->TopScoringPlayers.Remove(PlayerLeftGame);
	}
	ABlasterCharacter* CharacterLeaving = Cast<ABlasterCharacter>(PlayerLeftGame->GetPawn());
	if(CharacterLeaving)
	{
		CharacterLeaving->Elim(true);
	}
}