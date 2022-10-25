#include "MyAssetManager.h"
#include "MyGameplayTags.h"

UMyAssetManager::UMyAssetManager()
{

}

UMyAssetManager& UMyAssetManager::Get()
{
	check(GEngine);

	UMyAssetManager* MyAssetManager = Cast<UMyAssetManager>(GEngine->AssetManager);
	return *MyAssetManager;
}

void UMyAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	//Load Native Tags
	FMyGameplayTags::InitializeNativeTags();
}