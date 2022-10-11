#include "BCAnimInstance.h"

#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon.h"
#include "Camera/CameraComponent.h"
#include "MP_Course.h"
#include "CombatState.h"

void UBCAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BlasterCharacter=Cast<ABlasterCharacter>(TryGetPawnOwner());
	
}

void UBCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{

	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if(BlasterCharacter== nullptr)
	{
		BlasterCharacter=Cast<ABlasterCharacter>(TryGetPawnOwner());
	}
	if(BlasterCharacter == nullptr) return;
	
	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0.f;
	MovementSpeed = Velocity.Size();

	bIsInAir=BlasterCharacter->GetCharacterMovement()->IsFalling();
	bIsAccellerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bIsWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
	EquippedWeapon = BlasterCharacter->GetEquippedWeapon();
	bIsCrouched = BlasterCharacter->bIsCrouched;
	bAiming = BlasterCharacter->IsAiming();
	TurningInPlace = BlasterCharacter->GetTurningInPlace();
	bElimmed= BlasterCharacter->GetIsElimmed();


    // Get Yaw Offset, Used for Strafing -- Delta Rotator between character's Base Aim Rotation ( World Space direction camera is facing )
	// and Movement Rotation -  The direction our character is moving in ( also world space, only > 0 when moving ) 
	const FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
	const FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
	const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, 15.f );
	YawOffset = DeltaRotation.Yaw;
	// Get Lean variable used to lean in running blendspaces -
	// The Yaw  of a delta rotator between the character's current rotation and last frame rotation,
	// scaled by delta time and clamped to debug fast input. 
	// This gives the direction the player is turning. 
	CharacterRoationLastFrame = CharacterRotation;
	CharacterRotation = BlasterCharacter->GetActorRotation();
	const FRotator DeltaLeanRotation = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRoationLastFrame);
	const float LeanTarget = DeltaLeanRotation.Yaw / DeltaSeconds;
	const float LeanInterp = FMath::FInterpTo(Lean, LeanTarget, DeltaSeconds, 6.f);
	Lean = FMath::Clamp(LeanInterp, -180.f, 180.f);

	AOYaw = BlasterCharacter->GetAO_Yaw();
	AOPitch = BlasterCharacter->GetAO_Pitch();

	if(bIsWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BlasterCharacter->GetMesh())
	{
		// Correct Left Hand Placement On Weapon in Relation to Right Hand
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition{};
		FRotator OutRotation{};
		BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FQuat(LeftHandTransform.GetRotation()).Rotator(), OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));
		//Correct Left Thumb placement on weapon in relation to left hand
		LeftThumbTransform = EquippedWeapon -> GetWeaponMesh()->GetSocketTransform(FName("ThumbSocket"), ERelativeTransformSpace::RTS_World);
		
		FVector ThumbOutPosition{};
		FRotator ThumbOutRotation{};
		BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("thumb_03_l"), LeftThumbTransform.GetLocation(), FRotator::ZeroRotator, ThumbOutPosition, ThumbOutRotation);
		LeftThumbTransform.SetLocation(ThumbOutPosition);
		LeftThumbTransform.SetRotation(FQuat(ThumbOutRotation));
		//Correct Right Hand placement in relation to shoulder to position weapon correctly
		
			RightShoulderTransform = EquippedWeapon -> GetWeaponMesh()->GetSocketTransform(FName("ShoulderSocket"), ERelativeTransformSpace::RTS_World);
			FVector ShoulderOutPosition{};
			FRotator ShoulderOutRotation{};
			BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), RightShoulderTransform.GetLocation(), RightShoulderTransform.Rotator(), ShoulderOutPosition, ShoulderOutRotation);
			RightShoulderTransform.SetLocation(ShoulderOutPosition);
			RightShoulderTransform.SetRotation(FQuat(ShoulderOutRotation));
		
		// Correct Look At Rotation of equipped weapon to ensure it points towards crosshairs (clients only!!!)
		
		// FTransform MuzzleFlashTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"));
		if(BlasterCharacter->IsLocallyControlled())
		{
			bLocallyControlledCharacter=true;
			if(BlasterCharacter->GetDisableGameplay()) return;
		
			const FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r", ERelativeTransformSpace::RTS_World));
            FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation()-BlasterCharacter->GetHitTarget()));
            RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaSeconds, 40.f);    
		}
	}

	bUseFabrik = BlasterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied  && !BlasterCharacter->GetDisableGameplay();
	bUseAimOffsets = BlasterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !BlasterCharacter->GetDisableGameplay();
	
}
