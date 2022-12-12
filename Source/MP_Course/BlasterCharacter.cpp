// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"

#include "BuffComponent.h"
#include "Announcement.h"
#include "BlasterGameMode.h"
#include "BlasterPlayerController.h"
#include "BlasterPlayerState.h"
#include "CharacterOverlay.h"
#include "CombatComponent.h"
#include "Weapon.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "MP_Course.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextBlock.h"
#include "CombatState.h"
#include "LagCompensationComponent.h"
#include "MyEnhancedInputComponent.h"
#include "MyGameplayTags.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
ABlasterCharacter::ABlasterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetMesh());
	SpringArm->bUsePawnControlRotation = true;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm);
	
	bUseControllerRotationYaw = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Overhead Widget"));
	OverheadWidget->SetupAttachment(RootComponent);

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetIsReplicated(true);

	BuffComponent = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	BuffComponent->SetIsReplicated(true);

	LagCompensationComponent = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensationComponent"));
	

	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);

	Grenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grenade"));
	
	Grenade->SetupAttachment(GetMesh(), FName("PistolSocket"));
	
	Grenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HeadBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HeadBox"));
	HeadBox->SetupAttachment(GetMesh(), FName("head"));

	HitCollisionBoxes.Add(FName("head"), HeadBox);

	NeckBox = CreateDefaultSubobject<UBoxComponent>(TEXT("NeckBox"));
	NeckBox->SetupAttachment(GetMesh(), FName("neck_01"));

	HitCollisionBoxes.Add(FName("Neck"), NeckBox);
	
	ShouldersBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ShouldersBox"));
	ShouldersBox->SetupAttachment(GetMesh(), FName("spine_05"));

	HitCollisionBoxes.Add(FName("Shoulders"), ShouldersBox);

	ChestBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ChestBox"));
	ChestBox->SetupAttachment(GetMesh(), FName("spine_04"));

	HitCollisionBoxes.Add(FName("Chest"), ChestBox);

	WaistBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WaistBox"));
	WaistBox->SetupAttachment(GetMesh(), FName("spine_01"));

	HitCollisionBoxes.Add(FName("Waist"), WaistBox);

	UpperArmLeft = CreateDefaultSubobject<UBoxComponent>(TEXT("UpperArmLeft"));
	UpperArmLeft->SetupAttachment(GetMesh(), FName("upperarm_l"));

	HitCollisionBoxes.Add(FName("UpperArmLeft"), UpperArmLeft);

	UpperArmRight = CreateDefaultSubobject<UBoxComponent>(TEXT("UpperArmRight"));
	UpperArmRight->SetupAttachment(GetMesh(), FName("upperarm_r"));

	HitCollisionBoxes.Add(FName("UpperArmRight"), UpperArmRight);

	LowerArmLeft = CreateDefaultSubobject<UBoxComponent>(TEXT("LowerArmLeft"));
	LowerArmLeft->SetupAttachment(GetMesh(), FName("lowerarm_l"));

	HitCollisionBoxes.Add(FName("LowerArmLeft"), LowerArmLeft);
	
	LowerArmRight = CreateDefaultSubobject<UBoxComponent>(TEXT("LowerArmRight"));
	LowerArmRight->SetupAttachment(GetMesh(), FName("lowerarm_r"));

	HitCollisionBoxes.Add(FName("LowerArmRight"), LowerArmRight);

	HandRight = CreateDefaultSubobject<UBoxComponent>(TEXT("HandRight"));
	HandRight->SetupAttachment(GetMesh(), FName("hand_r"));

	HitCollisionBoxes.Add(FName("HandRight"), HandRight);

	HandLeft = CreateDefaultSubobject<UBoxComponent>(TEXT("HandLeft"));
	HandLeft->SetupAttachment(GetMesh(), FName("hand_l"));

	HitCollisionBoxes.Add(FName("HandLeft"), HandLeft);

	AbsBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AbsBox"));
	AbsBox->SetupAttachment(GetMesh(), FName("spine_03"));

	HitCollisionBoxes.Add(FName("Abs"), AbsBox);

	UpperLegLeft = CreateDefaultSubobject<UBoxComponent>(TEXT("UpperLegLeft"));
	UpperLegLeft->SetupAttachment(GetMesh(), FName("thigh_l"));

	HitCollisionBoxes.Add(FName("UpperLegLeft"), UpperLegLeft); 

	UpperLegRight = CreateDefaultSubobject<UBoxComponent>(TEXT("UpperLegRight"));
	UpperLegRight->SetupAttachment(GetMesh(), FName("thigh_r"));

	HitCollisionBoxes.Add(FName("UpperLegRight"), UpperLegRight);

	LowerLegLeft = CreateDefaultSubobject<UBoxComponent>(TEXT("LowerLegLeft"));
	LowerLegLeft->SetupAttachment(GetMesh(), FName("calf_l"));

	HitCollisionBoxes.Add(FName("LowerLegLeft"), LowerLegLeft);
	
	LowerLegRight = CreateDefaultSubobject<UBoxComponent>(TEXT("LowerLegRight"));
	LowerLegRight->SetupAttachment(GetMesh(), FName("calf_r"));

	HitCollisionBoxes.Add(FName("LowerLegRight"), LowerLegRight);

	FootRight = CreateDefaultSubobject<UBoxComponent>(TEXT("FootRight"));
	FootRight->SetupAttachment(GetMesh(), FName("ball_r"));

	HitCollisionBoxes.Add(FName("FootRight"), FootRight);

	FootLeft = CreateDefaultSubobject<UBoxComponent>(TEXT("FootLeft"));
	FootLeft->SetupAttachment(GetMesh(), FName("ball_l"));

	HitCollisionBoxes.Add(FName("FootLeft"), FootLeft);

	for(auto Box : HitCollisionBoxes)
	{
		if(Box.Value)
		{
			Box.Value->SetCollisionObjectType(ECC_HitBox);
			Box.Value->SetCollisionResponseToAllChannels(ECR_Ignore);
			Box.Value->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);
			Box.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			 
		}
	}
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
/* NEW WAY: 
	// UMyEnhancedInputComponent* MyEnhancedInputComponent = Cast<UMyEnhancedInputComponent>(PlayerInputComponent);
	//
	// check(MyEnhancedInputComponent);
	//
	//
	// const FMyGameplayTags& GameplayTags = FMyGameplayTags::Get();
	//
	// //Bind Input actions by tag
	// MyEnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ABlasterCharacter::Input_Move);
	// MyEnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ABlasterCharacter::Input_Look);
	// MyEnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ABlasterCharacter::Input_Look);
	// MyEnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_Fire, ETriggerEvent::Triggered, this, &ABlasterCharacter::Input_Fire);
	// MyEnhancedInputComponent->BindActionByTag(InputConfig, GameplayTags.InputTag_Jump, ETriggerEvent::Triggered, this, &ABlasterCharacter::Input_Jump);
*/




	// OLD WAY 
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	check(PlayerInputComponent);
	
	
	
	PlayerInputComponent->BindAxis("MoveForward", this, &ABlasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABlasterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABlasterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ABlasterCharacter::LookUp);
	//TODO : OVERRIDE JUMP AND CHECK IF GAME IS OVER 
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABlasterCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABlasterCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABlasterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABlasterCharacter::AimButtonReleased);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABlasterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABlasterCharacter::FireButtonReleased);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ABlasterCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &ABlasterCharacter::GrenadeButtonPressed);
	
	
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	BlasterPlayerController = Cast<ABlasterPlayerController>(Controller);
	
	SpawnDefaultWeapon();
	UpdateHudHealth();
	UpdateHudShields();
	UpdateHudAmmo();
	if(HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ABlasterCharacter::ReceiveDamage);
		
	}
	
	HideElimText();

	if(Grenade)
	{
		Grenade->SetVisibility(false);
	}
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ABlasterCharacter, Health);
	DOREPLIFETIME(ABlasterCharacter, bDisableGameplay);
	DOREPLIFETIME(ABlasterCharacter, Shield);

}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	AimOffset(DeltaTime);

	if(BlasterPlayerController)
	{
		float gametime = BlasterPlayerController->GetServerTime();

	
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, -1, FColor::Blue, FString::Printf(TEXT("Current Time : %f"), gametime));
		}
	}
	
}

