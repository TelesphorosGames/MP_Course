// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"


#include "BlasterCharacter.h"
#include "Casing.h"
#include "Engine/SkeletalMeshSocket.h"
// #include "DrawDebugHelpers.h"
// #include "Kismet/GameplayStatics.h"
// #include "Particles/ParticleSystemComponent.h"
#include "BlasterPlayerController.h"
#include "LagCompensationComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "MP_Course.h"
#include "Kismet/KismetMathLibrary.h"

void AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if(MuzzleSocket == nullptr)
	{
		return;
	}
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
	const FVector Start = SocketTransform.GetLocation();
	
	// Pointing from the trace start location to the hit target ( Normalized ( 0-1 )) 
	const FVector ToTargetNormalized = (HitTarget - Start).GetSafeNormal();
	const FVector SphereCenterLocation = Start + (ToTargetNormalized * DistanceToSphere);
	
	for(uint32 i=0; i<NumberOfPellets; i++)
	{
		const FVector RandomVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		const FVector Endloc = SphereCenterLocation + RandomVector;
		FVector ToEndLoc = Endloc - Start;
		FVector_NetQuantize FVNQToEndLoc = {Start + ToEndLoc * 10};
		HitTargets.Add(TraceEndWithScatter(FVNQToEndLoc));
	}
}

void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(FVector());
	
	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if(MuzzleSocket)
	{
		const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
		const FVector Start = SocketTransform.GetLocation();
		TMap<ABlasterCharacter*, uint32> HitMap;

		for(FVector_NetQuantize HitTarget : HitTargets)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);
			
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			if(BlasterCharacter)
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
		TArray<ABlasterCharacter*> HitCharacters;
		bool bCauseAuthDamage = !bUseServerSideRewind || BlasterOwnerCharacter->IsLocallyControlled();
		
		for (auto Hitpair : HitMap)
		{
			
			if(Hitpair.Key && HasAuthority() && bCauseAuthDamage)
			{
				UGameplayStatics::ApplyDamage(Hitpair.Key, Damage * Hitpair.Value, GetOwner()->GetInstigatorController(), this, UDamageType::StaticClass());
			}
			HitCharacters.Add(Hitpair.Key);
		}
		if(!HasAuthority() && bUseServerSideRewind)
		{
			if(BlasterOwnerCharacter && BlasterPlayerController && BlasterOwnerCharacter->GetLagCompensationComponent() && BlasterOwnerCharacter->IsLocallyControlled())
			{
				BlasterOwnerCharacter->GetLagCompensationComponent()->Server_ShotgunScoreRequest(HitCharacters, Start, HitTargets, BlasterPlayerController->GetServerTime() + BlasterPlayerController->SingleTripTime);
			}
		}
	}
}
/*// void AShotgun::Fire(const FVector& HitTarget)
// {
// 	AWeapon::Fire(HitTarget);
// 	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
// 	if(MuzzleSocket)
// 	{
// 		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
// 		FVector Start = SocketTransform.GetLocation();
// 		
// 		TMap<ABlasterCharacter*, uint32> HitMap;
// 		
// 		for (uint32 i=0; i<NumberOfPellets; i++)
// 		{
// 			FHitResult FireHit;
// 			WeaponTraceHit(Start, HitTarget, FireHit);
// 			
// 			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
// 			if(BlasterCharacter && HasAuthority())
// 			{
// 				if(HitMap.Contains(BlasterCharacter))
// 				{
// 					HitMap[BlasterCharacter]++;
// 				}
// 				else
// 				{
// 					HitMap.Emplace(BlasterCharacter, 1);
// 				}
// 			}
// 			if(HitScanImpactParticles)
// 			{
// 				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitScanImpactParticles, FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
// 			}
// 			if(HitSound)
// 			{
// 				UGameplayStatics::PlaySoundAtLocation(this,HitSound,FireHit.ImpactPoint, .5f, FMath::FRandRange(-.5f, .5f));
// 			}
// 		}
//
// 		for (auto Hitpair : HitMap)
// 		{
// 				if(Hitpair.Key && HasAuthority())
// 				{
// 					UGameplayStatics::ApplyDamage(Hitpair.Key, Damage * Hitpair.Value, GetOwner()->GetInstigatorController(), this, UDamageType::StaticClass());
// 				}
// 			
// 		}
//
// 		
// 		
// 	}
// 	
// } 
*/
