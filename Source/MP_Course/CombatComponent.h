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
	const USkeletalMeshSocket* GetEquipSocket();
	void ReloadWeapon();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();
	void UpdateAmmoValues();
	void JumpToShotgunEnd();
	void UpdateShotgunAmmoValues();
	void FireButtonPressed(bool bPressed);

	void SwapWeapons();
	

	
	UFUNCTION(BlueprintCallable)
	void ShotgunShellReload();
	UFUNCTION(BlueprintCallable)
	void ThrowGrenadeFinished();
	UFUNCTION(BlueprintCallable)
	void LaunchGrenade();
	UFUNCTION(Server, Reliable)
	void Server_LaunchGrenade(const FVector_NetQuantize& Target);

	
	void PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount);


		UFUNCTION(BlueprintPure)
    	FORCEINLINE AWeapon* GetEquippedWeapon() const {return EquippedWeapon ;}
    	FORCEINLINE int32 GetGrenades() const { return Grenades ;}
		FORCEINLINE int32 GetCarriedAmmo() const { return CarriedAmmo ;}
		FORCEINLINE bool GetIsLocallyReloading() const {return  bLocallyReloading;}

protected:

	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();
	UFUNCTION()
	void OnRep_SecondaryWeapon();
	void Fire();
	void FireProjectileWeapon();
	void FireHitScanWeapon();
	void FireShotgun();

	
	UFUNCTION(Server,Reliable)
	void Server_Reload();

	void HandleReload();

	int32 AmountToReload();

	void ThrowGrenade();

	UFUNCTION(Server, Reliable)
	void Server_ThrowGrenade();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> GrenadeClass;
	
	UFUNCTION(Server, Reliable)
	void Server_Fire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Fire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable)
	void Server_ShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets);

	

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHudCrosshairs(float DeltaTime);

	void AttachActorToRightHand(AActor* ActorToAttach);
	void AttachActorToLeftHand(AActor* ActorToAttach);
	void AttachActorToBackpack(AActor* ActorToAttach);
	void UpdateCarriedAmmo();
	void PlayWeaponEquipSound(AWeapon* WeaponToEquip);

	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);

private:

	UPROPERTY()
	ABlasterCharacter* Character{};
	UPROPERTY()
	class ABlasterPlayerController* BC_Controller{};
	UPROPERTY()
	class ABlasterHud* BC_Hud{};

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing= OnRep_EquippedWeapon, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon{};

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing= OnRep_SecondaryWeapon, meta = (AllowPrivateAccess = "true"))
	AWeapon* SecondaryWeapon{};

	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool bAiming = false;

	UPROPERTY()
	bool bAimingButtonPressed= false;

	UFUNCTION()
	void OnRep_Aiming();
	
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

	FVector_NetQuantize HitTargetImpactPoint{};

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
	void LocalFire(const FVector_NetQuantize& TraceHitTarget);
	void LocalShotgunFire(const TArray<FVector_NetQuantize> TraceHitTargets);

	bool CanFire();
	UPROPERTY(ReplicatedUsing=OnRep_CarriedAmmo)
	int32 CarriedAmmo;
	UFUNCTION()
	void OnRep_CarriedAmmo();
	UPROPERTY(ReplicatedUsing=OnRep_Grenades)
	int32 Grenades = 4;
	UFUNCTION()
	void OnRep_Grenades();
	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo = 500;
	UPROPERTY(EditAnywhere)
	int32 MaxGrenades = 4;
	void UpdateHudGrenades();

	UPROPERTY(EditAnywhere)
	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere)
	int32 StartingAmmo = 30;
	
	void InitializeCarriedAmmo();

	UPROPERTY(ReplicatedUsing=OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;
	
	UFUNCTION()
	void OnRep_CombatState();

	void ShowAttachedGrenade(bool bShowGrenade);

	bool ShouldSwapWeapons();

	UPROPERTY()
	bool bLocallyReloading = false;
	
};


