// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"

#include "BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"


void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if(MuzzleSocket)
	{
		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		

		FHitResult FireHit;
		WeaponTraceHit(Start, HitTarget, FireHit);

		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
		if(BlasterCharacter)
		{
			if(HasAuthority())
			{
				UGameplayStatics::ApplyDamage(BlasterCharacter, Damage, GetOwner()->GetInstigatorController(), this, UDamageType::StaticClass());
			}
		}
		if(HitScanImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitScanImpactParticles, FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
		}
                 
		if(HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this,HitSound,FireHit.ImpactPoint);
		}
	
		
		if(MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}
		if(FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation(), GetActorRotation());
		}
	}
	
}

FVector AHitScanWeapon::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget)
{
	// Pointing from the trace start location to the hit target
	FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();

	FVector SphereCenterLocation = TraceStart + (ToTargetNormalized * DistanceToSphere);


	FVector RandomVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
	FVector Endloc = SphereCenterLocation + RandomVector;

	FVector ToEndLoc = Endloc - TraceStart ;
	
	DrawDebugSphere(GetWorld(), SphereCenterLocation, SphereRadius, 12, FColor::Red, true);
	DrawDebugSphere(GetWorld(), Endloc, 4.f, 12, FColor::Orange, true);
	DrawDebugLine(GetWorld(), TraceStart, FVector(TraceStart + ToEndLoc * 10), FColor::Cyan, true );
	
	
	return  FVector(TraceStart + ToEndLoc * 10);
	
}

void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	
	UWorld* World = GetWorld();
	if (World)
	{
		const FVector End = bUseScatter ? TraceEndWithScatter(TraceStart, HitTarget) : TraceStart + (HitTarget - TraceStart) * 1.25;
		
		World->LineTraceSingleByChannel(
			OutHit,
			TraceStart,
			End, 
			ECollisionChannel::ECC_Visibility
			);

		FVector BeamEnd = End;
		
		if(OutHit.bBlockingHit)
		{
			BeamEnd=OutHit.ImpactPoint;
		}
		if(BeamParticles)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(World, BeamParticles, TraceStart, FRotator::ZeroRotator, true);
			if(Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}
}
