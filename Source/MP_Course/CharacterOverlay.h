// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

class UEditableTextBox;
/**
 * 
 */
UCLASS()
class MP_COURSE_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta=(BindWidget))
	class UProgressBar* HealthBar;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* HealthText;
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* ShieldBar;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ShieldText;

	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* ScoreAmount;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* DefeatsAmount;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* ElimText;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* AmmoCount;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* CarriedAmmo{};
	UPROPERTY(meta=(BindWidget))
	UTextBlock* CDText;
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* UrgentText;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* GrenadesCount;
	UPROPERTY(meta=(BindWidget))
	class UImage* HighPingImage;
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* HighPingAnimation;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* PingNumberText;
	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* ChatInputTextBox;

	UPROPERTY()
	bool bChatBoxVisible = false;

	UFUNCTION()
	void OnTextComitted(const FText& Text, ETextCommit::Type CommitMethod);

	void ToggleChatBox();



protected:

	virtual void NativeOnInitialized() override;

	
};
