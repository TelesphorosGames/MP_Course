#include "BlasterHud.h"

#include "Announcement.h"
#include "BlasterPlayerController.h"
#include "CharacterOverlay.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameMode.h"


void ABlasterHud::DrawHUD()
{
	Super::DrawHUD();

	if(OwningPlayerController && OwningPlayerController->GetBlasterMatchState() != MatchState::InProgress) return;
	
	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		

		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		const float BaseSpread= 45.f;
		const float SpreadScaled = (CrosshairSpreadMax * HudPackage.CrosshairSpread) + BaseSpread;
		
		if(HudPackage.CrosshairsCenter != nullptr)
		{
			const FVector2D Spread{0.f, 0.f};
			DrawCrosshairs(HudPackage.CrosshairsCenter, ViewportCenter, Spread, HudPackage.CrosshairsColor);
			
		}
		if(HudPackage.CrosshairsLeft != nullptr)
		{
			const FVector2D Spread{-SpreadScaled,0.f};
			DrawCrosshairs(HudPackage.CrosshairsLeft, ViewportCenter, Spread, HudPackage.CrosshairsColor);
		}
		if(HudPackage.CrosshairsRight != nullptr)
		{
			FVector2D Spread{SpreadScaled, 0.f};
			DrawCrosshairs(HudPackage.CrosshairsRight, ViewportCenter, Spread, HudPackage.CrosshairsColor);
		}
		if(HudPackage.CrosshairsTop != nullptr)
		{
			FVector2D Spread{0.f, -SpreadScaled};
			DrawCrosshairs(HudPackage.CrosshairsTop, ViewportCenter, Spread, HudPackage.CrosshairsColor);
		}
		if(HudPackage.CrosshairsBottom != nullptr)
		{
			FVector2D Spread{0.f, SpreadScaled};
			DrawCrosshairs(HudPackage.CrosshairsBottom, ViewportCenter, Spread, HudPackage.CrosshairsColor);
		}
	}
}

void ABlasterHud::BeginPlay()
{
	Super::BeginPlay();
	OwningPlayerController = Cast<ABlasterPlayerController>(GetOwningPlayerController());
	UE_LOG(LogTemp,Warning,TEXT("BEING CALLED YES YES"));
}

void ABlasterHud::AddAnnouncement()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if(PlayerController && AnnouncementClass && AnnouncementWidget == nullptr)
	{
		AnnouncementWidget = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
		AnnouncementWidget->AddToViewport();
	}
	else if (PlayerController && AnnouncementClass && AnnouncementWidget)
	{
		AnnouncementWidget->SetVisibility(ESlateVisibility::Visible);
	}
	
}

void ABlasterHud::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if(PlayerController && CharacterOverlayClass && CharacterOverlay == nullptr)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
	else
	{
		CharacterOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}

void ABlasterHud::RemoveAnnouncement()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if(PlayerController && AnnouncementWidget != nullptr)
		{
			AnnouncementWidget->SetVisibility(ESlateVisibility::Hidden);
		}
}

void ABlasterHud::RemoveCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if(PlayerController && CharacterOverlayClass && CharacterOverlay != nullptr)
	{
		CharacterOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABlasterHud::DrawCrosshairs(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,	ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y);
	DrawTexture(Texture, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1.f,1.f, CrosshairColor);	
}
