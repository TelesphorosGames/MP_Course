// Fill out your copyright notice in the Description page of Project Settings.


#include "LagCompensationComponent.h"

#include "BlasterCharacter.h"
#include "BlasterPlayerController.h"
#include "Weapon.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "MP_Course.h"


ULagCompensationComponent::ULagCompensationComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}

void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();

	FFramePackage FramePackage;
	SaveFramePackage(FramePackage);

	// ShowFramePackage(FramePackage, FColor::Yellow);

	
}


FFramePackage ULagCompensationComponent::InterpBetweenFrames(const FFramePackage& OlderFrame,
	const FFramePackage& YoungerFrame, float HitTime)
{
	const float Distance = YoungerFrame.Time - OlderFrame.Time;
	const float InterpFraction = FMath::Clamp((HitTime - OlderFrame.Time) / Distance, 0, 1);

	FFramePackage InterpFramePackage;
	InterpFramePackage.Time = HitTime;

	for(auto& YoungerPair : YoungerFrame.HitBoxInfo)
	{
		const FName& BoxInfoName = YoungerPair.Key;

		const FBoxInformation& OlderBox = OlderFrame.HitBoxInfo[BoxInfoName];
		const FBoxInformation& YoungerBox = YoungerFrame.HitBoxInfo[BoxInfoName];


		FBoxInformation InterpBoxInfo;

		InterpBoxInfo.Location = FMath::VInterpTo(OlderBox.Location, YoungerBox.Location, 1, InterpFraction);
		InterpBoxInfo.Rotation = FMath::RInterpTo(OlderBox.Rotation, YoungerBox.Rotation, 1, InterpFraction);
		InterpBoxInfo.Extent = YoungerBox.Extent;

		InterpFramePackage.HitBoxInfo.Add(BoxInfoName, InterpBoxInfo);
	}

	
	return InterpFramePackage;
}

FServerSideRewindResult ULagCompensationComponent::ConfirmHit(const FFramePackage& Package,
	ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation)
{
	if(HitCharacter == nullptr) return FServerSideRewindResult();

	
	FFramePackage CurrentFrame;
	CacheBoxPositions(HitCharacter, CurrentFrame);

	ShowFramePackage(CurrentFrame, FColor::Yellow);
	
	MoveBoxes(HitCharacter, Package);

	ShowFramePackage(Package, FColor::Red);
	
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::NoCollision);
	
	UBoxComponent* HeadshotBox = HitCharacter->HitCollisionBoxes["head"];
	HeadshotBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HeadshotBox->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);

	const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;
	UWorld* World = GetWorld();
	if(World)
	{
		FHitResult ConfirmedHitResult;
		World->LineTraceSingleByChannel(ConfirmedHitResult, TraceStart, TraceEnd, ECC_HitBox);
		if(ConfirmedHitResult.GetComponent() == HeadshotBox)
		{
			ResetHitBoxes(HitCharacter, CurrentFrame);
			EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
			return FServerSideRewindResult{true, true};
		}
		
		
		World->LineTraceSingleByChannel(ConfirmedHitResult, TraceStart, TraceEnd, ECC_HitBox);
		
		if(ConfirmedHitResult.GetActor() == HitCharacter)
		{
			ResetHitBoxes(HitCharacter, CurrentFrame);
			EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
			return FServerSideRewindResult{true, false};
		}
		
	}

	ResetHitBoxes(HitCharacter, CurrentFrame);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
	
	return FServerSideRewindResult{false, false};
}