void ABlasterCharacter::PostInitializeComponents()
{
	
	if(CombatComponent)
	{
		CombatComponent->Character = this;
	}
	if(BuffComponent)
	{
		BuffComponent->Character = this;
		BuffComponent->SetInitialSpeeds(GetCharacterMovement()->MaxWalkSpeed, GetCharacterMovement()->MaxWalkSpeedCrouched);
	}
	if(LagCompensationComponent)
	{
		LagCompensationComponent->Character = this;
		if(Controller)
		{
			LagCompensationComponent->BlasterPlayerController = Cast<ABlasterPlayerController>(Controller);
		}
	}

	
	Super::PostInitializeComponents();
}

void ABlasterCharacter::Destroyed()
{
	Super::Destroyed();
	ABlasterGameMode* BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	bool bMatchNotInProgress = BlasterGameMode && BlasterGameMode->GetMatchState() != MatchState::InProgress;
	if(CombatComponent && CombatComponent->EquippedWeapon && bMatchNotInProgress)
	{
		CombatComponent->EquippedWeapon->Destroy();
	}
}

void ABlasterCharacter::DropOrDestroyWeapons()
{
	if(GetEquippedWeapon())
	{
		DropOrDestroyWeapon(GetEquippedWeapon());
	}
	if(GetSecondaryWeapon())
	{
		DropOrDestroyWeapon(GetSecondaryWeapon());
	}
}

