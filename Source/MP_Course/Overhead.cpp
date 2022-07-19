// Fill out your copyright notice in the Description page of Project Settings.


#include "Overhead.h"

#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"


void UOverhead::SetDisplayText(FString TextToDisplay)
{

	if(DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverhead::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole LocalRole = InPawn->GetLocalRole();
	FString Role;
	switch(LocalRole)
	{
	case ENetRole::ROLE_Authority:
		Role = FString("Authority");
		break;
	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;
	case ENetRole::ROLE_None:
		Role = FString("None");
		break;
	default: ;
	}

	APlayerState* PlayerState = GetOwningPlayerState();
	if(PlayerState)
	{
		  auto PlayerName = PlayerState->GetPlayerName();
        	
        	FString LocalRoleString = FString::Printf(TEXT("Local Role: %s, Player Name : %s"), *Role, *PlayerName);
        
        	if(DisplayText)
        	{
        		SetDisplayText(LocalRoleString);
        	}
	}
  
	
}

void UOverhead::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{

	RemoveFromParent();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);

	
}
