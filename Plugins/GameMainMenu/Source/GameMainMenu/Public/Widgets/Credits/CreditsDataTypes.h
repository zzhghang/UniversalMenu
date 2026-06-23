// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CreditsDataTypes.generated.h"

USTRUCT(BlueprintType)
struct GAMEMAINMENU_API FCreditsEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Role;
};
