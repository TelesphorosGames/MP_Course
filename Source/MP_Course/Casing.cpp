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
	
	
	
	CasingVelocity = {50.f};

	

}

void ACasing::BeginPlay()
{
	Super::BeginPlay();

	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);		
	

	FVector CasingEjectionDirection = GetActorForwardVector();
		CasingEjectionDirection.X += FMath::RandRange(-3.f,3.f);
	
	CasingMesh->AddTorqueInRadians(GetActorRightVector() * 100'000);
	
	CasingMesh->AddImpulse((CasingEjectionDirection * CasingVelocity));
	
	
	
	
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	
	if(ShellSound && !bSoundPlayed)
		{
			bSoundPlayed=true;
			UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
		}
	
	SetLifeSpan(2.f);
}


void ACasing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

