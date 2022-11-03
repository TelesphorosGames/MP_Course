// Fill out your copyright notice in the Description page of Project Settings.


#include "AccuracyPickup.h"

#include "BlasterCharacter.h"
#include "BuffComponent.h"
#include "CombatComponent.h"
#include "Weapon.h"

void AAccuracyPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if(OtherActor)
	{
		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
		if(BlasterCharacter)
		{
			UBuffComponent* BuffComponent = BlasterCharacter->GetBuffComponent();
			if(BuffComponent)
			{
				BuffComponent->BuffAccuracy(
					BlasterCharacter->GetEquippedWeapon()->GetScatterSphereRadius(),
					ScatterRadiusSubtractAmount,
					AccuracyBuffTime);
			}
		}
		Destroy();
	}
}
