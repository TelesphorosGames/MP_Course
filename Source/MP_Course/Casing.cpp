// Fill out your copyright notice in the Description page of Project Settings.


#include "Casing.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ACasing::ACasing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);

	CasingMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CasingMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CasingMesh->SetNotifyRigidBodyCollision(true);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	CasingMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	
	
	CasingVelocity = {5.f};

	

}

void ACasing::BeginPlay()
{
	Super::BeginPlay();

	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);		


	const float RandomFloat = FMath::RandRange(0,360);
	const FRotator RandomRotation = {FRotator(RandomFloat, RandomFloat, RandomFloat)};
	SetActorRotation(RandomRotation);
	
	CasingMesh->AddImpulse((GetActorForwardVector()* CasingVelocity));

	
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	
	if(ShellSound && bSoundPlayed==false)
		{
			bSoundPlayed=true;
			UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
		}	
	
}


void ACasing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

