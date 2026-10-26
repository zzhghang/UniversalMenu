// HangHang All Rights Reserved


#include "Widgets/MenuConfig/MenuConfigData.h"

#include "FunctionLibrary/FrontendFunctionLibrary.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"

static void ApplyCommonConfig(UListDataObject_Base* Entry, const UMenuEntryConfig* Data)
{
	if (Data->BindingKey == TEXT("FrameRateLimit"))
	{
		if (UListDataObject_String* StringEntry = Cast<UListDataObject_String>(Entry))
		{
			// Existing assets use keys such as "60", while the float getter returns "60.000000".
			StringEntry->SetUseNumericOptionMatching(true);
		}
	}
	Entry->SetDataId(Data->DataId);
	Entry->SetDataDisplayName(Data->DisplayName);
	Entry->SetDescriptionRichText(Data->Description);

	// The detail panel image is optional. Resolve it here so Options and Multiplayer entries
	// created through CreateEntryFromConfig share the same tag -> texture path.
	if (Data->DescriptionImageTag.IsValid())
	{
		Entry->SetSoftDescriptionImage(UFrontendFunctionLibrary::GetOptionsSoftImageByTag(Data->DescriptionImageTag));
	}

	if (Cast<UListDataObject_Value>(Entry)) Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
	Entry->SetDynamicDetailsRichText(Data->DynamicDetails);
	if (!Data->DisableRichReason.IsEmpty())
		Entry->SetDisableRichText(Data->DisableRichReason);
}

// ============================================================================
// CreateEntryFromConfig
// ============================================================================

UListDataObject_Base* CreateEntryFromConfig(const UMenuEntryConfig* Data)
{
	if (!Data) return nullptr;

	// Loaded assets own an inline config. Old C++ callers can still populate legacy fields.
	const UMenuEntryValueConfig* ValueConfig = Data->ValueConfig;
	if (!ValueConfig) ValueConfig = Data->CreateLegacyValueConfig(GetTransientPackage());
	if (!ValueConfig) return nullptr;

	UListDataObject_Base* Entry = ValueConfig->CreateDataObject(*Data);

	if (Entry)
	{
		ApplyCommonConfig(Entry, Data);
	}

	return Entry;
}

UListDataObject_Collection* CreateCollectionFromConfig(const FName& Id, const FText& DisplayName)
{
	UListDataObject_Collection* Col = NewObject<UListDataObject_Collection>();
	Col->SetDataId(Id);
	Col->SetDataDisplayName(DisplayName);
	return Col;
}
