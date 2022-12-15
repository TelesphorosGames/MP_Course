// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ElimAnnouncement.generated.h"

/**
 * 
 */
UCLASS()
class MP_COURSE_API UElimAnnouncement : public UUserWidget
{
	GENERATED_BODY()


public:

	
	void SetElimAnnounementText(FString AttackerName, FString VictimName);
	void SetPlayerChatText(APlayerState* Sender, const FString& MessageToDisplay);
	
	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* AnnouncementBox{};

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* AnnouncementText{};

	
};
