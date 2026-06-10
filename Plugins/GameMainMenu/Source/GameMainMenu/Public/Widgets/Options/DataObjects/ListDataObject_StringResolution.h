// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"
#include "ListDataObject_StringResolution.generated.h"

/**
 * 
 */
UCLASS()
class GAMEMAINMENU_API UListDataObject_StringResolution : public UListDataObject_String
{
	GENERATED_BODY()
	
public:

	void InitResolutionValues();

	FORCEINLINE FString GetMaxAllowedResolution() { return FString("MaxAllowedResolution"); }

protected:

	// ~Begin UListDataObject_String Interface
	virtual void OnDataObjectInitialized() override;
	// ~End UListDataObject_String Interface

private:

	FString ResolutionToValueString(const FIntPoint& InResolution) const;
	FText ResolutionToDisplayText(const FIntPoint& InResolution) const;

	FString MaxAllowedResolution;
	
};
