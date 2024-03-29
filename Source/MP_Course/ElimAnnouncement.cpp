// Fill out your copyright notice in the Description page of Project Settings.


#include "ElimAnnouncement.h"

#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UElimAnnouncement::SetElimAnnounementText(FString AttackerName, FString VictimName)
{
	const FString ElimAnnouncementText = FString::Printf(TEXT("%s Eliminated %s!"), *AttackerName, *VictimName);
	if(AnnouncementText)
	{
		AnnouncementText->SetText(FText::FromString(ElimAnnouncementText));
	}
}

void UElimAnnouncement::SetPlayerChatText(const FString& SenderName, const FString& MessageToDisplay)
{
	const FString ChatAnnouncementText = FString::Printf(TEXT("%s : %s"), *SenderName, *MessageToDisplay );
	if(AnnouncementText)
	{
		AnnouncementText->SetText(FText::FromString(ChatAnnouncementText));
	}
}
