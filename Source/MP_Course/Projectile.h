// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class MP_COURSE_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	

	/* Be sure to construct your projectile movement component in any classes that are derived from AProjectile!!!*/
	AProjectile();

	FORCEINLINE UStaticMeshComponent* GetProjectileMesh() const {return ProjectileMesh ;}

	
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

	UPROPERTY()
	bool bUseServerSideRewind = false;
	
	FVector_NetQuantize TraceStart;
	FVector_NetQuantize100 InitialVelocity;
	
	UPROPERTY(EditAnywhere)
	float BulletSpeed = 15000.f;

	/* ONLY SET FOR GRENADES AND ROCKETS - BULLET VALUE IS SET WHEN BULLET IS SPAWNED*/
	UPROPERTY(EditAnywhere)
	float Damage = {20.f};
	UPROPERTY(EditAnywhere)
	float HeadshotDamage = {100.f};

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_OnHit(ECollisionChannel CollisionChannel);
	virtual void Multicast_OnHit(AActor* OtherActor, float DamageToCause);

	void ExplodeDamage();
	

	UPROPERTY(EditAnywhere)
	float DamageInnerRadius = 200.f;
	UPROPERTY(EditAnywhere)
	float DamageOuterRadius = 500.f;
	
	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere)
	UParticleSystem* HitPlayerParticles;
	
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;
	
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TrailSystem;
	
	UPROPERTY()
	class UNiagaraComponent* TrailSystemComponent;
	
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;
	
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* ProjectileMesh;

	UFUNCTION()
	void SpawnTrailSystem();

	void StartDestroyTimer();
			
	FTimerHandle DestroyHandle;
	UPROPERTY(EditAnywhere)
	float DestroyTime = 3.f;
	UFUNCTION()
	void DestroyTimerFinished();
	

	
private:
	

	UPROPERTY(EditAnywhere)
	class UParticleSystem* Tracer;

	UPROPERTY(EditAnywhere)
	class UParticleSystemComponent* TracerComponent;

	
};