void ABlasterCharacter::Elim(bool bPlayerLeftGame)
{
	
	DropOrDestroyWeapons();
	
	Multicast_Elim(bPlayerLeftGame);
	
		
}

void ABlasterCharacter::HideElimText()
{
	if(BlasterPlayerController)
	{
		ABlasterHud* BlasterHud = Cast<ABlasterHud>(BlasterPlayerController->GetHUD());
		if(BlasterHud &&
			BlasterHud->CharacterOverlay&&
			BlasterHud->CharacterOverlay->ElimText)
		{
			BlasterHud->CharacterOverlay->ElimText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ABlasterCharacter::ShowElimText()
{
	if(BlasterPlayerController)
	{
		ABlasterHud* BlasterHud = Cast<ABlasterHud>(BlasterPlayerController->GetHUD());
		if(BlasterHud &&
			BlasterHud->CharacterOverlay&&
			BlasterHud->CharacterOverlay->ElimText
			)
		{
			BlasterHud->CharacterOverlay->ElimText->SetVisibility(ESlateVisibility::Visible);
			
		}
	}
}

void ABlasterCharacter::Multicast_Elim_Implementation(bool bPlayerLeftGame)
{
	bLeftGame = bPlayerLeftGame;

	
	SetIsElimmed(true);
	PlayElimMontage();
	if(BlasterPlayerController)
	{
		BlasterPlayerController->SetHudWeaponAmmo(0);
	}
	SetDisableGameplay(true);
	if(CombatComponent)
	{
		CombatComponent->FireButtonPressed(false);
		if(IsLocallyControlled() &&
			CombatComponent->EquippedWeapon &&
			CombatComponent->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle &&
			CombatComponent->bAiming)
		{
			ShowSniperScopeWidget(false);
		}
	}
	ShowElimText();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetWorldTimerManager().SetTimer(ElimTimer, this, &ABlasterCharacter::ElimTimerFinished, ElimDelay);
		
}

void ABlasterCharacter::ElimTimerFinished()
{
	ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	if(BlasterGameMode && !bLeftGame)
	{
		BlasterGameMode->RequestRespawn(this, Controller);
	}
	
	if(bLeftGame && IsLocallyControlled())
	{
		OnLeftGame.Broadcast();
	}
	
	HideElimText();
}

void ABlasterCharacter::Server_LeaveGame_Implementation()
{
	ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
	if(BlasterGameMode && BlasterPlayerState)
	{
		BlasterGameMode->PlayerLeftGame(BlasterPlayerState);
		
	}
}

AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	if(CombatComponent == nullptr) return nullptr;
	return CombatComponent->EquippedWeapon;
}

AWeapon* ABlasterCharacter::GetSecondaryWeapon()
{
	if(CombatComponent == nullptr) return nullptr;
	return CombatComponent->SecondaryWeapon;
}

bool ABlasterCharacter::IsLocallyReloading()
{
	if(CombatComponent == nullptr) return false;
	return CombatComponent->bLocallyReloading;
}


FVector ABlasterCharacter::GetHitTarget() const
{
	if(CombatComponent==nullptr) return FVector();
	return CombatComponent->HitTargetImpactPoint;
}

ECombatState ABlasterCharacter::GetCombatState() const
{
	if(CombatComponent==nullptr)
	{
		return ECombatState::ECS_MAX;
	}
	return CombatComponent->CombatState;
}


void ABlasterCharacter::MoveForward(float Value)
{
	if(bDisableGameplay) return;
	if (Controller != nullptr && Value !=0.f)
	{
		const FRotator YawRotation( 0.f, Controller->GetControlRotation().Yaw, 0.f );
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
		
	}
}

void ABlasterCharacter::MoveRight(float Value)
{
	if(bDisableGameplay) return;
	if (Controller != nullptr && Value !=0.f)
	{
		const FRotator YawRotation = { 0.f, Controller->GetControlRotation().Yaw, 0.f };
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
		
	}
}

void ABlasterCharacter::Turn(float Value)
{
	
	AddControllerYawInput(Value);
	
}

void ABlasterCharacter::LookUp(float Value)
{
	if( Value > 0)
	{
		if(AO_Pitch < -80.f) return;
	}

	AddControllerPitchInput(Value);
}

void ABlasterCharacter::EquipButtonPressed()
{
	if(bDisableGameplay) return;
	if(CombatComponent)
	{
		if(GetCombatState()==ECombatState::ECS_Unoccupied)
		{
			ServerEquipButtonPressed();
		}
		
		if(CombatComponent->ShouldSwapWeapons() && !HasAuthority() && GetCombatState() == ECombatState::ECS_Unoccupied && OverlappingWeapon == nullptr)
		{
			PlaySwapWeaponMontage();
			CombatComponent->CombatState = ECombatState::ECS_SwappingWeapons;
		}
	}
}

void ABlasterCharacter::CrouchButtonPressed()
{
	if(bDisableGameplay) return;
	if(bIsCrouched)
	{
		UnCrouch();
		SpringArm->SocketOffset.Z+=75.f;
	}
	else
	{
		Crouch();
		SpringArm->SocketOffset.Z-=75.f;
	}
}

void ABlasterCharacter::AimButtonPressed()
{
	if(bDisableGameplay) return;
	if(CombatComponent)
	{
		CombatComponent->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased()
{
	if(bDisableGameplay) return;
	if(CombatComponent)
	{
		CombatComponent->SetAiming(false);
	}
}

void ABlasterCharacter::GrenadeButtonPressed()
{
	if(CombatComponent)
	{
		CombatComponent->ThrowGrenade();
	}
}

void ABlasterCharacter::ReloadButtonPressed()
{
	if(bDisableGameplay) return;
	if(CombatComponent)
	{
		CombatComponent->ReloadWeapon();
	}
}

void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if(CombatComponent && CombatComponent->EquippedWeapon == nullptr || !CombatComponent) return;
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	const float Speed = Velocity.Size();
	const bool bIsInAir = GetCharacterMovement()->IsFalling();

	if(Speed == 0.f && !bIsInAir)
	{
		
		const FRotator CurrentAimRotation = {0.f, GetBaseAimRotation().Yaw, 0.f};
		const FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(StartingAimRotation, CurrentAimRotation);
		SetAO_Yaw(DeltaAimRotation.Yaw);
		if(TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}		
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if(Speed > 0.f || bIsInAir)
	{
		StartingAimRotation = {0.f, GetBaseAimRotation().Yaw, 0.f};
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	AO_Pitch = GetBaseAimRotation().Pitch;

	if(AO_Pitch > 90.f && !IsLocallyControlled())
	{
		const FVector2D InRange(270.f, 360.f);
		const FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	
	if( AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	if( AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	
	if(TurningInPlace!=ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0, DeltaTime, 5.f);
		SetAO_Yaw(InterpAO_Yaw);
		if(FMath::Abs(AO_Yaw) < 25.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = {0.f, GetBaseAimRotation().Yaw, 0.f};
		}
	}
	
}

void ABlasterCharacter::FireButtonPressed()
{
	if(bDisableGameplay) return;
	if(CombatComponent)
	{
		CombatComponent->FireButtonPressed(true);
	}
	
}

void ABlasterCharacter::DropOrDestroyWeapon(AWeapon* Weapon)
{
	if(Weapon == nullptr) return;
	if(Weapon->bDestroyWeapon)
	{
		Weapon->Destroy();
	}
	else
	{

		Weapon->Dropped();
	}
}

void ABlasterCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	if (Controller != nullptr)
	{
		const FVector2D MoveValue = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (MoveValue.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			AddMovementInput(MovementDirection, MoveValue.X);
		}

		if (MoveValue.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			AddMovementInput(MovementDirection, MoveValue.Y);
		}
	}
}

void ABlasterCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	
	if (Controller != nullptr)
	{
		const FVector2D LookValue = InputActionValue.Get<FVector2D>();

		if (LookValue.X != 0.0f)
		{
			Turn(LookValue.X);
		}

		if (LookValue.Y != 0.0f)
		{
			LookUp(LookValue.Y);
		}
	}
}

void ABlasterCharacter::Input_Jump(const FInputActionValue& InputActionValue)
{
	Jump();
}

void ABlasterCharacter::Input_Fire(const FInputActionValue& InputActionValue)
{
	FireButtonPressed();
}

void ABlasterCharacter::FireButtonReleased()
{
	if(bDisableGameplay) return;
	if(CombatComponent)
	{
		CombatComponent->FireButtonPressed(false);
	}
}

void ABlasterCharacter::HideCameraForFpp()
{
	if(!IsLocallyControlled()) return;
	if((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->GetChildComponent(1)->GetChildComponent(2)->SetVisibility(false);
	}
	else
	{
		GetMesh()->GetChildComponent(1)->GetChildComponent(2)->SetVisibility(true);
	}
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if(LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void ABlasterCharacter::OnRep_Health(float LastHealth)
{
	UpdateHudHealth();

	if(Health<LastHealth)
	{
		PlayOnHitMontage();
	}
	
}

void ABlasterCharacter::OnRep_Shield(float LastShield)
{
	UpdateHudShields();

	if(Shield<LastShield)
	{
		PlayOnHitMontage();
	}
	
}


void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if(CombatComponent)
	{
		if(OverlappingWeapon && OverlappingWeapon != CombatComponent->GetEquippedWeapon())
		{
			CombatComponent->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			if(CombatComponent->ShouldSwapWeapons())
			{
				CombatComponent->SwapWeapons();
				
			}
		}
		
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	
	if(OverlappingWeapon != nullptr)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	
	OverlappingWeapon = Weapon;
	
	if(IsLocallyControlled())
	{
		if(OverlappingWeapon != nullptr)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool ABlasterCharacter::IsWeaponEquipped() const
{
	return (CombatComponent && CombatComponent->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming() const
{
	return(CombatComponent && CombatComponent->bAiming);
}

void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if(CombatComponent == nullptr || CombatComponent->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		bAiming ? SectionName = FName("RifleSights") : SectionName = FName("RifleHip") ;
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && OnElimMontage)
	{
		AnimInstance->Montage_Play(OnElimMontage);
		AnimInstance->Montage_JumpToSection(FName("Elim"));
	}
}

void ABlasterCharacter::PlayReloadMontage()
{
	if(CombatComponent == nullptr || CombatComponent->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;
		switch (GetEquippedWeapon()->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle :
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_Pistol :
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_SubMachineGun :
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_Shotgun :
			SectionName = FName("Shotgun");
			break;
		case EWeaponType::EWT_SniperRifle :
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_GrenadeLauncher :
			SectionName = FName("Shotgun");
			break;
		default: ;
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayThrowGrenadeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && ThrowGrenadeMontage)
	{
		AnimInstance->Montage_Play(ThrowGrenadeMontage);
		AnimInstance->Montage_JumpToSection(FName("Throw"));
	}
}

void ABlasterCharacter::PlaySwapWeaponMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && SwapWeaponMontage)
	{
		AnimInstance->Montage_Play(SwapWeaponMontage);
		AnimInstance->Montage_JumpToSection(FName("Swap"));
	}
}

void ABlasterCharacter::PlayOnHitMontage()
{
	if(CombatComponent == nullptr || CombatComponent->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && OnHitMontage)
	{
		AnimInstance->Montage_Play(OnHitMontage);
		const int32 SwitchSection = FMath::RandRange(0,3);
		FName SectionName{};
		switch (SwitchSection)
		{
		case 0:
			SectionName ="FromRight";
			break;
		case 1:
			SectionName ="FromLeft";
			break;
		case 2:
			SectionName ="FromFront";
			break;
		case 3:
			SectionName ="FromBack";
			break;
		default: ;
		}
	
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatorController, AActor* DamageCauser)
{
	if(bElimmed) return;

	float DamageToReceiveToHealth = Damage;

	if(Shield > 0)
	{
		if(Shield >= Damage)
		{
			Shield = FMath::Clamp(Shield - Damage, 0.f, MaxShield);
			DamageToReceiveToHealth = 0.f ;
		}
		else
		{
			Shield = 0.f ;
			DamageToReceiveToHealth = FMath::Clamp(DamageToReceiveToHealth - Shield, 0.f, Damage);
		}
	}
	
	Health=FMath::Clamp(Health-DamageToReceiveToHealth,0.f, MaxHealth);
	UpdateHudHealth();
	UpdateHudShields();
	PlayOnHitMontage();

	if(Health==0.f)
	{
		ABlasterGameMode* GameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
		if(GameMode)
		{
			if(BlasterPlayerController == nullptr) BlasterPlayerController=Cast<ABlasterPlayerController>(Controller);
			if(BlasterPlayerController)
			{
				ABlasterPlayerController* InsigatingPC = Cast<ABlasterPlayerController>(InstigatorController);
				if(InsigatingPC)
				{
					GameMode->PlayerEliminated(this, BlasterPlayerController, InsigatingPC);
				}
			}
			else
			{
				ABlasterPlayerController* InsigatingPC = Cast<ABlasterPlayerController>(InstigatorController);
				if(InsigatingPC)
				{
					GameMode->PlayerEliminated(this, nullptr, InsigatingPC);
				}
			}
		}
	}
}

void ABlasterCharacter::UpdateHudHealth()
{
	if(BlasterPlayerController == nullptr)
	{
		BlasterPlayerController = Cast<ABlasterPlayerController>(Controller);
	}
	
	if(BlasterPlayerController)
	{
		BlasterPlayerController->SetHudHealth(Health, MaxHealth);
	}
		
}

void ABlasterCharacter::UpdateHudShields()
{
	if(BlasterPlayerController == nullptr)
	{
		BlasterPlayerController = Cast<ABlasterPlayerController>(Controller);
	}
	
	if(BlasterPlayerController)
	{
		BlasterPlayerController->SetHudShields(Shield, MaxShield);
	}
		
}

void ABlasterCharacter::UpdateHudAmmo()
{
	
	if(BlasterPlayerController == nullptr)
	{
		BlasterPlayerController = Cast<ABlasterPlayerController>(Controller);
	}
	if(BlasterPlayerController && CombatComponent && CombatComponent->EquippedWeapon)
	{
		BlasterPlayerController->SetHudCarriedAmmo(CombatComponent->CarriedAmmo);
		BlasterPlayerController->SetHudWeaponAmmo(CombatComponent->EquippedWeapon->GetAmmo());
	}
	
}


void ABlasterCharacter::SpawnDefaultWeapon()
{
	ABlasterGameMode* BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	UWorld* World = GetWorld();
	if(BlasterGameMode && World && !bElimmed && DefaultWeaponClass)
	{
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->bDestroyWeapon = true;
		StartingWeapon->ShowPickupWidget(false);
		StartingWeapon->GetAreaSphere()->SetCollisionResponseToAllChannels(ECR_Ignore);
		StartingWeapon->EnableCustomDepth(false);
		StartingWeapon->SetOwner(this);
		StartingWeapon->SetHUDWeaponAmmo(StartingWeapon->GetAmmo());
		if(CombatComponent)
		{
			CombatComponent->EquipPrimaryWeapon(StartingWeapon);
		}
	
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = true;
		
	}
}