FShotgunServerSideRewindResult ULagCompensationComponent::ShotgunConfirmHit(const TArray<FFramePackage>& FramePackages,
	const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& HitLocations)
{
	for(auto& Frame : FramePackages)
	{
		if(Frame.Character == nullptr) return FShotgunServerSideRewindResult();
	}
	FShotgunServerSideRewindResult ShotgunResult;

	TArray<FFramePackage> CurrentFrames;
	
	for(auto& Frame : FramePackages)
	{
		FFramePackage CurrentFrame;
		CurrentFrame.Character = Frame.Character;
		CacheBoxPositions(Frame.Character, CurrentFrame);
		ShowFramePackage(CurrentFrame, FColor::Yellow);
		MoveBoxes(Frame.Character, Frame);
		ShowFramePackage(Frame, FColor::Red);
		EnableCharacterMeshCollision(Frame.Character, ECollisionEnabled::NoCollision);
		CurrentFrames.Add(CurrentFrame);

	}
	
	for(auto& Frame : FramePackages)
	{
	    UBoxComponent* HeadBox = Frame.Character->HitCollisionBoxes[FName("head")];
		HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		HeadBox->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
	}
	UWorld* World = GetWorld();
	for(auto& HitLocation : HitLocations)
	{
		FHitResult ConfirmedHitResult;
		const FVector TraceEnd = TraceStart + (HitLocation - TraceStart);
		if(World)
		{
			
			World->LineTraceSingleByChannel(ConfirmedHitResult, TraceStart, TraceEnd, ECC_HitBox);
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(ConfirmedHitResult.GetActor());
			if(BlasterCharacter)
			{
				if(ShotgunResult.Headshots.Contains(BlasterCharacter))
				{
					ShotgunResult.Headshots[BlasterCharacter]++;
				}
				else
				{
					ShotgunResult.Headshots.Emplace(BlasterCharacter, 1);
				}
			}
		}
	}

	for(auto& Frame : FramePackages)
	{
		
		for(auto& Hitboxes : Frame.Character->HitCollisionBoxes)
		{
			if(Hitboxes.Value != nullptr)
			{
				Hitboxes.Value->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				Hitboxes.Value->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);
			}
		}
		UBoxComponent* HeadBox = Frame.Character->HitCollisionBoxes[FName("head")];
		HeadBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	for(auto& HitLocation : HitLocations)
	{
		const FVector TraceEnd = TraceStart + (HitLocation - TraceStart);
		
		if(World)
		{
			FHitResult ConfirmedHitResult;
			World->LineTraceSingleByChannel(ConfirmedHitResult, TraceStart, TraceEnd, ECC_HitBox);
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(ConfirmedHitResult.GetActor());
			if(BlasterCharacter)
			{
				if(ShotgunResult.Bodyshots.Contains(BlasterCharacter))
				{
					ShotgunResult.Bodyshots[BlasterCharacter]++;
				}
				else
				{
					ShotgunResult.Bodyshots.Emplace(BlasterCharacter, 1);
				}
			}
		}
	}

	for(auto& Frame : CurrentFrames)
	{
		ResetHitBoxes(Frame.Character, Frame);
		EnableCharacterMeshCollision(Frame.Character, ECollisionEnabled::QueryAndPhysics);
	}

	return ShotgunResult;
	
}

FServerSideRewindResult ULagCompensationComponent::ProjectileConfirmHit(const FFramePackage& Package, ABlasterCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime)
{
	
	FFramePackage CurrentFrame;
	CacheBoxPositions(HitCharacter, CurrentFrame);

	ShowFramePackage(CurrentFrame, FColor::Yellow);
	
	MoveBoxes(HitCharacter, Package);

	ShowFramePackage(Package, FColor::Red);
	
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::NoCollision);
	
	UBoxComponent* HeadshotBox = HitCharacter->HitCollisionBoxes["head"];
	HeadshotBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HeadshotBox->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);

	FPredictProjectilePathParams PathParams;

	
	PathParams.bTraceWithChannel = true;
	PathParams.bTraceWithCollision = true;
	PathParams.DrawDebugTime = 5.f;
	PathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
	PathParams.LaunchVelocity = InitialVelocity;
	PathParams.MaxSimTime = MaxRecordFrameHistoryTime;
	PathParams.ProjectileRadius = 5.f;
	PathParams.SimFrequency = 15.f;
	PathParams.StartLocation = TraceStart;
	PathParams.TraceChannel = ECC_HitBox;
	PathParams.ActorsToIgnore.Add(GetOwner());
	
	FPredictProjectilePathResult PathResult;

	UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);

	if(PathResult.HitResult.bBlockingHit)
	{
			ResetHitBoxes(HitCharacter, CurrentFrame);
			EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
			return FServerSideRewindResult{true, true};
		
	}
	for(auto& HitBoxes : CurrentFrame.Character->HitCollisionBoxes)
	{
		if(HitBoxes.Value != nullptr)
		{
			HitBoxes.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			HitBoxes.Value->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);
		}
	}
	UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);

	if(PathResult.HitResult.bBlockingHit)
	{
		ResetHitBoxes(HitCharacter, CurrentFrame);
		EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
		return FServerSideRewindResult{true, false};
	}
		
	
	ResetHitBoxes(HitCharacter, CurrentFrame);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
	return FServerSideRewindResult{false, false};

}

