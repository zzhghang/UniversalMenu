// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Widgets/Credits/CreditsDataTypes.h"
#include "CreditsDataAsset.generated.h"

UCLASS(BlueprintType)
class GAMEMAINMENU_API UCreditsDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits")
	TArray<FCreditsEntry> Entries;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits")
	float ScrollSpeed = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits")
	float StartDelay = 1.f;
};
