// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoPickup.h"
#include "BlasterCharacter.h"
#include "CombatComponent.h"


AAmmoPickup::AAmmoPickup()
{
	PickupMesh->SetRenderCustomDepth(true);
	PickupMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_PURPLE);
	
}

void AAmmoPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if(OtherActor)
	{
		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
		if(BlasterCharacter)
		{
			UCombatComponent* CombatComponent = BlasterCharacter->GetCombatComponent();
			if(CombatComponent)
			{
				CombatComponent->PickupAmmo(WeaponType, AmmoAmount);
			}
		}
		Destroy();
	}
	
	
	
}