void ULagCompensationComponent::CacheBoxPositions(ABlasterCharacter* HitCharacter, FFramePackage& OutFramePackage)
{
	if(HitCharacter == nullptr) return;
		

	for(auto& HitBoxes : HitCharacter->HitCollisionBoxes)
	{
		if(HitBoxes.Value != nullptr)
		{
			FBoxInformation BoxInfo;

			BoxInfo.Location = HitBoxes.Value->GetComponentLocation();
			BoxInfo.Rotation = HitBoxes.Value->GetComponentRotation();
			BoxInfo.Extent = HitBoxes.Value->GetScaledBoxExtent();
			OutFramePackage.HitBoxInfo.Add(HitBoxes.Key, BoxInfo);
			
		}
	}
}

void ULagCompensationComponent::MoveBoxes(ABlasterCharacter* HitCharacter, const FFramePackage& Package)
{
	if(HitCharacter == nullptr) return;
	for(auto& HitBoxes : HitCharacter->HitCollisionBoxes)
	{
		if(HitBoxes.Value != nullptr)
		{
			HitBoxes.Value->SetWorldLocation(Package.HitBoxInfo[HitBoxes.Key].Location);
			HitBoxes.Value->SetWorldRotation(Package.HitBoxInfo[HitBoxes.Key].Rotation);
			HitBoxes.Value->SetBoxExtent(Package.HitBoxInfo[HitBoxes.Key].Extent);
		
			HitBoxes.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			HitBoxes.Value->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);
		
			
		}
	}
}

