#include "BlasterHud.h"

#include "Announcement.h"
#include "BlasterPlayerController.h"
#include "CharacterOverlay.h"
#include "ElimAnnouncement.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
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

void ABlasterHud::AddElimAnnouncement(FString Attacker, FString Victim)
{
	if(OwningPlayerController && ElimAnnouncementClass)
	{
		UElimAnnouncement* ElimAnnouncementWidget = CreateWidget<UElimAnnouncement>(OwningPlayerController, ElimAnnouncementClass);
		if(ElimAnnouncementWidget)
		{
			
			ElimAnnouncementWidget->SetElimAnnounementText(Attacker, Victim);
			ElimAnnouncementWidget->AddToViewport();
			
			for(auto Msg : Announcements)
			{
				if(Msg && Msg->AnnouncementBox)
				{
					UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Msg->AnnouncementBox);
					if(CanvasSlot)
					{
						const FVector2D Position = CanvasSlot->GetPosition();
						const FVector2D NewPosition =  { Position.X, Position.Y - CanvasSlot->GetSize().Y};
						CanvasSlot->SetPosition(NewPosition);
					}
				}
			}
			
			Announcements.Add(ElimAnnouncementWidget);

			FTimerHandle ElimMsgTimer;
			FTimerDelegate ElimTimerDelegate;

			ElimTimerDelegate.BindUFunction(this, FName("ElimAnnouncementTimerFinished"), ElimAnnouncementWidget);
			GetWorldTimerManager().SetTimer(ElimMsgTimer, ElimTimerDelegate, ElimAnnounementTime, false);
		}
		
	}
}

void ABlasterHud::AddChatAnnouncement(const FString& SenderName, const FString& ChatMessage)
{
	if(OwningPlayerController && ElimAnnouncementClass)
	{
		UElimAnnouncement* ChatAnnouncementWidget = CreateWidget<UElimAnnouncement>(OwningPlayerController, ElimAnnouncementClass);
		if(ChatAnnouncementWidget)
		{
			
			ChatAnnouncementWidget->SetPlayerChatText(SenderName, ChatMessage);
			ChatAnnouncementWidget->AddToViewport();
			
			for(auto Msg : Announcements)
			{
				if(Msg && Msg->AnnouncementBox)
				{
					UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Msg->AnnouncementBox);
					if(CanvasSlot)
					{
						const FVector2D Position = CanvasSlot->GetPosition();
						const FVector2D NewPosition =  { Position.X, Position.Y - CanvasSlot->GetSize().Y};
						CanvasSlot->SetPosition(NewPosition);
					}
				}
			}
			
			Announcements.Add(ChatAnnouncementWidget);

			FTimerHandle ElimMsgTimer;
			FTimerDelegate ElimTimerDelegate;

			ElimTimerDelegate.BindUFunction(this, FName("ElimAnnouncementTimerFinished"), ChatAnnouncementWidget);
			GetWorldTimerManager().SetTimer(ElimMsgTimer, ElimTimerDelegate, ElimAnnounementTime, false);
		}
		
	}
}

void ABlasterHud::DrawCrosshairs(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,	ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y);
	DrawTexture(Texture, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1.f,1.f, CrosshairColor);	
}

void ABlasterHud::ElimAnnouncementTimerFinished(UElimAnnouncement* MsgToRemove)
{
	if(MsgToRemove)
	{
		MsgToRemove->RemoveFromParent();
		
	}
}
