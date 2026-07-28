// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/MenuConfig/MenuConfigTypes.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"
#include "ListDataObject_Editable.generated.h"

/**
 * Editable text data object for multiplayer room name / password input.
 */
UCLASS()
class GAMEMAINMENU_API UListDataObject_Editable : public UListDataObject_Base
{
	GENERATED_BODY()

public:
	void SetText(const FText& InText);

	const FText& GetText() const { return CurrentText; }

	FString GetHashedPassword() const;

	/** Check whether InText passes MaxCharCount and CharFilter rules. Sets OutReason if invalid. */
	bool ValidateText(const FText& InText, FText& OutReason) const;

	LIST_DATA_ACCESSOR(int32, MaxCharCount)
	LIST_DATA_ACCESSOR(bool, bIsPassword)
	LIST_DATA_ACCESSOR(FText, HintText)
	LIST_DATA_ACCESSOR(EEditableCharFilter, CharFilter)

protected:
	virtual void OnDataObjectInitialized() override;

private:
	FText CurrentText;

	int32 MaxCharCount = 0;
	bool bIsPassword = false;
	FText HintText;
	EEditableCharFilter CharFilter = EEditableCharFilter::All;
};
