// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"

#include "BlasterCharacter.h"
#include "BuffComponent.h"
#include "NiagaraFunctionLibrary.h"


AHealthPickup::AHealthPickup()
{
	bReplicates = true;

}

void AHealthPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if(OtherActor)
	{
		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
		if(BlasterCharacter)
		{
			UBuffComponent* Buff = BlasterCharacter->GetBuffComponent();
			if(Buff)
			{
				Buff->Heal(HealthAmount, HealingTime);
			}
		}
		Destroy();
	}
}

