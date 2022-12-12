// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuReturnWidget.h"

#include "BlasterCharacter.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "MultiplayerSessionSubsystem.h"
#include "GameFramework/GameModeBase.h"


void UMenuReturnWidget::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();

	if(World)
	{
		PlayerController = World->GetFirstPlayerController();
		if(PlayerController)
		{
			FInputModeGameAndUI InputModeGameAndUI;
			PlayerController->SetInputMode(InputModeGameAndUI);
			PlayerController->SetShowMouseCursor(true);
			InputModeGameAndUI.SetWidgetToFocus(TakeWidget());
			
		}
	}
	if(ReturnButton && !ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.AddDynamic(this, &UMenuReturnWidget::ReturnButtonClicked);
	}
	UGameInstance* GameInstance = GetGameInstance();
	if(GameInstance)
	{
		MultiplayerSessionSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionSubsystem>();
		if(MultiplayerSessionSubsystem)
		{
			MultiplayerSessionSubsystem->MultiplayerSubsessionOnDestroySessionComplete.AddDynamic(this, &UMenuReturnWidget::OnDestroySession);
		}

	}

	
}

void UMenuReturnWidget::MenuTeardown()
{
	RemoveFromParent();

	const UWorld* World = GetWorld();

	if(World)
	{
		if(PlayerController == nullptr)
		{
			PlayerController = World->GetFirstPlayerController();
		}

		if(PlayerController)
		{
			const FInputModeGameOnly GameOnly ;
			PlayerController->SetInputMode(GameOnly);
			PlayerController->SetShowMouseCursor(false);
			
		}
	}
	if(ReturnButton && ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.RemoveDynamic(this, &UMenuReturnWidget::ReturnButtonClicked);
	}
	if(MultiplayerSessionSubsystem && MultiplayerSessionSubsystem->MultiplayerSubsessionOnDestroySessionComplete.IsBound())
	{
		MultiplayerSessionSubsystem->MultiplayerSubsessionOnDestroySessionComplete.RemoveDynamic(this, &UMenuReturnWidget::OnDestroySession);
	}
}

bool UMenuReturnWidget::Initialize()
{
	if(!Super::Initialize())
	{
		return false;
	}
	
	return true;
}

void UMenuReturnWidget::OnDestroySession(bool bWasSuccessful)
{
	if(!bWasSuccessful)
	{
		SetIsEnabled(true);
		return;
	}

	UWorld* World = GetWorld();
	if(World)
	{
		AGameModeBase* GameMode = World->GetAuthGameMode<AGameModeBase>();
		if(GameMode)
		{
			GameMode->ReturnToMainMenuHost();
		}
		else
		{
			if(PlayerController)
			{
				PlayerController->ClientReturnToMainMenuWithTextReason(FText());
			}
		}
	
	}
}

void UMenuReturnWidget::OnPlayerLeftGame()
{
	if(MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->DestroySession();
	}
}

void UMenuReturnWidget::ReturnButtonClicked()
{
	ReturnButton->SetIsEnabled(false);

	UWorld* World = GetWorld();
	if(World)
	{
		APlayerController* FirstPlayerController = World->GetFirstPlayerController();
		if(FirstPlayerController)
		{
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FirstPlayerController->GetPawn());
			if(BlasterCharacter)
			{
				BlasterCharacter->OnLeftGame.AddDynamic(this, &UMenuReturnWidget::OnPlayerLeftGame);
				BlasterCharacter->Server_LeaveGame();
				
				
			}
			else
			{
				ReturnButton->SetIsEnabled(true);
			}
		}
	}
}
