#include "BlasterHud.h"

#include "Announcement.h"
#include "CharacterOverlay.h"
#include "Blueprint/UserWidget.h"


void ABlasterHud::DrawHUD()
{
	Super::DrawHUD();
	
	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		

		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		const float BaseSpread= 45.f;
		const float SpreadScaled = (CrosshairSpreadMax * HudPackage.CrosshairSpread) + BaseSpread;
		
		if(HudPackage.CrosshairsCenter)
		{
			const FVector2D Spread{0.f, 0.f};
			DrawCrosshairs(HudPackage.CrosshairsCenter, ViewportCenter, Spread, HudPackage.CrosshairsColor);
			
		}
		if(HudPackage.CrosshairsLeft)
		{
			const FVector2D Spread{-SpreadScaled,0.f};
			DrawCrosshairs(HudPackage.CrosshairsLeft, ViewportCenter, Spread, HudPackage.CrosshairsColor);
		}
		if(HudPackage.CrosshairsRight)
		{
			FVector2D Spread{SpreadScaled, 0.f};
			DrawCrosshairs(HudPackage.CrosshairsRight, ViewportCenter, Spread, HudPackage.CrosshairsColor);
		}
		if(HudPackage.CrosshairsTop)
		{
			FVector2D Spread{0.f, -SpreadScaled};
			DrawCrosshairs(HudPackage.CrosshairsTop, ViewportCenter, Spread, HudPackage.CrosshairsColor);
		}
		if(HudPackage.CrosshairsBottom)
		{
			FVector2D Spread{0.f, SpreadScaled};
			DrawCrosshairs(HudPackage.CrosshairsBottom, ViewportCenter, Spread, HudPackage.CrosshairsColor);
		}
	}
	
}

void ABlasterHud::BeginPlay()
{
	Super::BeginPlay();

}

void ABlasterHud::AddAnnouncement()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if(PlayerController && AnnouncementClass)
	{
		AnnouncementWidget = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
		AnnouncementWidget->AddToViewport();
	}
}

void ABlasterHud::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if(PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
		
	}

}

void ABlasterHud::DrawCrosshairs(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,	ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y);

	DrawTexture(Texture, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1.f,1.f, CrosshairColor);
	
}
