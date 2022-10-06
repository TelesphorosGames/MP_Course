// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "BlasterCharacter.h"
#include "BlasterPlayerController.h"
#include "Casing.h"
#include "../../Plugins/Developer/RiderLink/Source/RD/thirdparty/spdlog/include/spdlog/fmt/bundled/chrono.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"


AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(WeaponMesh);
	
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	
	
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Pickup Widget"));
	PickupWidget->SetupAttachment(RootComponent);

	
}


void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnAreaSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnAreaSphereEndOverlap);
	}
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
	SetHUDWeaponAmmo();

	
}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State ;

	switch(WeaponState)
	{
	case EWeaponState::EWS_Equipped :

		ShowPickupWidget(false);

		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		break;
		
	case EWeaponState::EWS_Dropped :
		
		if(HasAuthority())
		{
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
		break;
	default: ;
	}
	
}

void AWeapon::OnAreaSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if(BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(this);
	}
	
}

void AWeapon::OnAreaSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if(BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(nullptr);
	}
	
}

void AWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules FDR(EDetachmentRule::KeepRelative, true);
	WeaponMesh->DetachFromComponent(FDR);
	SetOwner(nullptr);
	BlasterOwnerCharacter = nullptr;
	BlasterPlayerController = nullptr;
	
}

void AWeapon::OnRep_WeaponState()
{
	switch(WeaponState)
	{
	case EWeaponState::EWS_Equipped :

		ShowPickupWidget(false);
		
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		break;

	case EWeaponState::EWS_Dropped :
		
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		
		break;
		
	default: ;
	}
}

void AWeapon::OnRep_Ammo()
{
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

bool AWeapon::IsEmpty()
{
	if(Ammo <=0)
	{
		return true;
	}
	return false;
}

void AWeapon::AddAmmo(int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo-AmmoToAdd, 0 , MagCapacity);
	SetHUDWeaponAmmo();
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
	SpendRound();

}

