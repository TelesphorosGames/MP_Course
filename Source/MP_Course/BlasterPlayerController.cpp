// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"

#include "BlasterCharacter.h"
#include "BlasterHud.h"
#include "CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MP_Course.h"


void ABlasterPlayerController::SetHudHealth(float Health, float MaxHealth)
{
	if(BlasterHud == nullptr)
	{
		BlasterHud = Cast<ABlasterHud>(GetHUD());
	}
	// Check to make sure health bar elements are valid 
	if(BlasterHud &&
		BlasterHud->CharacterOverlay &&
		BlasterHud->CharacterOverlay->HealthBar &&
		BlasterHud->CharacterOverlay->HealthText)
	{
		const float HealthPercent = Health / MaxHealth;
		BlasterHud->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		BlasterHud->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
	}
}

void ABlasterPlayerController::SetHudScore(float Score)
{
	if(BlasterHud == nullptr)
	{
		BlasterHud = Cast<ABlasterHud>(GetHUD());
	}
	if(BlasterHud &&
		BlasterHud->CharacterOverlay &&
		BlasterHud->CharacterOverlay->ScoreAmount)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::CeilToInt(Score));
		BlasterHud->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
}

void ABlasterPlayerController::SetHudDefeats(int32 Defeats)
{
	if(BlasterHud == nullptr)
	{
		BlasterHud = Cast<ABlasterHud>(GetHUD());
	}
	if(BlasterHud &&
		BlasterHud->CharacterOverlay &&
		BlasterHud->CharacterOverlay->DefeatsAmount)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		BlasterHud->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
}

void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);
	if(BlasterCharacter)
	{
		SetHudHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
	}

}

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BlasterHud = Cast<ABlasterHud>(GetHUD());
	
}
