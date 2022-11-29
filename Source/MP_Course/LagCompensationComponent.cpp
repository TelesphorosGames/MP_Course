// Fill out your copyright notice in the Description page of Project Settings.


#include "LagCompensationComponent.h"

#include "BlasterCharacter.h"
#include "BlasterPlayerController.h"
#include "Weapon.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


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
	HeadshotBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;
	UWorld* World = GetWorld();
	if(World)
	{
		World->LineTraceSingleByChannel(ConfirmedHitResult, TraceStart, TraceEnd, ECC_Visibility);
		if(ConfirmedHitResult.GetComponent() == HeadshotBox)
		{
			ResetHitBoxes(HitCharacter, CurrentFrame);
			EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
			return FServerSideRewindResult{true, true};
		}
		
		
		World->LineTraceSingleByChannel(ConfirmedHitResult, TraceStart, TraceEnd, ECC_Visibility);
		
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
		HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		HeadBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	}
	UWorld* World = GetWorld();
	for(auto& HitLocation : HitLocations)
	{
		const FVector TraceEnd = (TraceStart + (HitLocation - TraceStart)) * 1.25f;
		if(World)
		{
			World->LineTraceSingleByChannel(ConfirmedHitResult, TraceStart, TraceEnd, ECC_Visibility);
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
		for(auto& HitBoxPair : Frame.Character->HitCollisionBoxes)
		{
			if(HitBoxPair.Value != nullptr)
			{
				HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				HitBoxPair.Value->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECR_Block);
			}
		}
		UBoxComponent* HeadBox = Frame.Character->HitCollisionBoxes[FName("head")];
		HeadBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	for(auto& HitLocation : HitLocations)
	{
		const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;
		
		if(World)
		{
			World->LineTraceSingleByChannel(ConfirmedHitResult, TraceStart, TraceEnd, ECC_Visibility);
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

void ULagCompensationComponent::CacheBoxPositions(ABlasterCharacter* HitCharacter, FFramePackage& OutFramePackage)
{
	if(HitCharacter == nullptr) return;
		

	for(auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
	{
		if(HitBoxPair.Value != nullptr)
		{
			FBoxInformation BoxInfo;

			BoxInfo.Location = HitBoxPair.Value->GetComponentLocation();
			BoxInfo.Rotation = HitBoxPair.Value->GetComponentRotation();
			BoxInfo.Extent = HitBoxPair.Value->GetScaledBoxExtent();
			OutFramePackage.HitBoxInfo.Add(HitBoxPair.Key, BoxInfo);
			
		}
	}
}

void ULagCompensationComponent::MoveBoxes(ABlasterCharacter* HitCharacter, const FFramePackage& Package)
{
	if(HitCharacter == nullptr) return;
	for(auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
	{
		if(HitBoxPair.Value != nullptr)
		{
			HitBoxPair.Value->SetWorldLocation(Package.HitBoxInfo[HitBoxPair.Key].Location);
			HitBoxPair.Value->SetWorldRotation(Package.HitBoxInfo[HitBoxPair.Key].Rotation);
			HitBoxPair.Value->SetBoxExtent(Package.HitBoxInfo[HitBoxPair.Key].Extent);
		
			HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			HitBoxPair.Value->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		
			
		}
	}
}

void ULagCompensationComponent::ResetHitBoxes(ABlasterCharacter* HitCharacter, const FFramePackage& Package)
{
	if(HitCharacter == nullptr) return;
	for(auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
	{
		if(HitBoxPair.Value != nullptr)
		{
			HitBoxPair.Value->SetWorldLocation(Package.HitBoxInfo[HitBoxPair.Key].Location);
			HitBoxPair.Value->SetWorldRotation(Package.HitBoxInfo[HitBoxPair.Key].Rotation);
			HitBoxPair.Value->SetBoxExtent(Package.HitBoxInfo[HitBoxPair.Key].Extent);
			HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

void ULagCompensationComponent::Server_ScoreRequest_Implementation(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime, AWeapon* DamageCauser)
{
	FServerSideRewindResult ConfirmedResult = ServerSideRewind(HitCharacter, TraceStart, HitLocation, HitTime);

	if(HitCharacter && DamageCauser && ConfirmedResult.bHitConfirmed)
	{
		UGameplayStatics::ApplyDamage(HitCharacter, DamageCauser->GetDamage(), Character->Controller, DamageCauser, UDamageType::StaticClass());
	}
}

void ULagCompensationComponent::Server_ShotgunScoreRequest_Implementation(
	const TArray<ABlasterCharacter*>& HitCharacters, const FVector_NetQuantize& TraceStart,
	const TArray<FVector_NetQuantize>& HitLocations, float HitTime)
{
	const FShotgunServerSideRewindResult& Confirmed = ShotgunServerSideRewind(HitCharacters, TraceStart, HitLocations, HitTime);

	for(auto& HitCharacter  : HitCharacters)
	{
		if(Character == nullptr || Character->GetEquippedWeapon() == nullptr) return;
		if(HitCharacter == nullptr) continue;
		
		float HeadshotDamage{};
		float BodyshotDamage{};
		if(Confirmed.Headshots.Contains(HitCharacter))
		{
			HeadshotDamage = Confirmed.Headshots[HitCharacter] * Character->GetEquippedWeapon()->GetDamage();
			UE_LOG(LogTemp,Warning,TEXT("HeadshotDamage : %d , To Character : %s"), HeadshotDamage, *HitCharacter->GetName());
		}

		if(Confirmed.Bodyshots.Contains(HitCharacter))
		{
			BodyshotDamage = Confirmed.Bodyshots[HitCharacter] * Character->GetEquippedWeapon()->GetDamage();
			UE_LOG(LogTemp,Warning,TEXT("BodyshotDamage : %d , To Character : %s"), BodyshotDamage, *HitCharacter->GetName());
		}
		UGameplayStatics::ApplyDamage(HitCharacter, HeadshotDamage + BodyshotDamage, Character->Controller, Character->GetEquippedWeapon(), UDamageType::StaticClass());
	}
}


