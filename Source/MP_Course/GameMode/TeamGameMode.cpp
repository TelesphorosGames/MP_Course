// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "MP_Course/BlasterGameState.h"
#include "MP_Course/BlasterPlayerState.h"


void ATeamGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
	if(BlasterGameState)
	{
		ABlasterPlayerState* BlasterPlayerState = NewPlayer->GetPlayerState<ABlasterPlayerState>();
		if(BlasterPlayerState && BlasterPlayerState->GetTeam() == ETeam::ET_TeamDefault)
		{
			if(BlasterGameState->TeamOne.Num() >= BlasterGameState->TeamTwo.Num())
			{
				BlasterGameState->TeamTwo.AddUnique(BlasterPlayerState);
				BlasterPlayerState->SetTeam(ETeam::ET_TeamTwo);
			}
			else
			{
				BlasterGameState->TeamOne.AddUnique(BlasterPlayerState);
				BlasterPlayerState->SetTeam(ETeam::ET_TeamOne);
			}
		}
	}

	
}

void ATeamGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	ABlasterPlayerState* BlasterPlayerState = Exiting->GetPlayerState<ABlasterPlayerState>();
	ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
	if(BlasterGameState && BlasterPlayerState)
	{
		if(BlasterGameState->TeamOne.Contains(BlasterPlayerState))
		{
			BlasterGameState->TeamOne.Remove(BlasterPlayerState);
		}
		else if(BlasterGameState->TeamTwo.Contains(BlasterPlayerState))
		{
			BlasterGameState->TeamTwo.Remove(BlasterPlayerState);
		}
	}
	
}

float ATeamGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	ABlasterPlayerState* AttackerPlayerState = Attacker->GetPlayerState<ABlasterPlayerState>();
	ABlasterPlayerState* VictimPlayerState = Victim->GetPlayerState<ABlasterPlayerState>();
	if(AttackerPlayerState == nullptr || VictimPlayerState == nullptr || VictimPlayerState == AttackerPlayerState)
	{
		return BaseDamage;
	}
	if(AttackerPlayerState->GetTeam() == VictimPlayerState->GetTeam())
	{
		return 0.f;
	}
	return BaseDamage;
}

void ATeamGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	
	ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
	if(BlasterGameState)
	{
		for(auto PlayerState : BlasterGameState->PlayerArray)
		{
			ABlasterPlayerState* BlasterPlayerState = Cast<ABlasterPlayerState>(PlayerState.Get());
			if(BlasterPlayerState && BlasterPlayerState->GetTeam() == ETeam::ET_TeamDefault)
			{
				if(BlasterGameState->TeamOne.Num() < BlasterGameState->TeamTwo.Num())
				{
					BlasterGameState->TeamOne.AddUnique(BlasterPlayerState);
					BlasterPlayerState->SetTeam(ETeam::ET_TeamOne);
				}
				else
				{
					BlasterGameState->TeamTwo.AddUnique(BlasterPlayerState);
					BlasterPlayerState->SetTeam(ETeam::ET_TeamTwo);
				}
			}
		}
	}
}
