#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TurnInPlace.h"


#include "BCAnimInstance.generated.h"

/*  Animation Instance class to parent our character's Anim BP from */
UCLASS()
class MP_COURSE_API UBCAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	
protected:
	


private:

	UPROPERTY(BlueprintReadOnly, Category="Character", meta = (AllowPrivateAccess="true"))
	class ABlasterCharacter* BlasterCharacter;

	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))
	float MovementSpeed;
	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))
	bool bIsInAir;
	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))
	bool bIsAccellerating;

	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))
	bool bIsWeaponEquipped;

	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))
	class AWeapon* EquippedWeapon;
	
	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))
	bool bIsCrouched;
	
	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))
	bool bAiming;

	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))
	float YawOffset;
	
	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))
	float Lean;
	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))
	FRotator CharacterRoationLastFrame;
	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))
	FRotator CharacterRotation;
	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))
	FRotator DeltaRotation;
	
	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))
    float AOYaw;
    	
    UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))
    float AOPitch;

	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))
	FTransform LeftHandTransform{};

	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))
	FTransform RightShoulderTransform{};

	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))
	FTransform LeftThumbTransform{};

	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))	
	ETurningInPlace TurningInPlace{};
	
	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))	
	FRotator RightHandRotation{};

	UPROPERTY(BlueprintReadOnly, Category="Character | Movement", meta = (AllowPrivateAccess="true"))	
	bool bLocallyControlledCharacter;
	
};
