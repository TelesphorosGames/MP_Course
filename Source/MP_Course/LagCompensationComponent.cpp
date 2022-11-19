// Fill out your copyright notice in the Description page of Project Settings.


#include "LagCompensationComponent.h"

#include "BlasterCharacter.h"
#include "Components/BoxComponent.h"

ULagCompensationComponent::ULagCompensationComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}



void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();

	FFramePackage FramePackage;
	SaveFramePackage(FramePackage);

	ShowFramePackage(FramePackage, FColor::Yellow);

	
}

void ULagCompensationComponent::SaveFramePackage(FFramePackage& Package)
{
	if(Character)
	{
		Package.Time = GetWorld()->GetTimeSeconds();
		for (auto& BoxPair : Character->GetHitCollisionBoxes())
		{
			FBoxInformation BoxInformation;
			BoxInformation.Location = BoxPair.Value->GetComponentLocation();
			BoxInformation.Rotation = BoxPair.Value->GetComponentRotation();
			BoxInformation.Extent = BoxPair.Value->GetScaledBoxExtent();
			Package.HitBoxInfo.Add(BoxPair.Key, BoxInformation);
		}
	}
}


void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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
		ShowFramePackage(ThisFrame, FColor::Red);
		
	}

}

void ULagCompensationComponent::ShowFramePackage(const FFramePackage& Package, FColor Color)
{
	for(auto& BoxInfo : Package.HitBoxInfo)
	{
		DrawDebugBox(GetWorld(), BoxInfo.Value.Location, BoxInfo.Value.Extent, FQuat(BoxInfo.Value.Rotation), Color, false, 4.f);
	}
	
	
}

