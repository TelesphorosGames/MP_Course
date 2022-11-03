// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "AccuracyPickup.generated.h"

/**
 * 
 */
UCLASS()
class MP_COURSE_API AAccuracyPickup : public APickup
{
	GENERATED_BODY()


public:

	
	
protected:

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
private:

	UPROPERTY(EditAnywhere, Category= "Weapon Scatter")
	float ScatterRadiusSubtractAmount = 50.f;

	UPROPERTY(EditAnywhere, Category= "Weapon Scatter")
	float AccuracyBuffTime = 10.f;
};
