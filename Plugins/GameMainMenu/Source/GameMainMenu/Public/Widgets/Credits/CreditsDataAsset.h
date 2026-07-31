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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits", meta=(ClampMin="0.0"))
	float ScrollSpeed = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits", meta=(ClampMin="0.0"))
	float StartDelay = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits", meta=(ClampMin="1.0"))
	float LineHeight = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits", meta=(ClampMin="0.0"))
	float EntrySpacing = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits", meta=(ClampMin="0.0"))
	float LogoSpacing = 120.f;
};
