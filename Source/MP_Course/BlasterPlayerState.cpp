// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"

#include "BlasterCharacter.h"
#include "BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerState, Defeats);
	DOREPLIFETIME(ABlasterPlayerState, Team);
}


void ABlasterPlayerState::BeginPlay()
{
	Super::BeginPlay();

	SetScore(0);
	BlasterPlayerController=Cast<ABlasterPlayerController>(GetPlayerController());
	if(BlasterPlayerController)
	{
		BlasterPlayerController->SetHudScore(GetScore());
	
	}

}

void ABlasterPlayerState::SetTeam(const ETeam TeamToSet)
{
	Team = TeamToSet;

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
	if(BlasterCharacter)
	{
		BlasterCharacter->SetTeamColor(Team);
	}
}

void ABlasterPlayerState::OnRep_Team()
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
	if(BlasterCharacter)
	{
		BlasterCharacter->SetTeamColor(Team);
	}
}


void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore()+ScoreAmount);

	if(Character==nullptr)	Character = Cast<ABlasterCharacter>(GetPawn());
	if(Character)
	{
		if(BlasterPlayerController == nullptr) BlasterPlayerController=Cast<ABlasterPlayerController>(Character->Controller);
		if(BlasterPlayerController)
		{
			BlasterPlayerController->SetHudScore(GetScore());
		}
	}
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	if(Character==nullptr)	Character = Cast<ABlasterCharacter>(GetPawn());
	if(Character)
	{
		if(BlasterPlayerController == nullptr) BlasterPlayerController=Cast<ABlasterPlayerController>(Character->Controller);
		if(BlasterPlayerController)
		{
			BlasterPlayerController->SetHudScore(GetScore());
		}
	}
}

void ABlasterPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount ;

	if(Character==nullptr)	Character = Cast<ABlasterCharacter>(GetPawn());
	if(Character)
	{
		if(BlasterPlayerController == nullptr) BlasterPlayerController=Cast<ABlasterPlayerController>(Character->Controller);
		if(BlasterPlayerController)
		{
			BlasterPlayerController->SetHudDefeats(Defeats);
		}
	}
}

void ABlasterPlayerState::OnRep_Defeats()
{
	if(Character==nullptr)	Character = Cast<ABlasterCharacter>(GetPawn());
	if(Character)
	{
		if(BlasterPlayerController == nullptr) BlasterPlayerController=Cast<ABlasterPlayerController>(Character->Controller);
		if(BlasterPlayerController)
		{
			BlasterPlayerController->SetHudDefeats(Defeats);
		}
	}
}
