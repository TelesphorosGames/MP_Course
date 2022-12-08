// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define ECC_SkeletalMesh ECollisionChannel::ECC_GameTraceChannel1
#define ECC_HitBox ECollisionChannel::ECC_GameTraceChannel2
#define Debug(Text) UE_LOG(LogTemp, Warning, TEXT(Text))
#define Error(Text) UE_LOG(LogTemp, Error, TEXT(Text))