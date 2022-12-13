// Fill out your copyright notice in the Description page of Project Settings.


#include "ElimAnnouncement.h"

#include "Components/TextBlock.h"

void UElimAnnouncement::SetElimAnnounementText(FString AttackerName, FString VictimName)
{
	FString ElimAnnounementText = FString::Printf(TEXT("%s Ellimed %s!"), *AttackerName, *VictimName);
	if(AnnouncementText)
	{
		AnnouncementText->SetText(FText::FromString(ElimAnnounementText));
	}
}
