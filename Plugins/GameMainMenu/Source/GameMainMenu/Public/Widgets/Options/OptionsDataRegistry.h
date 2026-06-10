// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Math/Range.h"
#include "UObject/Object.h"
#include "OptionsDataRegistry.generated.h"

class UListDataObject_Base;
class UListDataObject_Collection;
class UListDataObject_StringBool;
class UListDataObject_Scalar;
class UListDataObject_String;
class UListDataObject_StringInteger;
class FOptionsDataInteractionHelper;
enum class ECommonNumericType : uint8;

UCLASS()
class GAMEMAINMENU_API UOptionsDataRegistry : public UObject
{
	GENERATED_BODY()

public:

	void InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer);

	const TArray<UListDataObject_Collection*>& GetRegisteredOptionsCollectionTabs() { return RegisteredOptionsCollectionTabs;}

	TArray<UListDataObject_Base*> GetListSourceItemBySelectedTabId(const FName& InSelectedTabId) const;

private:

	void FindChildListDataRecursively(UListDataObject_Base* InParentData, TArray<UListDataObject_Base*>& OutFoundChildListData) const;

	void InitGameplayCollectionTab();
	void InitAudioCollectionTab();
	void InitVideoCollectionTab();
	void InitControlCollectionTab(ULocalPlayer* InOwningLocalPlayer);

	// ---- Factory helpers ----

	UListDataObject_Collection* CreateCollection(const FName& Id, const FText& DisplayName) const;

	UListDataObject_StringBool* CreateBoolEntry(
		const FName& Id, const FText& DisplayName, const FText& Description,
		const TSharedPtr<FOptionsDataInteractionHelper>& Getter,
		const TSharedPtr<FOptionsDataInteractionHelper>& Setter,
		bool bDefaultValue) const;

	UListDataObject_Scalar* CreateScalarEntry(
		const FName& Id, const FText& DisplayName, const FText& Description,
		const TSharedPtr<FOptionsDataInteractionHelper>& Getter,
		const TSharedPtr<FOptionsDataInteractionHelper>& Setter,
		const TRange<float>& DisplayRange, const TRange<float>& OutRange,
		float StepSize, float DefaultValue,
		ECommonNumericType NumericType) const;

	UListDataObject_String* CreateStringEntry(
		const FName& Id, const FText& DisplayName, const FText& Description,
		const TSharedPtr<FOptionsDataInteractionHelper>& Getter,
		const TSharedPtr<FOptionsDataInteractionHelper>& Setter,
		const TArray<TPair<FString, FText>>& Options,
		const FString& DefaultValue) const;

	UListDataObject_StringInteger* CreateQualityPresetEntry(
		const FName& Id, const FText& DisplayName, const FText& Description,
		const TSharedPtr<FOptionsDataInteractionHelper>& Getter,
		const TSharedPtr<FOptionsDataInteractionHelper>& Setter) const;

	UPROPERTY(Transient)
	TArray<UListDataObject_Collection*> RegisteredOptionsCollectionTabs;

};
