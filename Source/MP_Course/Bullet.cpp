// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

#include "BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"

void ABullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{

	// ABlasterCharacter* OwnerMan = Cast<ABlasterCharacter>(GetOwner());
	// if(OwnerMan)
	// {
	// 	AController* OController = OwnerMan->Controller;
	// 	if(OController)
	// 	{
	// if(OtherActor == GetOwner() || Hit.Distance < 10.f) return;
	if(OtherActor)
		UGameplayStatics::ApplyDamage(OtherActor, Damage, GetOwner()->GetInstigatorController(), this, UDamageType::StaticClass());
	// // 	}
	// // }

	
	
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	
	
	
	
}
