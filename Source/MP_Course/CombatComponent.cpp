// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "BlasterCharacter.h"
#include "Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
// #include "Components/SphereComponent.h"
#include "BlasterPlayerController.h"
#include "BlasterHud.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
// #include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Sound/SoundCue.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	BaseWalkSpeed= 600.f;
	AimingWalkSpeed = 450.f;

}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
		if(Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}

		if(Character->HasAuthority())
        {
        	InitializeCarriedAmmo();
        }
	}
	


	
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTargetImpactPoint = HitResult.ImpactPoint;
		InterpFOV(DeltaTime);
		SetHudCrosshairs(DeltaTime);
	}
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME(UCombatComponent, CombatState);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo,COND_OwnerOnly);
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	if(Character == nullptr || EquippedWeapon == nullptr) return;
	
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimingWalkSpeed : BaseWalkSpeed ;
	}
	if(Character->IsLocallyControlled() && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle && bAiming)
	{
		Character->ShowSniperScopeWidget(true);
	}
	else if(Character->IsLocallyControlled() && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle && !bAiming)
	{
		Character->ShowSniperScopeWidget(false);
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimingWalkSpeed : BaseWalkSpeed ;
	}
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if(Character == nullptr || WeaponToEquip == nullptr) return;
	if(CombatState != ECombatState::ECS_Unoccupied) return;
	if(EquippedWeapon)
	{
		EquippedWeapon->Dropped();
	}

	EquippedWeapon = WeaponToEquip;
	
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	if(EquippedWeapon->GetEquipSound())
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquippedWeapon->GetEquipSound(), EquippedWeapon->GetActorLocation(), FRotator());
	}
	
	const USkeletalMeshSocket* RightHandSocket = GetEquipSocket();
     
	if(RightHandSocket)
	{
		RightHandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->SetHUDWeaponAmmo();

	if(CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	
	BC_Controller = BC_Controller == nullptr ? Cast<ABlasterPlayerController>(Character->GetController()) : BC_Controller ;
	if(BC_Controller)
	{
		BC_Controller->SetHudCarriedAmmo(CarriedAmmo);
	}
	
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

const USkeletalMeshSocket* UCombatComponent::GetEquipSocket()
{
	const USkeletalMeshSocket* RightHandSocket{};
	switch (EquippedWeapon->GetWeaponType())
	{
		case EWeaponType::EWT_AssaultRifle:
			RightHandSocket = Character->GetMesh()->GetSocketByName(FName("ARSocket"));
		break;
		case EWeaponType::EWT_RocketLauncher:
			RightHandSocket = Character->GetMesh()->GetSocketByName(FName("RocketSocket"));
			break;
		case EWeaponType::EWT_Pistol:
			RightHandSocket = Character->GetMesh()->GetSocketByName(FName("PistolSocket"));
			break;
		case EWeaponType::EWT_SubMachineGun:
			RightHandSocket = Character->GetMesh()->GetSocketByName(FName("SMGSocket"));
			break;
		case EWeaponType::EWT_Shotgun:
			RightHandSocket = Character->GetMesh()->GetSocketByName(FName("ShotgunSocket"));
			break;
		case EWeaponType::EWT_SniperRifle:
			RightHandSocket = Character->GetMesh()->GetSocketByName(FName("PistolSocket"));
			break;
		case EWeaponType::EWT_GrenadeLauncher:
			RightHandSocket = Character->GetMesh()->GetSocketByName(FName("GrenadeLauncherSocket"));
			break;
		case EWeaponType::EWT_MAX:
			break;
		default: ;
	}


	return RightHandSocket;
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if(EquippedWeapon && Character)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		
		const USkeletalMeshSocket* RightHandSocket = GetEquipSocket();
		if(RightHandSocket)
		{
			RightHandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
		}
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::ReloadWeapon()
{
	if(CarriedAmmo<=0) return;

	if(CarriedAmmo>0 && CombatState == ECombatState::ECS_Unoccupied)
	{
		Server_Reload();
	}
}

void UCombatComponent::FinishReloading()
{
	if(Character == nullptr) return;
	if(Character->HasAuthority())
	{
		CombatState= ECombatState::ECS_Unoccupied;
		UpdateAmmoValues();
	}
	if(bFireButtonPressed)
	{
		Fire();
	}
}

void UCombatComponent::UpdateAmmoValues()
{
	if(Character == nullptr || EquippedWeapon == nullptr ) return;
	const int32 ReloadAmount = AmountToReload();
	if(CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	
	
	BC_Controller = BC_Controller == nullptr ? Cast<ABlasterPlayerController>(Character->GetController()) : BC_Controller ;
	if(BC_Controller)
	{
		BC_Controller->SetHudCarriedAmmo(CarriedAmmo);
	}
	
	EquippedWeapon->AddAmmo(-ReloadAmount);
}

void UCombatComponent::JumpToShotgunEnd()
{

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
    		
	if(EquippedWeapon->IsFull() || CarriedAmmo == 0)
	{
		if(AnimInstance && Character->GetReloadMontage())
            {
            	AnimInstance->Montage_JumpToSection(FName("ShotgunEnd"));
            }
	}
	else
	{
		if(AnimInstance && Character->GetReloadMontage())
		{
			if(AnimInstance->Montage_GetCurrentSection(Character->GetReloadMontage()) == FName("ShotgunCheck"))
			{
				AnimInstance->Montage_JumpToSection(FName("ShotgunShell"));
			}
		}
	}
	
}

void UCombatComponent::UpdateShotgunAmmoValues()
{
	if(Character == nullptr || EquippedWeapon == nullptr ) return;
	
	if(CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= 1;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	BC_Controller = BC_Controller == nullptr ? Cast<ABlasterPlayerController>(Character->GetController()) : BC_Controller ;
	if(BC_Controller)
	{
		BC_Controller->SetHudCarriedAmmo(CarriedAmmo);
	}
	EquippedWeapon->AddAmmo(-1);
	bCanFire = true;
	
		JumpToShotgunEnd();
	
	
	
}

void UCombatComponent::Server_Reload_Implementation()
{
	if(Character == nullptr || EquippedWeapon == nullptr) return;
	
	CombatState=ECombatState::ECS_Reloading;
	HandleReload();
}

void UCombatComponent::HandleReload()
{
	Character->PlayReloadMontage();
}

int32 UCombatComponent::AmountToReload()
{
	if(EquippedWeapon == nullptr) return 0;

	int32 RoomInMagazine = EquippedWeapon->GetMagazineCapacity() - EquippedWeapon->GetAmmo();
	if(CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		const int32 AmountCarried = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		const int32 Least = FMath::Min(RoomInMagazine, AmountCarried);
		return FMath::Clamp(RoomInMagazine, 0, Least);
	}	
	return 0;
}

void UCombatComponent::ThrowGrenade()
{
	if(CombatState != ECombatState::ECS_Unoccupied) return;
	CombatState = ECombatState::ECS_ThrowingGrenade;
	if(Character)
	{
		Character->PlayThrowGrenadeMontage();
	}
	if(Character && !Character->HasAuthority())
	{
		Server_ThrowGrenade();
	}
	
}

void UCombatComponent::Server_ThrowGrenade_Implementation()
{
	CombatState = ECombatState::ECS_ThrowingGrenade;
	if(Character)
	{
		Character->PlayThrowGrenadeMontage();
	}
}


void UCombatComponent::OnRep_CombatState()
{
	switch(CombatState)
	{
	case ECombatState::ECS_Reloading :
		HandleReload();
		break;
	case ECombatState::ECS_Unoccupied :
		if(bFireButtonPressed)
		{
			Fire();
		}
		break;
	case ECombatState::ECS_ThrowingGrenade:
		if(Character && !Character->IsLocallyControlled())
		{
			Character->PlayThrowGrenadeMontage();
		}
		break;
		
	default: ;
	}
}

void UCombatComponent::Fire()
{
	if(Character->GetDisableGameplay()) return;
	if(EquippedWeapon)
	{
		if(CanFire())
		{
			bCanFire=false;
			Server_Fire(HitTargetImpactPoint);
	        CrosshairShootingFactor = -1.5f;
	        StartFireTimer();
        }
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	if(Character->GetDisableGameplay()) return;
	bFireButtonPressed = bPressed;
	if(bFireButtonPressed)
	{
		Fire();
	}
}

void UCombatComponent::ShotgunShellReload()
{
	if(Character && Character->HasAuthority())
	{
		UpdateShotgunAmmoValues();
	}
}

void UCombatComponent::ThrowGrenadeFinished()
{
	CombatState = ECombatState::ECS_Unoccupied;
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;

	if(GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation = {ViewportSize.X / 2.f , ViewportSize.Y/2.f };
	FVector CrosshairWorldPostition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPostition,
		CrosshairWorldDirection
	);

	if(bScreenToWorld)
	{
		FVector Start = CrosshairWorldPostition;

		if(Character)
		{
			const float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start+=CrosshairWorldDirection * (DistanceToCharacter + 30.f);
			DrawDebugSphere(GetWorld(),Start, 16.f, 12, FColor::Red);
		}
		
		const FVector End = Start + CrosshairWorldDirection * 5'000;

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
			
			);

		if(TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UIInteractWithCrosshairs>())
		{
			HUDPackage.CrosshairsColor = FLinearColor::Red;
			CrosshairTargetFactor = .5f;
		}
		else
		{
			HUDPackage.CrosshairsColor = FLinearColor::White;
			CrosshairTargetFactor = 0.f;
		}

		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
			
		}
		else
		{
		
		}
	}
}

void UCombatComponent::SetHudCrosshairs(float DeltaTime)
{
	if(Character==nullptr || Character->Controller == nullptr) return;

	if(BC_Controller == nullptr)
	{
		BC_Controller = Cast<ABlasterPlayerController>(Character->Controller);
	}
	 
	if(BC_Controller)
	{
		if(BC_Hud == nullptr)
		{
			BC_Hud = Cast<ABlasterHud>(BC_Controller->GetHUD());
		}
		if(BC_Hud)
		{
			
			if(EquippedWeapon)
			{
				HUDPackage.CrosshairsCenter = EquippedWeapon->GetCrosshairsCenter();
				HUDPackage.CrosshairsLeft = EquippedWeapon->GetCrosshairsLeft();
				HUDPackage.CrosshairsRight = EquippedWeapon->GetCrosshairsRight();
				HUDPackage.CrosshairsTop = EquippedWeapon->GetCrosshairsTop();
				HUDPackage.CrosshairsBottom = EquippedWeapon->GetCrosshairsBottom();
                		
			}
			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
			}
			
			const FVector2D WalkSpeedRange = {0.f, Character->GetCharacterMovement()->MaxWalkSpeed};
			const FVector2D VelocityMultiplierRange(0.f,1.f);
				FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;

			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());
			
			if(Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairFallingFactor = FMath::FInterpTo(CrosshairFallingFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairFallingFactor = FMath::FInterpTo(CrosshairFallingFactor, 0.f, DeltaTime, 30.f);
			}
			if(CrosshairMovingFactor <= 0.1f && CrosshairVelocityFactor <= 0.1f )
			{
				if(bAiming)
				{
					CrosshairAimingFactor = FMath::FInterpTo(CrosshairAimingFactor, 2.f, DeltaTime, .5f);
				}
				else
				{
					CrosshairAimingFactor = FMath::FInterpTo(CrosshairAimingFactor, 0, DeltaTime, 30.f);
				}
				
			}
			
			if(Character->GetCharacterMovement()->IsCrouching())
			{
				CrosshairCrouchingFactor = FMath::FInterpTo(CrosshairCrouchingFactor, 1.f, DeltaTime, 2.5f);
			}
			else
			{
				CrosshairCrouchingFactor = FMath::FInterpTo(CrosshairCrouchingFactor, 0.f, DeltaTime, 2.5f);
			}
			
			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 2.f);
			
			HUDPackage.CrosshairSpread = (CrosshairVelocityFactor + CrosshairFallingFactor ) - (CrosshairTargetFactor + CrosshairCrouchingFactor+CrosshairAimingFactor + CrosshairShootingFactor);
			
			BC_Hud->SetHudPackage(HUDPackage);
		}
	}
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if(EquippedWeapon == nullptr) return;

	if(bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomedInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, 4);
	}
	if(Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::FireTimerFinished()
{
	if(EquippedWeapon == nullptr) return;
	bCanFire = true;
	if(bFireButtonPressed && EquippedWeapon->GetAutomaticWeapon())
	{
		Fire();
	}
	if(EquippedWeapon->IsEmpty())
	{
		ReloadWeapon();
	}
}

void UCombatComponent::StartFireTimer()
{
    if(Character)
	Character->GetWorldTimerManager().SetTimer(AutoFireHandle, this, &UCombatComponent::FireTimerFinished, EquippedWeapon->GetFireDelay());
}

void UCombatComponent::Multicast_Fire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if(EquippedWeapon == nullptr)
	{
		return;
	}
	if(Character && CombatState == ECombatState::ECS_Reloading && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
		CombatState = ECombatState::ECS_Unoccupied;
		return;
	}
	if(Character && CombatState == ECombatState::ECS_Unoccupied)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}
}

void UCombatComponent::Server_Fire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	Multicast_Fire(TraceHitTarget);
}

bool UCombatComponent::CanFire()
{
	if(EquippedWeapon == nullptr)
	{
		return false;
	}
	if (!EquippedWeapon->IsEmpty() && bCanFire && CombatState == ECombatState::ECS_Reloading && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun) return true;
	return !EquippedWeapon->IsEmpty() && bCanFire && CombatState==ECombatState::ECS_Unoccupied;
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	BC_Controller = BC_Controller == nullptr ? Cast<ABlasterPlayerController>(Character->GetController()) : BC_Controller ;
	if(BC_Controller)
	{
		BC_Controller->SetHudCarriedAmmo(CarriedAmmo);
	}
	const bool bJumpToShotgunEnd = (CombatState == ECombatState::ECS_Reloading &&
		EquippedWeapon != nullptr &&
		EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun);
		// && CarriedAmmo == 0);

	if(bJumpToShotgunEnd)
	{
		JumpToShotgunEnd();
	}

		
}

void UCombatComponent::InitializeCarriedAmmo()
{
	
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, StartingAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher, StartingAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, StartingAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SubMachineGun, StartingAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Shotgun, StartingAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SniperRifle, StartingAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_GrenadeLauncher, StartingAmmo);
	
}
