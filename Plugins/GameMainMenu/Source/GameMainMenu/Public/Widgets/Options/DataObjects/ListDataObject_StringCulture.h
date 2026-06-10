// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"
#include "ListDataObject_StringCulture.generated.h"

UCLASS()
class GAMEMAINMENU_API UListDataObject_StringCulture : public UListDataObject_String
{
	GENERATED_BODY()

protected:
	// ~Begin UListDataObject_String Interface
	virtual void OnDataObjectInitialized() override;
	// ~End UListDataObject_String Interface

private:
	void InitializeCultureOptions();
};
