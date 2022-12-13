// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Bullet.generated.h"

/**
 * 
 */
UCLASS()
class MP_COURSE_API ABullet : public AProjectile
{
	GENERATED_BODY()

public:

	ABullet();


#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	
protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void Multicast_OnHit(AActor* OtherActor, float DamageToCause) override;

	virtual void BeginPlay() override;
	
private:
	
};
