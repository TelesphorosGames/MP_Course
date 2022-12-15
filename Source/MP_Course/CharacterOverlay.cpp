// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterOverlay.h"

#include "BlasterPlayerController.h"
#include "ElimAnnouncement.h"
#include "Components/EditableTextBox.h"



void UCharacterOverlay::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	
}

void UCharacterOverlay::ToggleChatBox()
{
	
	ABlasterPlayerController* BlasterPlayerController = Cast<ABlasterPlayerController>(GetOwningPlayer());
	if(BlasterPlayerController)
	{
		if(!bChatBoxVisible)
        	{

				ChatInputTextBox->OnTextCommitted.AddDynamic(this, &UCharacterOverlay::OnTextComitted);
        		ChatInputTextBox->SetVisibility(ESlateVisibility::Visible);
        		ChatInputTextBox->SetFocus();
        		BlasterPlayerController->SetInputMode(FInputModeGameAndUI());
        		bChatBoxVisible=true;
			
        	}
        	else
        	{
        		ChatInputTextBox->OnTextCommitted.RemoveDynamic(this, &UCharacterOverlay::OnTextComitted);
	
        		ChatInputTextBox->SetVisibility(ESlateVisibility::Collapsed);
        		BlasterPlayerController->SetInputMode(FInputModeGameOnly());
        		bChatBoxVisible = false;
        	}
	}
	
}

void UCharacterOverlay::OnTextComitted(const FText& Text, ETextCommit::Type CommitMethod)
{

	ABlasterPlayerController* BlasterPlayerController = Cast<ABlasterPlayerController>(GetOwningPlayer());

	if(CommitMethod == ETextCommit::OnEnter)
	{
		if(BlasterPlayerController)
		{
			BlasterPlayerController->Server_BroadcastMessage(GetOwningPlayerState(), Text.ToString());
			ChatInputTextBox->SetVisibility(ESlateVisibility::Collapsed);
			BlasterPlayerController->SetInputMode(FInputModeGameOnly());
			bChatBoxVisible = false;
			ChatInputTextBox->SetText(FText::FromString(""));
			ChatInputTextBox->OnTextCommitted.RemoveDynamic(this, &UCharacterOverlay::OnTextComitted);
		}
	}
	else
    {
     	ChatInputTextBox->SetVisibility(ESlateVisibility::Collapsed);
     	BlasterPlayerController->SetInputMode(FInputModeGameOnly());
     	bChatBoxVisible = false;
		ChatInputTextBox->OnTextCommitted.RemoveDynamic(this, &UCharacterOverlay::OnTextComitted);
    }
	
}