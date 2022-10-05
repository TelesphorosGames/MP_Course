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

	virtual void Destroyed() override;


protected:
	
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void BeginPlay() override;

	
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TrailSystem;
	UPROPERTY()
	class UNiagaraComponent* TrailSystemComponent;
	UFUNCTION()
	void DestroyTimerFinished();

	UPROPERTY(EditAnywhere)
	class USoundCue* RocketInAirLoop;

	UPROPERTY()
	UAudioComponent* ProjectileLoopComponent;

	UPROPERTY(EditAnywhere)
	USoundAttenuation* LoopingSoundAttenuation;
	
private:

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* RocketMesh;

	FTimerHandle DestroyHandle;
	UPROPERTY(EditAnywhere)
	float DestroyTime = 3.f;

	


};
