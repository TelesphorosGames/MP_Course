// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TurnInPlace.h"
#include "IInteractWithCrosshairs.h"


#include "BlasterCharacter.generated.h"

UCLASS()
class MP_COURSE_API ABlasterCharacter : public ACharacter, public IIInteractWithCrosshairs
{
	GENERATED_BODY()

public:
	
	ABlasterCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	void SetOverlappingWeapon(class AWeapon* Weapon);

	bool IsWeaponEquipped() const;

	bool IsAiming() const;

	void PlayFireMontage(bool bAiming);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_OnHit();
	
	// GETTERS AND SETTERS:

	AWeapon* GetEquippedWeapon();

	FORCEINLINE void SetAO_Yaw(float inYaw) {AO_Yaw = inYaw; }
	FORCEINLINE void SetAO_Pitch(float inPitch) {AO_Pitch = inPitch; }
	FORCEINLINE float GetAO_Yaw() const {return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const {return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const {return TurningInPlace; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const {return FollowCamera ; }
	FORCEINLINE float GetMovingGunCrosshairFactor() const {return MovingGunCrosshairsFactor ; }
	
	FVector GetHitTarget() const;

	
protected:

	virtual void BeginPlay() override;
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();

	void AimOffset(float DeltaTime);
	void TurnInPlace(float DeltaTime);

	void FireButtonPressed();
	void FireButtonReleased();

	void HideCameraForFpp();

	void PlayOnHitMontage();
	
private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* CombatComponent;

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
	class UAnimMontage* OnHitMontage;

	float MovingGunCrosshairsFactor;
	
	UPROPERTY(EditAnywhere, Category= Combat, meta=(AllowPrivateAccess = "true"))
	float CameraThreshold =200.f;
	
	 
};
