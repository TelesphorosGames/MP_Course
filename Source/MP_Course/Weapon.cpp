// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "BlasterCharacter.h"
#include "BlasterPlayerController.h"
#include "Casing.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "CombatComponent.h"
#include "Kismet/KismetMathLibrary.h"


AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);
	
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	
	
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Pickup Widget"));
	PickupWidget->SetupAttachment(RootComponent);


	WeaponMesh->bAlwaysCreatePhysicsState = true;

	
}


void AWeapon::BeginPlay()
{
	Super::BeginPlay();

		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnAreaSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnAreaSphereEndOverlap);
	
	if(PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
	if(BlasterOwnerCharacter == nullptr)
	{
		BlasterOwnerCharacter = Cast<ABlasterCharacter>(GetOwner());
	}
}

void AWeapon::Tick(float DeltaTime)
 {
 	Super::Tick(DeltaTime);
 
 }

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME(AWeapon, Ammo);

	
	
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();

	if(Owner==nullptr)
	{
		BlasterOwnerCharacter=nullptr;
		BlasterPlayerController=nullptr;
	}
	else
	{
		if(BlasterOwnerCharacter == nullptr)
		{
			BlasterOwnerCharacter = Cast<ABlasterCharacter>(Owner);
		}
		if(BlasterOwnerCharacter && BlasterOwnerCharacter->GetEquippedWeapon() && BlasterOwnerCharacter->GetEquippedWeapon() == this)
		{
			SetHUDWeaponAmmo();
		}
	}
}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State ;
	OnWeaponStateSet();
}

void AWeapon::OnEquipped()
{
	ShowPickupWidget(false);

	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	
	WeaponMesh->SetCustomDepthStencilValue(0);
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(false);

	
	if(WeaponType == EWeaponType::EWT_SubMachineGun)
	{
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}
}

void AWeapon::OnDropped()
{
	if(HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);

}
void AWeapon::OnEquippedSecondary()
{
	OnEquipped();
}

void AWeapon::OnWeaponStateSet()
{
	switch(WeaponState)
	{
	case EWeaponState::EWS_Equipped :
		
		OnEquipped();
		
		break;
		
	case EWeaponState::EWS_Dropped :
		
		OnDropped();
		
		break;

	case EWeaponState::EWS_EquippedSecondary :

		OnEquipped();

		break;
		
	default:
		
		break;
	}
}


void AWeapon::OnRep_WeaponState()
{
	OnWeaponStateSet();
}


void AWeapon::OnAreaSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if(BlasterCharacter)
	{
		if(BlasterCharacter->GetEquippedWeapon() != this)
		{
			BlasterCharacter->SetOverlappingWeapon(this);
		}
	}
}

void AWeapon::OnAreaSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if(BlasterCharacter && BlasterCharacter->GetOverlappingWeapon() == this)
	{
		BlasterCharacter->SetOverlappingWeapon(nullptr);
	}
	
}

void AWeapon::Dropped()
{
	
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	SetWeaponState(EWeaponState::EWS_Dropped);
	SetOwner(nullptr);
	BlasterOwnerCharacter = nullptr;
	BlasterPlayerController = nullptr;
	
	EnableCustomDepth(true);
	
	
	
}


void AWeapon::OnRep_Ammo()
{

	if(BlasterOwnerCharacter &&
		BlasterOwnerCharacter->GetCombatComponent() &&
		IsFull() &&
		WeaponType == EWeaponType::EWT_Shotgun)
	{
		BlasterOwnerCharacter->GetCombatComponent()->JumpToShotgunEnd();
	}
	SetHUDWeaponAmmo();
	
}

void AWeapon::SetHUDWeaponAmmo()
{
	if(BlasterOwnerCharacter == nullptr)
	{
		BlasterOwnerCharacter = Cast<ABlasterCharacter>(GetOwner());
	}
	if(BlasterOwnerCharacter)
	{
		if(BlasterPlayerController == nullptr)
        {
            BlasterPlayerController = Cast<ABlasterPlayerController>(BlasterOwnerCharacter->GetController());
        }
        if(BlasterPlayerController)
        {
            BlasterPlayerController->SetHudWeaponAmmo(Ammo);
        }
	}
}

void AWeapon::SetHUDWeaponAmmo(int32 AmmoAmount)
{
	if(BlasterOwnerCharacter == nullptr)
	{
		BlasterOwnerCharacter = Cast<ABlasterCharacter>(GetOwner());
	}
	if(BlasterOwnerCharacter)
	{
		if(BlasterPlayerController == nullptr)
		{
			BlasterPlayerController = Cast<ABlasterPlayerController>(BlasterOwnerCharacter->GetController());
		}
		if(BlasterPlayerController)
		{
			BlasterPlayerController->SetHudWeaponAmmo(AmmoAmount);
		}
	}
}

bool AWeapon::IsEmpty()
{
	if(Ammo <=0)
	{
		return true;
	}
	return false;
}

bool AWeapon::IsFull()
{
	return (Ammo == MagCapacity);
}

void AWeapon::AddAmmo(int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo-AmmoToAdd, 0 , MagCapacity);
	SetHUDWeaponAmmo();
}

void AWeapon::EnableCustomDepth(bool bEnable)
{
	if(WeaponMesh != nullptr)
	{
		WeaponMesh->bRenderCustomDepth = bEnable;
	}
}

void AWeapon::SpendRound()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	
	SetHUDWeaponAmmo();
}

void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if(PickupWidget) 
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if(FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	if(CasingClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket = GetWeaponMesh()->GetSocketByName(FName("AmmoEject"));
		if(AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(GetWeaponMesh());

				
				UWorld* World = GetWorld();
				if(World)
				{
						World->SpawnActor<ACasing>(
						CasingClass,
						SocketTransform.GetLocation(),
						SocketTransform.GetRotation().Rotator()
						);
				}
		}
		
	}
	if(HasAuthority())
	{
		SpendRound();
	}


}




FVector AWeapon::TraceEndWithScatter(const FVector& HitTarget)
{

	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if(MuzzleSocket == nullptr)
	{
		return FVector();
	}
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
	const FVector Start = SocketTransform.GetLocation();
	
	// Pointing from the trace start location to the hit target
	const FVector ToTargetNormalized = (HitTarget - Start).GetSafeNormal();

	const FVector SphereCenterLocation = Start + (ToTargetNormalized * DistanceToSphere);


	const FVector RandomVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
	const FVector Endloc = SphereCenterLocation + RandomVector;

	const FVector ToEndLoc = Endloc - Start ;
	
	
	DrawDebugSphere(GetWorld(), SphereCenterLocation, SphereRadius, 12, FColor::Red, true);
	DrawDebugSphere(GetWorld(), Endloc, 4.f, 12, FColor::Orange, true);
	DrawDebugLine(GetWorld(), Start, FVector(Start + ToEndLoc * 10), FColor::Cyan, true );
	
	
	return  FVector(Start + ToEndLoc * 10);
	
}
