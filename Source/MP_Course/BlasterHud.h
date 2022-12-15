// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHud.generated.h"


class ABlasterPlayerController;
USTRUCT(BlueprintType)
struct FHudPackage
{
	GENERATED_BODY()
public:

	UPROPERTY()
	class UTexture2D* CrosshairsCenter;
	UPROPERTY()
	class UTexture2D* CrosshairsLeft;
	UPROPERTY()
	class UTexture2D* CrosshairsRight;
	UPROPERTY()
	class UTexture2D* CrosshairsTop;
	UPROPERTY()
	class UTexture2D* CrosshairsBottom;
	
	float CrosshairSpread;
	FLinearColor CrosshairsColor;
	
};

UCLASS()
class MP_COURSE_API ABlasterHud : public AHUD
{
	GENERATED_BODY()

public:

	virtual void DrawHUD() override;


	FORCEINLINE void SetHudPackage(const FHudPackage& Package) {HudPackage = Package ; }

	UPROPERTY(EditAnywhere,Category=Stats)
	TSubclassOf<class UUserWidget> CharacterOverlayClass;
	
	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay{};

	UPROPERTY(EditAnywhere,Category=Stats)
	TSubclassOf<class UUserWidget> AnnouncementClass;
	
	UPROPERTY()
	class UAnnouncement* AnnouncementWidget{};
	UFUNCTION()
	void AddAnnouncement();
	UFUNCTION()
	void AddCharacterOverlay();
	UFUNCTION()
	void RemoveAnnouncement();
	UFUNCTION()
	void RemoveCharacterOverlay();
	
	void AddElimAnnouncement(FString Attacker, FString Victim);

	void AddChatAnnouncement(const FString& SenderName, const FString& ChatMessage);
	
protected:

	virtual void BeginPlay() override;;

private:
	
	FHudPackage HudPackage{};
	UFUNCTION()
	void DrawCrosshairs(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor);
	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;
	UPROPERTY()
	ABlasterPlayerController* OwningPlayerController{};

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UElimAnnouncement> ElimAnnouncementClass;

	UPROPERTY(EditAnywhere)
	float ElimAnnounementTime = 7.5f;

	UFUNCTION()
	void ElimAnnouncementTimerFinished(UElimAnnouncement* MsgToRemove);

	UPROPERTY()
	TArray<UElimAnnouncement*> Announcements;


	
};
