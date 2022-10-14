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
#include "CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/WidgetAnimation.h"


void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	BlasterHud = Cast<ABlasterHud>(GetHUD());
	
	BlasterPlayerController = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());

	
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
	if(BlasterHud == nullptr)
	{
		BlasterHud = Cast<ABlasterHud>(GetHUD());
	}
	if(IsLocalController())
	{
		Server_RequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ABlasterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	PollInit();
	
	CheckTimeSync(DeltaSeconds);
	
	SetHudTime();
	
	
	
	

	
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

void ABlasterPlayerController::SetHudGrenades(int32 Grenades)
{
	if(BlasterHud == nullptr)
	{
		BlasterHud = Cast<ABlasterHud>(GetHUD());
	}
	if(BlasterHud &&
		BlasterHud->CharacterOverlay &&
		BlasterHud->CharacterOverlay->GrenadesCount)
	{
		const FString GrenadesText = FString::Printf(TEXT("%d"), Grenades);
		BlasterHud->CharacterOverlay->GrenadesCount->SetText(FText::FromString(GrenadesText));
	}
	else
	{
		HudGrenades = Grenades;
	}
	
}

void ABlasterPlayerController::SetHUDAnnouncementCountdown(float CountDownTime)
{
		if(BlasterHud == nullptr)
    	{
    		BlasterHud = Cast<ABlasterHud>(GetHUD());
    	}
    	if(BlasterHud &&
    		BlasterHud->AnnouncementWidget &&
    		BlasterHud->AnnouncementWidget->WarmupTime)
    	{
    		if(CountDownTime <= 0.f)
    		{
    			BlasterHud->AnnouncementWidget->WarmupTime->SetText(FText());
    			BlasterHud->RemoveAnnouncement();
    			return;
    		}
    		int32 Minutes = FMath::FloorToInt(CountDownTime/60.f);
    		int32 Seconds = CountDownTime - Minutes * 60.f;
    		
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
		RemoveOverlay();
	}

	if(BlasterMatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if(BlasterMatchState==MatchState::WaitingPostMatch)
	{
		HandleCooldown();
		if(!GetWorldTimerManager().IsTimerActive(RemoveOverlayTimer))
		{
			GetWorldTimerManager().SetTimer(RemoveOverlayTimer, this, &ABlasterPlayerController::RemoveOverlay, BlasterGameMode->GetCooldownTime());
		}
		
	}
}

void ABlasterPlayerController::RemoveOverlay()
{
	if(BlasterPlayerController == nullptr)
	{
		BlasterPlayerController = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	}
	if(BlasterPlayerController)
	{
		BlasterGameMode->RemoveCharacterOverlay(BlasterPlayerController);
	}
	if(BlasterHud)
	{
		BlasterHud->RemoveAnnouncement();
	}
	
	
}

void ABlasterPlayerController::OnRep_MatchState()
{
	if(BlasterMatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if(BlasterMatchState==MatchState::WaitingPostMatch)
	{
		HandleCooldown();

		if(!GetWorldTimerManager().IsTimerActive(RemoveOverlayTimer))
		{
			GetWorldTimerManager().SetTimer(RemoveOverlayTimer, this, &ABlasterPlayerController::RemoveOverlay, BlasterGameMode->GetCooldownTime());
		}
	}
}

void ABlasterPlayerController::HandleMatchHasStarted()
{
	if(BlasterHud == nullptr)
	{
		BlasterHud = Cast<ABlasterHud>(GetHUD());
	}
	if(BlasterHud)
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
					for (auto& TiedPlayer : TopPlayers)
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
	if(BlasterGameMode == nullptr)
	{
		BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	}
	if(BlasterGameMode)
	{
		WarmupTime = BlasterGameMode->GetWarmupTime();
		MatchTime = BlasterGameMode->GetMatchTime();
		LevelStartingTime = BlasterGameMode->GetLevelStartingTime();
		CoolDownTime = BlasterGameMode->GetCooldownTime();
		BlasterMatchState = BlasterGameMode->GetMatchState();
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

	if (BlasterGameMode == nullptr)
	{
		BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
		if(BlasterGameMode)
		{
			LevelStartingTime = BlasterGameMode->GetLevelStartingTime();
		}
		
	}
	
	float TimeLeft = 0.f;
	
	if(BlasterMatchState == MatchState::WaitingToStart)
	{
		TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	}
	if(BlasterMatchState == MatchState::InProgress)
	{
		TimeLeft = WarmupTime + MatchTime- GetServerTime() + LevelStartingTime;
	}
	if(BlasterMatchState == MatchState::WaitingPostMatch)
	{
		TimeLeft = CoolDownTime + WarmupTime + MatchTime- GetServerTime() + LevelStartingTime;
	}

	
	uint32 SecondsLeft = FMath::CeilToInt(MatchTime - GetServerTime());


	if(HasAuthority())
	{
		if(BlasterGameMode)
		{
			SecondsLeft = FMath::CeilToInt(BlasterGameMode->GetCountdownTime() + LevelStartingTime);
		}
	}
	

	if(CountDownInt != SecondsLeft)
	{
if(BlasterMatchState == MatchState::InProgress)
		{
			SetHuDCountdownTime(TimeLeft);
		}
		
		if(BlasterMatchState == MatchState::WaitingToStart || BlasterMatchState == MatchState::WaitingPostMatch)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}		
		
	}
	CountDownInt = SecondsLeft;
}

void ABlasterPlayerController::PollInit()
{
		if(BlasterHud && BlasterHud->CharacterOverlay)
		{
			CharacterOverlay = BlasterHud->CharacterOverlay;
			if(CharacterOverlay)
			{
				SetHudHealth(HudHealth, HudMaxHealth);
				SetHudScore(HudScore);
				SetHudDefeats(HudDefeats);
				ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
				if(BlasterCharacter && BlasterCharacter->GetCombatComponent())
				{
					SetHudGrenades(BlasterCharacter->GetCombatComponent()->GetGrenades());
				}
				SetHudGrenades(HudGrenades);
			}
		}
	if(BlasterHud && BlasterMatchState == MatchState::WaitingToStart)
	{
		BlasterHud->AddAnnouncement();
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
	const float CurrentServerTime = TimeServerRecievedRequest - (.5f * RoundTripTime );
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float ABlasterPlayerController::GetServerTime()
{
	if(HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
		return GetWorld()->GetTimeSeconds() + ClientServerDelta;
	
	
}
