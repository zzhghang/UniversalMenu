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
	
	LIST_DATA_ACCESSOR(UListDataObject_Base*, ParentData)
	
	//子类(UListDataObject_Collection)需要重写,返回该标签下的所有子数据
	virtual TArray<UListDataObject_Base*> GetAllChildListData() const { return TArray<UListDataObject_Base*>();}
	
	//子类(UListDataObject_Collection)需要重写
	virtual bool HasAnyChildListData() const { return false;}
	
	//初始化数据对象
	void InitializeDataObject();
	
	void SetShouldApplyChangeImmediately(const bool bShouldApplyRightAway) {bShouldApplyChangeImmediately = bShouldApplyRightAway;}
	
	//子类需要重写,用于重置条目
	virtual bool HasDefaultValue() const {return false;};
	virtual bool CanResetBackToDefaultValue() const { return false; };
	virtual bool TryResetBackToDefaultValue() { return false; };

	void AddEditConditionDescriptor(const FOptionsDataEditConditionDescriptor& InEditConditionDesc);
	void AddEditDependencyData(UListDataObject_Base* InDependencyDataObject);
	bool IsDataCurrentlyEditable();
	
protected:
	
	//子类应该重写此方法，以相应地处理所需的初始化
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
	
	TSoftObjectPtr<UTexture2D> SoftDescriptionImage;

	UPROPERTY(Transient)
	UListDataObject_Base* ParentData;

	bool bShouldApplyChangeImmediately = false; 
	
	UPROPERTY(Transient)
	TArray<FOptionsDataEditConditionDescriptor> EditConditionDescArray;
	
};
