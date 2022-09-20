// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MP_COURSE_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()

public:

	virtual void Fire(const FVector& HitTarget) override;
	
protected:
	



	
private:


	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileClass;

	
	
};
