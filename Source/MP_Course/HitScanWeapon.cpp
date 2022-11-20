// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"

#include "BlasterCharacter.h"
#include "BlasterPlayerController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"
#include "LagCompensationComponent.h"
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
			if(HasAuthority() && !bUseServerSideRewind)
			{
				UGameplayStatics::ApplyDamage(BlasterCharacter, Damage, GetOwner()->GetInstigatorController(), this, UDamageType::StaticClass());
			}
			if(!HasAuthority() && bUseServerSideRewind)
			{
				if(BlasterOwnerCharacter && BlasterPlayerController && BlasterOwnerCharacter->GetLagCompensationComponent())
				{
					BlasterOwnerCharacter->GetLagCompensationComponent()->Server_ScoreRequest(BlasterCharacter, Start, FireHit.ImpactPoint, BlasterPlayerController->GetServerTime() - BlasterPlayerController->SingleTripTime, this);
				}
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

void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	
	UWorld* World = GetWorld();
	if (World)
	{
		const FVector End = TraceStart + (HitTarget - TraceStart) * 1.25;
		
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

		DrawDebugSphere(GetWorld(), BeamEnd, 16.f, 12, FColor::Magenta, true);

		
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
