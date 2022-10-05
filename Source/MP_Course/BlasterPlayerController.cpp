// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"

#include "Announcement.h"
#include "BlasterCharacter.h"
#include "BlasterHud.h"
#include "CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MP_Course.h"
#include "GameFramework/GameMode.h"
#include "Net/UnrealNetwork.h"
#include "BlasterGameMode.h"
#include "BlasterGameState.h"
#include "BlasterPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/WidgetAnimation.h"


void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	BlasterHud = Cast<ABlasterHud>(GetHUD());

	
	Server_CheckMatchState();
	
	
}

void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerController, BlasterMatchState);
}

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
	
	PollInit();

	
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
		bInitializeCharacterOverlay = true;
		HudHealth = Health;
		HudMaxHealth = MaxHealth;
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
	else
	{
		bInitializeCharacterOverlay = true;
		HudScore = Score;
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
	else
	{
		bInitializeCharacterOverlay = true;
		HudDefeats = Defeats;
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
		if(CountdownTime< 0.f)
		{
			BlasterHud->CharacterOverlay->CDText->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountdownTime/60.f);
		int32 Seconds = CountdownTime - Minutes * 60.f;
    		
		FString CountdownText = FString::Printf(TEXT("%02d : %02d"), Minutes, Seconds);
		BlasterHud->CharacterOverlay->CDText->SetText(FText::FromString(CountdownText));
		if(BlasterHud->CharacterOverlay->UrgentText && Minutes == 0 && Seconds <= 30.f)
		{
			BlasterHud->CharacterOverlay->PlayAnimation(BlasterHud->CharacterOverlay->UrgentText);
		}
	}
}

