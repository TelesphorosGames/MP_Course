// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"


#include "BlasterCharacter.h"
#include "Casing.h"
#include "Engine/SkeletalMeshSocket.h"
// #include "DrawDebugHelpers.h"
// #include "Kismet/GameplayStatics.h"
// #include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "MP_Course.h"

void AShotgun::Fire(const FVector& HitTarget)
{
	AWeapon::Fire(HitTarget);
	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if(MuzzleSocket)
	{
		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		
		TMap<ABlasterCharacter*, uint32> HitMap;
		
		for (uint32 i=0; i<NumberOfPellets; i++)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);
			
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			if(BlasterCharacter && HasAuthority())
			{
				if(HitMap.Contains(BlasterCharacter))
				{
					HitMap[BlasterCharacter]++;
				}
				else
				{
					HitMap.Emplace(BlasterCharacter, 1);
				}
			}
			if(HitScanImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitScanImpactParticles, FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
			}
			if(HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this,HitSound,FireHit.ImpactPoint, .5f, FMath::FRandRange(-.5f, .5f));
			}
		}

		for (auto Hitpair : HitMap)
		{
				if(Hitpair.Key)
				{
					UGameplayStatics::ApplyDamage(Hitpair.Key, Damage * Hitpair.Value, GetOwner()->GetInstigatorController(), this, UDamageType::StaticClass());
				}
			
		}

		
		
	}
	
}
