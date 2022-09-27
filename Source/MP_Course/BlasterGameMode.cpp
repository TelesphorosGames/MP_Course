// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "BlasterCharacter.h"
#include "BlasterPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimPlayer, ABlasterPlayerController* ElimController,
                                        ABlasterPlayerController* InsigController)
{
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
	for (AActor* PlayerStart : PlayerStarts)
	{
		const float DistanceToStart = FVector::Distance(ElimmedCharacter->GetActorLocation(), PlayerStart->GetActorLocation());
		if(DistanceToStart>LongestDistance)
		{
			LongestDistance = DistanceToStart;
			FurthestPlayerStart = PlayerStart;
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
