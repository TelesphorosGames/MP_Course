// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileWeapon.h"
#include "ProjectileRocket.generated.h"

/**
 * 
 */
UCLASS()
class MP_COURSE_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()

public:

	AProjectileRocket();


protected:
	
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;


	
private:

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* RocketMesh;


};
