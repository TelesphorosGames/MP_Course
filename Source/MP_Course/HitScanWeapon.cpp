// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"

#include "BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"


void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if(MuzzleSocket)
	{
		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		FVector End = Start + (HitTarget - Start) * 1.25;

		FHitResult FireHit;
		
		UWorld* World = GetWorld();
		if(World)
		{
			World->LineTraceSingleByChannel(FireHit, Start, End, ECollisionChannel::ECC_Visibility);
			FVector BeamEnd = End;
			if(FireHit.bBlockingHit)
			{
				BeamEnd=FireHit.ImpactPoint;
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
                 						UGameplayStatics::SpawnEmitterAtLocation(World, HitScanImpactParticles, FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
                 					}
                 
                 					if(HitSound)
                 					{
                 						UGameplayStatics::PlaySoundAtLocation(this,HitSound,FireHit.ImpactPoint);
                 						
                 					}
			}

			if(BeamParticles)
			{
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(World, BeamParticles, SocketTransform);
				if(Beam)
				{
					Beam->SetVectorParameter(FName("Target"), BeamEnd);
				}
			}
		}
		if(MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(World, MuzzleFlash, SocketTransform);
		}
		if(FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation(), GetActorRotation());
		}
		
		
	}
	
}
