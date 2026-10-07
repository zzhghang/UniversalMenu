// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "FrontendTypes/FrontendEnumTypes.h"
#include "FrontendTypes/FrontendStructTypes.h"
#include "UObject/Object.h"
#include "ListDataObject_Base.generated.h"


#define LIST_DATA_ACCESSOR(DataType,PropertyName) \
FORCEINLINE DataType Get##PropertyName() const { return PropertyName;} \
void Set##PropertyName(DataType In##PropertyName) {PropertyName = In##PropertyName;}

UCLASS(Abstract)
class GAMEMAINMENU_API UListDataObject_Base : public UObject
{
	GENERATED_BODY()
	
public:
	
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnListDataModifiedDelegate, UListDataObject_Base*,EOptionsListDataModifyReason);
	
	FOnListDataModifiedDelegate OnListDataModified;
	
	FOnListDataModifiedDelegate OnDependencyDataModified;
	
public:
	
	LIST_DATA_ACCESSOR(FName, DataId)
	
	LIST_DATA_ACCESSOR(FText, DataDisplayName)
	
	LIST_DATA_ACCESSOR(FText, DescriptionRichText)
	
	LIST_DATA_ACCESSOR(FText, DisableRichText)
	
	LIST_DATA_ACCESSOR(TSoftObjectPtr<UTexture2D>, SoftDescriptionImage)
	
	LIST_DATA_ACCESSOR(FText, DynamicDetailsRichText)
	
	LIST_DATA_ACCESSOR(UListDataObject_Base*, ParentData)
	
	/** Override in UListDataObject_Collection to return all child entries. */
	virtual TArray<UListDataObject_Base*> GetAllChildListData() const { return TArray<UListDataObject_Base*>();}

	/** Override in UListDataObject_Collection to indicate whether this node has children. */
	virtual bool HasAnyChildListData() const { return false;}

	/** Initialize data object state. Calls OnDataObjectInitialized. */
	void InitializeDataObject();
	
	void SetShouldApplyChangeImmediately(const bool bShouldApplyRightAway) {bShouldApplyChangeImmediately = bShouldApplyRightAway;}
	void SetImmediateApplyHandler(TFunction<void()> Handler) { ImmediateApplyHandler = MoveTemp(Handler); }
	
	/** Override in subclasses to support reset-to-default behavior. */
	virtual bool HasDefaultValue() const {return false;};
	virtual bool CanResetBackToDefaultValue() const { return false; };
	virtual bool TryResetBackToDefaultValue() { return false; };

	/** Form serialization uses stored keys/values, never localized display labels. */
	virtual bool TryGetValueAsString(FString& OutValue) const { return false; }
	virtual bool TrySetValueFromString(const FString& Value) { return false; }

	void AddEditConditionDescriptor(const FOptionsDataEditConditionDescriptor& InEditConditionDesc);
	void AddEditDependencyData(UListDataObject_Base* InDependencyDataObject);
	bool IsDataCurrentlyEditable();
	
protected:
	
	/** Override in subclasses for custom initialization logic. */
	virtual void OnDataObjectInitialized();
	
	virtual void NotifyListDataModified(UListDataObject_Base* ModifyData, const EOptionsListDataModifyReason ModifyReason = EOptionsListDataModifyReason::DirectlyModified);
	
	virtual bool CanSetToForcedStringValue(const FString& InForcedStringValue) const { return false; }
	virtual void OnSetForcedStringValue(const FString& InForcedStringValue) {}
	
	virtual void OnEditDependencyDataModified(UListDataObject_Base* ModifyDependencyData, const EOptionsListDataModifyReason ModifyReason);
	
private:

	FName DataId;
	
	FText DataDisplayName;
	
	FText DescriptionRichText;
	
	FText DisableRichText;
	
	FText DynamicDetailsRichText;
	
	TSoftObjectPtr<UTexture2D> SoftDescriptionImage;

	UPROPERTY(Transient)
	UListDataObject_Base* ParentData;

	bool bShouldApplyChangeImmediately = false;
	TFunction<void()> ImmediateApplyHandler;
	
	UPROPERTY(Transient)
	TArray<FOptionsDataEditConditionDescriptor> EditConditionDescArray;
	
};
