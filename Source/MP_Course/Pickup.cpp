// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "WeaponTypes.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	OverlapShere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapShere->SetupAttachment(RootComponent);
	OverlapShere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapShere->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapShere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapShere->SetSphereRadius(150.f);
	
	PickupMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(RootComponent);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupMesh->SetRenderCustomDepth(true);
	PickupMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_PURPLE);
	
	PickupEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PickupEffectComponent"));
	PickupEffectComponent->SetupAttachment(RootComponent);
	
	
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
	{
		OverlapShere->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnSphereOverlap);
	}
	
	
}

void APickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(PickupMesh)
	{
		PickupMesh->AddLocalRotation(FRotator(0.f, BaseTurnRate * DeltaTime, 0.f));
	}

}


void APickup::Destroyed()
{
	Super::Destroyed();

	if(PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}
	
	if(PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickupEffect, GetActorLocation(), GetActorRotation());
	}
	
	
}