void ABlasterPlayerController::SetHUDAnnouncementCountdown(float Time)
{
		if(BlasterHud == nullptr)
    	{
    		BlasterHud = Cast<ABlasterHud>(GetHUD());
    	}
    	if(BlasterHud &&
    		BlasterHud->AnnouncementWidget &&
    		BlasterHud->AnnouncementWidget->WarmupTime)
    	{
    		if(Time< 0.f)
    		{
    			BlasterHud->AnnouncementWidget->WarmupTime->SetText(FText());
    			return;
    		}
    		int32 Minutes = FMath::FloorToInt(Time/60.f);
    		int32 Seconds = Time - Minutes * 60.f;
    		
    		FString CountdownText = FString::Printf(TEXT("%02d : %02d"), Minutes, Seconds);
    		BlasterHud->AnnouncementWidget->WarmupTime->SetText(FText::FromString(CountdownText));
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

void ABlasterPlayerController::OnMatchStateSet(FName State)
{
	BlasterMatchState = State;

	if(BlasterMatchState == MatchState::WaitingToStart)
	{
		
	}

	if(BlasterMatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if(BlasterMatchState==MatchState::CoolDown)
	{
		HandleCooldown();
	}
}

void ABlasterPlayerController::OnRep_MatchState()
{
	if(BlasterMatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if(BlasterMatchState==MatchState::CoolDown)
	{
		HandleCooldown();
	}
}


void ABlasterPlayerController::HandleMatchHasStarted()
{
	if(BlasterHud == nullptr)
	{
		BlasterHud = Cast<ABlasterHud>(GetHUD());
	}
	if(BlasterHud && BlasterHud->CharacterOverlay == nullptr)
	{
		BlasterHud->AddCharacterOverlay();
		if(BlasterHud->AnnouncementWidget)
		{
			BlasterHud->AnnouncementWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ABlasterPlayerController::HandleCooldown()
{
	if(BlasterHud == nullptr)
	{
		BlasterHud = Cast<ABlasterHud>(GetHUD());
	}
	if(BlasterHud) 
	{
		if(BlasterHud->CharacterOverlay)
		{
			BlasterHud->CharacterOverlay->RemoveFromParent();
		}
		
		if(BlasterHud->AnnouncementWidget &&
			BlasterHud->AnnouncementWidget->AnnouncementText &&
			BlasterHud->AnnouncementWidget->InfoText)
		{
			BlasterHud->AnnouncementWidget->SetVisibility(ESlateVisibility::Visible);
			const FString AnnoncementText("New Match Starts In:");
			BlasterHud->AnnouncementWidget->AnnouncementText->SetText(FText::FromString(AnnoncementText));
			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
			if(BlasterGameState)
			{
				TArray<ABlasterPlayerState*> TopPlayers = BlasterGameState->TopScoringPlayers;
				FString InfoTextString;
				if(TopPlayers.Num() == 0)
				{
					InfoTextString=FString("There is no winner. Git Gud");
				}
				else if(TopPlayers.Num() == 1 && TopPlayers[0] == GetPlayerState<ABlasterPlayerState>())
				{
					InfoTextString=FString("YOU WIN!");
				}
				else if(TopPlayers.Num()== 1)
				{
					InfoTextString = FString::Printf(TEXT("Winner : \n %s"), *TopPlayers[0]->GetPlayerName());
				}
				else if(TopPlayers.Num() > 1)
				{
					InfoTextString = FString::Printf(TEXT("TIE!"));
					for (auto&& TiedPlayer : TopPlayers)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
					}
				}
				BlasterHud->AnnouncementWidget->InfoText->SetText(FText::FromString(InfoTextString));
			}
		}
	}
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
	if(BlasterCharacter)
	{
		BlasterCharacter->SetDisableGameplay(true);
		BlasterCharacter->FireButtonPressed();
	}
}

void ABlasterPlayerController::Server_CheckMatchState_Implementation()
{
	const ABlasterGameMode* GameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	if(GameMode)
	{
		WarmupTime = GameMode->GetWarmupTime();
		MatchTime = GameMode->GetMatchTime();
		LevelStartingTime = GameMode->GetLevelStartingTime();
		CoolDownTime = GameMode->GetCooldownTime();
		BlasterMatchState = GameMode->GetMatchState();
		Client_JoinMidGame(BlasterMatchState, WarmupTime, MatchTime, LevelStartingTime, CoolDownTime);
	}
}

void ABlasterPlayerController::Client_JoinMidGame_Implementation(FName StateOfMatch, float Warmup, float Match, float StartingTime, float Cooldown)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	LevelStartingTime = StartingTime;
	BlasterMatchState = StateOfMatch;
	CoolDownTime = Cooldown;
	OnMatchStateSet(BlasterMatchState);

	if(BlasterHud == nullptr)
	{
		BlasterHud = Cast<ABlasterHud>(GetHUD());
	}
	if(BlasterHud && BlasterMatchState == MatchState::WaitingToStart)
	{
		BlasterHud->AddAnnouncement();
	}
}


void ABlasterPlayerController::SetHudTime()
{
	float TimeLeft = 0.f;
	
	if(BlasterMatchState == MatchState::WaitingToStart)
	{
		TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	}
	else if(BlasterMatchState == MatchState::InProgress)
	{
		TimeLeft = WarmupTime + MatchTime- GetServerTime() + LevelStartingTime;
	}
	else if(BlasterMatchState == MatchState::CoolDown)
	{
		TimeLeft = CoolDownTime + WarmupTime + MatchTime- GetServerTime() + LevelStartingTime;
	}
	uint32 SecondsLeft = FMath::CeilToInt(MatchTime - GetServerTime());


	if(HasAuthority())
	{
		if(BlasterGameMode == nullptr)
		{
			BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
		}
		if(BlasterGameMode)
		{
			SecondsLeft = FMath::CeilToInt(BlasterGameMode->GetCountdownTime() + LevelStartingTime);
		}
		
	}
	

	if(CountDownInt != SecondsLeft)
	{
		if(BlasterMatchState == MatchState::WaitingToStart || BlasterMatchState == MatchState::CoolDown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if(BlasterMatchState == MatchState::InProgress)
		{
			SetHuDCountdownTime(TimeLeft);
		}
	}
	CountDownInt = SecondsLeft;
}

void ABlasterPlayerController::PollInit()
{
	if(CharacterOverlay == nullptr)
	{
		if(BlasterHud && BlasterHud->CharacterOverlay)
		{
			CharacterOverlay = BlasterHud->CharacterOverlay;
			if(CharacterOverlay)
			{
				SetHudHealth(HudHealth, HudMaxHealth);
				SetHudScore(HudScore);
				SetHudDefeats(HudDefeats);
			}
		}
	}
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
