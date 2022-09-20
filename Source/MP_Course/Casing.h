// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
class MP_COURSE_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	

	ACasing();
	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CasingMesh;
	UPROPERTY(VisibleAnywhere)
	float CasingVelocity;
UPROPERTY(EditAnywhere)
	class USoundCue* ShellSound;

	bool bSoundPlayed = false;

};
