#include "BlasterHud.h"


void ABlasterHud::DrawHUD()
{
	Super::DrawHUD();
	
	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		

		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
		
		if(HudPackage.CrosshairsCenter)
		{
			DrawCrosshairs(HudPackage.CrosshairsCenter, ViewportCenter);
			
		}
		if(HudPackage.CrosshairsLeft)
		{
			DrawCrosshairs(HudPackage.CrosshairsLeft, ViewportCenter);
		}
		if(HudPackage.CrosshairsRight)
		{
			DrawCrosshairs(HudPackage.CrosshairsRight, ViewportCenter);
		}
		if(HudPackage.CrosshairsTop)
		{
			DrawCrosshairs(HudPackage.CrosshairsTop, ViewportCenter);
		}
		if(HudPackage.CrosshairsBottom)
		{
			DrawCrosshairs(HudPackage.CrosshairsBottom, ViewportCenter);
		}
	}
	
}

void ABlasterHud::DrawCrosshairs(UTexture2D* Texture, FVector2D ViewportCenter)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(ViewportCenter.X - (TextureWidth / 2.f),	ViewportCenter.Y - (TextureHeight / 2.f));

	DrawTexture(Texture, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1.f,1.f);
	
}