void ULagCompensationComponent::ResetHitBoxes(ABlasterCharacter* HitCharacter, const FFramePackage& Package)
{
	if(HitCharacter == nullptr) return;
	for(auto& HitBoxes : HitCharacter->HitCollisionBoxes)
	{
		if(HitBoxes.Value != nullptr)
		{
			HitBoxes.Value->SetWorldLocation(Package.HitBoxInfo[HitBoxes.Key].Location);
			HitBoxes.Value->SetWorldRotation(Package.HitBoxInfo[HitBoxes.Key].Rotation);
			HitBoxes.Value->SetBoxExtent(Package.HitBoxInfo[HitBoxes.Key].Extent);
			HitBoxes.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	
}

void ULagCompensationComponent::EnableCharacterMeshCollision(ABlasterCharacter* HitCharacter,
	ECollisionEnabled::Type CollisionEnabled)
{
	if(HitCharacter && HitCharacter->GetMesh())
	{
		HitCharacter->GetMesh()->SetCollisionEnabled(CollisionEnabled);
	}
}

void ULagCompensationComponent::SaveFramePackage()
{
	if(Character == nullptr || !Character->HasAuthority()) return;
	if(FrameHistory.Num() <= 1)
	{
		FFramePackage ThisFramesPackage;
		SaveFramePackage(ThisFramesPackage);
		FrameHistory.AddHead(ThisFramesPackage);		
	}
	else
	{
		float HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		
		while(HistoryLength>MaxRecordFrameHistoryTime)
		{
			FrameHistory.RemoveNode(FrameHistory.GetTail());
			HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		}
		FFramePackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);
		// ShowFramePackage(ThisFrame, FColor::Red);
		
	}
}


void ULagCompensationComponent::SaveFramePackage(FFramePackage& Package)
{
	if(Character)
	{
		Package.Time = GetWorld()->GetTimeSeconds();
		Package.Character = Character;
		for (auto& BoxPair : Character->HitCollisionBoxes)
		{
			FBoxInformation BoxInformation;
			BoxInformation.Location = BoxPair.Value->GetComponentLocation();
			BoxInformation.Rotation = BoxPair.Value->GetComponentRotation();
			BoxInformation.Extent = BoxPair.Value->GetScaledBoxExtent();
			Package.HitBoxInfo.Add(BoxPair.Key, BoxInformation);
		}
	}
}


FFramePackage ULagCompensationComponent::GetFrameToCheck(ABlasterCharacter* HitCharacter, float HitTime)
{
	if(HitCharacter == nullptr ||
		HitCharacter->GetLagCompensationComponent() == nullptr ||
		HitCharacter->GetLagCompensationComponent()->FrameHistory.GetHead() == nullptr ||
		HitCharacter->GetLagCompensationComponent()->FrameHistory.GetTail() == nullptr )
	{
		return FFramePackage();
	}

	FFramePackage FrameToCheck{};
	bool bShouldInterpolate = true;

	const TDoubleLinkedList<FFramePackage>& History = HitCharacter->GetLagCompensationComponent()->FrameHistory;

	const float OldestHistoryTime = History.GetTail()->GetValue().Time;
	if(OldestHistoryTime > HitTime)
	{
		return FFramePackage();
	}
	if(OldestHistoryTime == HitTime)
	{
		bShouldInterpolate = false;
		FrameToCheck = History.GetTail()->GetValue();
	}
	const float NewestHistoryTime = History.GetHead()->GetValue().Time;
	if(NewestHistoryTime <= HitTime)
	{
		
		FrameToCheck = History.GetHead()->GetValue();
		bShouldInterpolate = false;
		 
	}

	float ClientHitTime = HitTime;

	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Younger = History.GetHead();
	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Older = Younger;

	while(Older->GetValue().Time > ClientHitTime)
	{
		if(Older->GetNextNode() == nullptr) break;
		Older = Older->GetNextNode();
		if(Older->GetValue().Time > ClientHitTime)
		{
			Younger = Older;
		}
	}
	if(Older->GetValue().Time == ClientHitTime)
	{
		bShouldInterpolate = false;
		FrameToCheck = Older->GetValue();
	}
	if(bShouldInterpolate)
	{
		FrameToCheck = Younger->GetValue();
		// FrameToCheck = InterpBetweenFrames(Older->GetValue(), Younger->GetValue(), HitTime);
	}

	UE_LOG(LogTemp,Warning,TEXT("Younger->GetValue.Time == %f"), Younger->GetValue().Time);
	UE_LOG(LogTemp,Warning,TEXT("HitTime == %f"), HitTime);
	UE_LOG(LogTemp,Warning,TEXT("ClientHitTime == %f"), ClientHitTime);
	FrameToCheck.Character = HitCharacter;
	
	return  FrameToCheck;
}

void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SaveFramePackage();

}

void ULagCompensationComponent::ShowFramePackage(const FFramePackage& Package, FColor Color)
{
	for(auto& BoxInfo : Package.HitBoxInfo)
	{
		DrawDebugBox(GetWorld(), BoxInfo.Value.Location, BoxInfo.Value.Extent, FQuat(BoxInfo.Value.Rotation), Color, true);
	}
	
}

FServerSideRewindResult ULagCompensationComponent::ServerSideRewind(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart,
	const FVector_NetQuantize& HitLocation, float HitTime)
{
	const FFramePackage FrameToCheck = GetFrameToCheck(HitCharacter, HitTime);
	return ConfirmHit(FrameToCheck, HitCharacter, TraceStart, HitLocation);
	
}

