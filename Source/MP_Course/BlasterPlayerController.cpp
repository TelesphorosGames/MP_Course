// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"

#include "BlasterCharacter.h"
#include "BlasterHud.h"
#include "CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MP_Course.h"

void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if(IsLocalController())
	{
		Server_RequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ABlasterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SetHudTime();
	CheckTimeSync(DeltaSeconds);

	
}

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

void ABlasterPlayerController::SetHudWeaponAmmo(int32 Ammo)
{
	if(BlasterHud == nullptr)
	{
		BlasterHud = Cast<ABlasterHud>(GetHUD());
	}
	if(BlasterHud &&
		BlasterHud->CharacterOverlay &&
		BlasterHud->CharacterOverlay->AmmoCount)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		BlasterHud->CharacterOverlay->AmmoCount->SetText(FText::FromString(AmmoText));
	}
}

void ABlasterPlayerController::SetHudCarriedAmmo(int32 Ammo)
{
	if(BlasterHud == nullptr)
	{
		BlasterHud = Cast<ABlasterHud>(GetHUD());
	}
	if(BlasterHud &&
		BlasterHud->CharacterOverlay &&
		BlasterHud->CharacterOverlay->CarriedAmmo)
	{
		const FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		BlasterHud->CharacterOverlay->CarriedAmmo->SetText(FText::FromString(AmmoText));
	}
}

void ABlasterPlayerController::SetHuDCountdownTime(float CountdownTime)
{
	if(BlasterHud == nullptr)
	{
		BlasterHud = Cast<ABlasterHud>(GetHUD());
	}
	if(BlasterHud &&
		BlasterHud->CharacterOverlay &&
		BlasterHud->CharacterOverlay->CDText)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime/60.f);
		int32 Seconds = CountdownTime - Minutes * 60.f;
		
		FString CountdownText = FString::Printf(TEXT("%02d : %02d"), Minutes, Seconds);
		BlasterHud->CharacterOverlay->CDText->SetText(FText::FromString(CountdownText));
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


void ABlasterPlayerController::SetHudTime()
{
	const uint32 SecondsLeft = FMath::CeilToInt(MatchTime - GetServerTime());

	if(CountDownInt != SecondsLeft)
	{
		SetHuDCountdownTime(MatchTime - GetServerTime());
	}
	
	CountDownInt = SecondsLeft;
	
}

void ABlasterPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if(IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		Server_RequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void ABlasterPlayerController::Server_RequestServerTime_Implementation(float TimeOfClientRequest)
{
	const float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	Client_ReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ABlasterPlayerController::Client_ReportServerTime_Implementation(float TimeOfClientRequest,
	float TimeServerRecievedRequest)
{
	const float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	const float CurrentServerTime = TimeServerRecievedRequest + (.5f * RoundTripTime );
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();

}

float ABlasterPlayerController::GetServerTime()
{
	if(HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	else return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}
