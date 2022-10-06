// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

#include "BlasterCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ABullet::ABullet()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity=true;
	bReplicates=true;
}

void ABullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                    FVector NormalImpulse, const FHitResult& Hit)
{

	// ABlasterCharacter* OwnerMan = Cast<ABlasterCharacter>(GetOwner());
	// if(OwnerMan)
	// {
	// 	AController* OController = OwnerMan->Controller;
	// 	if(OController)

	if(OtherActor)
	{
		Multicast_OnHit(OtherActor);
	}
	
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	
	
	
	
}

void ABullet::Multicast_OnHit(AActor* OtherActor)
{
	if(OtherActor)
		UGameplayStatics::ApplyDamage(OtherActor, Damage, GetOwner()->GetInstigatorController(), this, UDamageType::StaticClass());
	
}
