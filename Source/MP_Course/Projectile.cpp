// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

// #include "BlasterCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "MP_Course.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECR_Block);
	

	
	
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
		
	}
	

	if(Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(
			Tracer,
			CollisionBox,
			FName(""),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition
		);
	}
	
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{

	
	if(HasAuthority())
	{
		Multicast_OnHit(OtherComp->GetCollisionObjectType());
	}
	Destroy();
}

void AProjectile::Multicast_OnHit(AActor* OtherActor)
{
}

void AProjectile::Multicast_OnHit_Implementation(ECollisionChannel CollisionChannel)
{
	if(ImpactParticles && HitPlayerParticles)
    {
		if(CollisionChannel == ECC_SkeletalMesh)
		{
			Debug("EC_SkeletalMesh");
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitPlayerParticles, GetActorTransform());
		}
		else
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
		}
    	
    }
	
    if(ImpactSound)
    {
    	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
    }
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::Destroyed()
{
	Super::Destroyed();
	

	
}

