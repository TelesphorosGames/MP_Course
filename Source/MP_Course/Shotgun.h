// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitScanWeapon.h"
#include "Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class MP_COURSE_API AShotgun : public AHitScanWeapon
{
	GENERATED_BODY()

public:

	void ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets);
	virtual void FireShotgun(const TArray<FVector_NetQuantize>& HitTargets);

	
private:
	
	UPROPERTY(EditAnywhere)
	uint32 NumberOfPellets = 10;

	
};
