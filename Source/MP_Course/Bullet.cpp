// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

#include "BlasterCharacter.h"
#include "BlasterPlayerController.h"
#include "LagCompensationComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ABullet::ABullet()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity=true;
	bReplicates=true;
	
	ProjectileMovementComponent->InitialSpeed = BulletSpeed;
	ProjectileMovementComponent->MaxSpeed = BulletSpeed;
	
}

#if WITH_EDITOR
void ABullet::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	FName PropertyName;
	
	if(PropertyChangedEvent.Property != nullptr)
	{
		PropertyName = PropertyChangedEvent.Property->GetFName();
	}
	else
	{
		PropertyName = NAME_None;
	}

	if(PropertyName == GET_MEMBER_NAME_CHECKED(AProjectile, BulletSpeed))
	{
		if(ProjectileMovementComponent)
		{
			ProjectileMovementComponent->InitialSpeed = BulletSpeed;
			ProjectileMovementComponent->MaxSpeed = BulletSpeed;
		}
	}
}
#endif

void ABullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                    FVector NormalImpulse, const FHitResult& Hit)
{

	ABlasterCharacter* OwnerMan = Cast<ABlasterCharacter>(GetOwner());
	if(OwnerMan)
	{
		ABlasterPlayerController* OwnerController = Cast<ABlasterPlayerController>(OwnerMan->Controller);
		bool bCauseAuthDamage = !bUseServerSideRewind || OwnerMan->IsLocallyControlled();
		if(GetOwner()->HasAuthority() && bCauseAuthDamage)
			{
				float DamageToCause = Hit.BoneName.ToString() == FString("head") ? HeadshotDamage : Damage;
			
				if(OtherActor)
				{
					Multicast_OnHit(OtherActor, DamageToCause);
				}
				Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
				return;
			}
		ABlasterCharacter* HitCharacter = Cast<ABlasterCharacter>(OtherActor);
		if(HitCharacter && bUseServerSideRewind && OwnerMan->GetLagCompensationComponent() && OwnerMan->IsLocallyControlled() && OwnerController)
		{
			OwnerMan->GetLagCompensationComponent()->Server_ProjectileScoreRequest(HitCharacter, TraceStart, InitialVelocity, OwnerController->GetServerTime() + OwnerController->SingleTripTime);
		}
	}
	
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	
	
	
}

void ABullet::Multicast_OnHit(AActor* OtherActor, float DamageToCause)
{
	if(OtherActor)
	{
		UGameplayStatics::ApplyDamage(OtherActor, DamageToCause, GetOwner()->GetInstigatorController(), this, UDamageType::StaticClass());
	}
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();

	// FPredictProjectilePathParams PathParams;
	//
	// PathParams.bTraceWithChannel = true;
	// PathParams.bTraceWithCollision = true;
	// PathParams.DrawDebugTime = 5.f;
	// PathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
	// PathParams.LaunchVelocity = GetActorForwardVector() * BulletSpeed;
	// PathParams.MaxSimTime = 4.f;
	// PathParams.ProjectileRadius = 5.f;
	// PathParams.SimFrequency =30.f;
	// PathParams.StartLocation = GetActorLocation();
	// PathParams.TraceChannel = ECC_Visibility;
	// PathParams.ActorsToIgnore.Add(this);
	
}
