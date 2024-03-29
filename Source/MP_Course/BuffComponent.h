// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


class ABlasterCharacter;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MP_COURSE_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UBuffComponent();
	friend class ABlasterCharacter;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Heal(int32 HealAmount, float HealTime);
	void ReplenishShield(float ShieldAmount, float ReplenishTime);
	void BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime);
	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);

	void BuffAccuracy(float BaseAccuracy, float BuffAccuracy, float BuffTime);
		
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void HealRampUp(float DeltaTime);
	void ShieldRampUp(float DeltaTime);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float BaseSpeed, float CrouchSpeed);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAccuracyBuff(float InBuffAccuracy);
	
private:

	UPROPERTY()
	ABlasterCharacter* Character;

	bool bHealing = false;
	float HealingRate = 0.f;
	float AmountToHeal = 0.f;

	bool bReplenishingShield = false;
	float ReplenishRate = 0.f;
	float AmountToReplenish = 0.f;

	FTimerHandle SpeedBuffTimer;
	UFUNCTION()
	void ResetSpeed();

	float InitialBaseSpeed;
	float InitialCrouchedSpeed;

	FTimerHandle AccuracyBuffTimer;
	UFUNCTION()
	void ResetAccuracy();

	float InitialAccuracy;
	
};
