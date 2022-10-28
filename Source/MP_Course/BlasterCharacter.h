// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TurnInPlace.h"
#include "IInteractWithCrosshairs.h"
#include "CombatState.h"


#include "BlasterCharacter.generated.h"

class UBuffComponent;
struct FInputActionValue;
class UInputConfig;
class UCombatComponent;
UCLASS()
class MP_COURSE_API ABlasterCharacter : public ACharacter, public IIInteractWithCrosshairs
{
	GENERATED_BODY()

public:
	
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void UpdateHudHealth();
	void UpdateHudShields();
	void UpdateHudAmmo();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void Destroyed() override;
	void Elim();
	void HideElimText();
	void ShowElimText();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Elim();

	/* PUBLIC FUNCTIONS */
	
	void SetOverlappingWeapon(class AWeapon* Weapon);
	bool IsWeaponEquipped() const;
	bool IsAiming() const;
	void PlayFireMontage(bool bAiming);
	void PlayElimMontage();
	void PlayReloadMontage();
	void PlayThrowGrenadeMontage();
	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);
	
	/* GETTERS AND SETTERS: */
	
	FORCEINLINE void SetAO_Yaw(const float InYaw) {AO_Yaw = InYaw ;}
	FORCEINLINE void SetAO_Pitch(const float InPitch) {AO_Pitch = InPitch ;}
	FORCEINLINE float GetAO_Yaw() const {return AO_Yaw ;}
	FORCEINLINE float GetAO_Pitch() const {return AO_Pitch ;}
	FORCEINLINE ETurningInPlace GetTurningInPlace() const {return TurningInPlace ;}
	FORCEINLINE class UCameraComponent* GetFollowCamera() const {return FollowCamera ;}
	FORCEINLINE bool GetIsElimmed() const { return bElimmed ;}
	FORCEINLINE void SetIsElimmed(const bool Elimmed) {bElimmed = Elimmed ;}
	FORCEINLINE float GetHealth() const {return Health ;}
	FORCEINLINE float GetMaxHealth() const {return MaxHealth ;}
	FORCEINLINE float GetShields() const {return Shield ;}
	FORCEINLINE float GetMaxShields() const {return MaxShield ;}
	FORCEINLINE void SetHealth(float HealthAmount) { Health = HealthAmount ; }
	FORCEINLINE void SetShields(float ShieldAmount) { Shield = ShieldAmount ; }
	FORCEINLINE bool GetDisableGameplay() const {return bDisableGameplay ;}
	FORCEINLINE void SetDisableGameplay(const bool bDisable) { bDisableGameplay = bDisable ;}
	FORCEINLINE UCombatComponent* GetCombatComponent() const {return CombatComponent ;}
	FORCEINLINE UBuffComponent* GetBuffComponent() const {return BuffComponent ;}
	FORCEINLINE UAnimMontage* GetReloadMontage() const {return ReloadMontage ;}
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const {return Grenade ;}
	
	AWeapon* GetEquippedWeapon();
	FVector GetHitTarget() const;
	ECombatState GetCombatState() const;
	void FireButtonPressed();
	
protected:

	/** The input config that maps Input Actions to Input Tags*/
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputConfig* InputConfig;

	/** Handles moving forward/backward */
	void Input_Move(const FInputActionValue& InputActionValue);

	/** Handles mouse and stick look */
	void Input_Look(const FInputActionValue& InputActionValue);

	/** Handles Jumping */
	void Input_Jump(const FInputActionValue& InputActionValue);

	/** Handles Pew Pew */
	void Input_Fire(const FInputActionValue& InputActionValue);

	virtual void BeginPlay() override;
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void GrenadeButtonPressed();
	void ReloadButtonPressed();
	

	void AimOffset(float DeltaTime);	
	void TurnInPlace(float DeltaTime);

	void FireButtonReleased();
	
	void HideCameraForFpp();

	void PlayOnHitMontage();

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);
	
	void SpawnDefaultWeapon();


private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon{};

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess ="true"))
	class UCombatComponent* CombatComponent;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess ="true"))
	class UBuffComponent* BuffComponent;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();
	
	FRotator StartingAimRotation{};
	
	float AO_Yaw{};
	float InterpAO_Yaw{};
	float AO_Pitch{};

	ETurningInPlace TurningInPlace{};

	UPROPERTY(EditAnywhere, Category= Combat, meta=(AllowPrivateAccess = "true"))
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category= Combat, meta=(AllowPrivateAccess = "true"))
	UAnimMontage* OnHitMontage;

	UPROPERTY(EditAnywhere, Category= Combat, meta=(AllowPrivateAccess = "true"))
	UAnimMontage* OnElimMontage;

	UPROPERTY(EditAnywhere, Category= Combat, meta=(AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category= Combat, meta=(AllowPrivateAccess = "true"))
	UAnimMontage* ThrowGrenadeMontage;
	
	UPROPERTY(EditAnywhere, Category= Combat, meta=(AllowPrivateAccess = "true"))
	float CameraThreshold =200.f;
	UPROPERTY(EditAnywhere, Category= Stats, meta=(AllowPrivateAccess = "true"))
	float MaxHealth = 100.f;
	UPROPERTY(EditAnywhere, Category= Stats, meta=(AllowPrivateAccess = "true"), ReplicatedUsing=OnRep_Health)
	float Health = 100.f;
	UPROPERTY(EditAnywhere, Category= Stats, meta=(AllowPrivateAccess = "true"))
	float MaxShield = 100.f;
	UPROPERTY(EditAnywhere, Category= Stats, meta=(AllowPrivateAccess = "true"), ReplicatedUsing=OnRep_Shield)
	float Shield = 20.f;
	
	UFUNCTION()
	void OnRep_Health(float LastHealth);
	
	UFUNCTION()
	void OnRep_Shield(float LastShield);
	
	UPROPERTY()
	class ABlasterPlayerController* BlasterPlayerController{};
	UPROPERTY()
	bool bElimmed = false;

	FTimerHandle ElimTimer;
	UFUNCTION()
	void ElimTimerFinished();
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay={3.f};

	UPROPERTY(Replicated)
	bool bDisableGameplay{};
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess ="true"))
	UStaticMeshComponent* Grenade;
	
	/*
	 * Default Weapon
	 */

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;
	
	
	
};
