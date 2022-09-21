// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHud.generated.h"

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
	
};

UCLASS()
class MP_COURSE_API ABlasterHud : public AHUD
{
	GENERATED_BODY()

public:

	virtual void DrawHUD() override;


	FORCEINLINE void SetHudPackage(const FHudPackage& Package) {HudPackage = Package ; }
	
private:

	FHudPackage HudPackage{};

	void DrawCrosshairs(UTexture2D* Texture, FVector2D ViewportCenter);
	
};
