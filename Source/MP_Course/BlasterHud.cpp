#include "BlasterHud.h"


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
			DrawCrosshairs(HudPackage.CrosshairsCenter, ViewportCenter, Spread);
			
		}
		if(HudPackage.CrosshairsLeft)
		{
			const FVector2D Spread{-SpreadScaled,0.f};
			DrawCrosshairs(HudPackage.CrosshairsLeft, ViewportCenter, Spread);
		}
		if(HudPackage.CrosshairsRight)
		{
			FVector2D Spread{SpreadScaled, 0.f};
			DrawCrosshairs(HudPackage.CrosshairsRight, ViewportCenter, Spread);
		}
		if(HudPackage.CrosshairsTop)
		{
			FVector2D Spread{0.f, -SpreadScaled};
			DrawCrosshairs(HudPackage.CrosshairsTop, ViewportCenter, Spread);
		}
		if(HudPackage.CrosshairsBottom)
		{
			FVector2D Spread{0.f, SpreadScaled};
			DrawCrosshairs(HudPackage.CrosshairsBottom, ViewportCenter, Spread);
		}
	}
	
}

void ABlasterHud::DrawCrosshairs(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,	ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y);

	DrawTexture(Texture, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1.f,1.f);
	
}