FShotgunServerSideRewindResult ULagCompensationComponent::ShotgunServerSideRewind(
	const TArray<ABlasterCharacter*>& HitCharacters, const FVector_NetQuantize& TraceStart,
	const TArray<FVector_NetQuantize>& HitLocations, float HitTime)
{

	TArray<FFramePackage> FramesToCheck;
	for(ABlasterCharacter* HitCharacter : HitCharacters)
	{
		FramesToCheck.Add(GetFrameToCheck(HitCharacter, HitTime));
	}
	
	return ShotgunConfirmHit(FramesToCheck, TraceStart, HitLocations);
}

FServerSideRewindResult ULagCompensationComponent::ProjectileServerSideRewind(ABlasterCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime)
{
	const FFramePackage FrameToCheck = GetFrameToCheck(HitCharacter, HitTime);
	return ProjectileConfirmHit(FrameToCheck, HitCharacter, TraceStart, InitialVelocity, HitTime);
	
}

void ULagCompensationComponent::Server_ScoreRequest_Implementation(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime)
{
	const FServerSideRewindResult ConfirmedResult = ServerSideRewind(HitCharacter, TraceStart, HitLocation, HitTime);

	if(Character && HitCharacter && Character->GetEquippedWeapon() && ConfirmedResult.bHitConfirmed)
	{
		const float DamageToCause = ConfirmedResult.bHeadshot ? Character->GetEquippedWeapon()->GetHeadshotDamage() : Character->GetEquippedWeapon()->GetDamage();

		
		UGameplayStatics::ApplyDamage(HitCharacter, DamageToCause, Character->Controller, Character->GetEquippedWeapon(), UDamageType::StaticClass());
	}
}

void ULagCompensationComponent::Server_ShotgunScoreRequest_Implementation(
	const TArray<ABlasterCharacter*>& HitCharacters, const FVector_NetQuantize& TraceStart,
	const TArray<FVector_NetQuantize>& HitLocations, float HitTime)
{
	const FShotgunServerSideRewindResult& ConfirmedResult = ShotgunServerSideRewind(HitCharacters, TraceStart, HitLocations, HitTime);

	for(auto& HitCharacter  : HitCharacters)
	{
		if(Character == nullptr || Character->GetEquippedWeapon() == nullptr) return;
		if(HitCharacter == nullptr) continue;
		
		float HeadshotDamage{};
		float BodyshotDamage{};
		float TotalDamage{};
		if(ConfirmedResult.Headshots.Contains(HitCharacter))
		{
			
			HeadshotDamage = ConfirmedResult.Headshots[HitCharacter] * Character->GetEquippedWeapon()->GetHeadshotDamage();
			UE_LOG(LogTemp,Warning,TEXT("HeadshotDamage : %f , To Character : %s"), HeadshotDamage, *HitCharacter->GetName());
			TotalDamage += HeadshotDamage;
			
		}

		if(ConfirmedResult.Bodyshots.Contains(HitCharacter))
		{
			BodyshotDamage = ConfirmedResult.Bodyshots[HitCharacter] * Character->GetEquippedWeapon()->GetDamage();
			UE_LOG(LogTemp,Warning,TEXT("BodyshotDamage : %f , To Character : %s"), BodyshotDamage, *HitCharacter->GetName());
			TotalDamage += BodyshotDamage;
		}
		UGameplayStatics::ApplyDamage(HitCharacter, TotalDamage, Character->Controller, Character->GetEquippedWeapon(), UDamageType::StaticClass());
	}
}

void ULagCompensationComponent::Server_ProjectileScoreRequest_Implementation(ABlasterCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime)
{
	
	FServerSideRewindResult ConfirmedResult = ProjectileServerSideRewind(HitCharacter, TraceStart, InitialVelocity, HitTime);

	const float DamageToCause = ConfirmedResult.bHeadshot ? Character->GetEquippedWeapon()->GetHeadshotDamage() : Character->GetEquippedWeapon()->GetDamage();

	
	if(Character && HitCharacter && ConfirmedResult.bHitConfirmed)
	{
		UGameplayStatics::ApplyDamage(HitCharacter, DamageToCause, Character->Controller, Character->GetEquippedWeapon(), UDamageType::StaticClass());
	}
	
}

