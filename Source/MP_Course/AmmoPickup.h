// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "WeaponTypes.h"


#include "AmmoPickup.generated.h"

/**
 * 
 */
UCLASS()
class MP_COURSE_API AAmmoPickup : public APickup
{
	GENERATED_BODY()


public:

AAmmoPickup();
	
protected:

virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	
private:

	UPROPERTY(EditAnywhere)
	int32 AmmoAmount = 0;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
	
};
