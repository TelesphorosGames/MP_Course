// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterHud.h"
#include "Components/ActorComponent.h"
#include "WeaponTypes.h"
#include "CombatState.h"


#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MP_COURSE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UCombatComponent();

	friend class ABlasterCharacter;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(class AWeapon* WeaponToEquip);
	void ReloadWeapon();
	
protected:

	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();
	void Fire();

	UFUNCTION(Server,Reliable)
	void Server_Reload();

	void HandleReload();

	void FireButtonPressed(bool bPressed);
	
	UFUNCTION(Server, Reliable)
	void Server_Fire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Fire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHudCrosshairs(float DeltaTime);
	

private:

	UPROPERTY()
	ABlasterCharacter* Character{};
	UPROPERTY()
	class ABlasterPlayerController* BC_Controller{};
	UPROPERTY()
	class ABlasterHud* BC_Hud{};

	UPROPERTY(ReplicatedUsing= OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;
	
	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere)
	float AimingWalkSpeed;

	bool bFireButtonPressed;


	// Hud and Crosshairs
	
	FHudPackage HUDPackage;
	float CrosshairVelocityFactor{};
	float CrosshairAimingFactor{};
	float CrosshairFallingFactor{};
	float CrosshairCrouchingFactor{};
	float CrosshairShootingFactor{};
	float CrosshairMovingFactor{};
	float CrosshairTargetFactor{};

	FVector HitTargetImpactPoint{};

	// Aiming and FOV

	// Base camera FOV
	float DefaultFOV{};

	float CurrentFOV{};
	
	// varies per weapon
	UPROPERTY(EditAnywhere)
	float ZoomedFOV{30.f};
	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed{20.f};

	void InterpFOV(float DeltaTime);

	bool bCanFire = true;
	FTimerHandle AutoFireHandle;
	
	void FireTimerFinished();
	void StartFireTimer();

	bool CanFire();
	UPROPERTY(ReplicatedUsing=OnRep_CarriedAmmo)
	int32 CarriedAmmo;
	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere)
	int32 StartingAmmo = 30;
	
	void InitializeCarriedAmmo();

	UPROPERTY(ReplicatedUsing=OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;
	
	UFUNCTION()
	void OnRep_CombatState();
};
