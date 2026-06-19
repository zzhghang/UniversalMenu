// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "LoadingScreenPhaseMapping.generated.h"

USTRUCT(BlueprintType)
struct GAMEMAINMENU_API FLoadingPhaseDisplayInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "Frontend.Loading.Phase"))
	FGameplayTag PhaseTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor TintColor = FLinearColor::White;
};

UCLASS(BlueprintType)
class GAMEMAINMENU_API ULoadingScreenPhaseMapping : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loading Screen")
	TArray<FLoadingPhaseDisplayInfo> PhaseEntries;

	UFUNCTION(BlueprintPure)
	FLoadingPhaseDisplayInfo FindDisplayInfo(FGameplayTag PhaseTag) const;
};
