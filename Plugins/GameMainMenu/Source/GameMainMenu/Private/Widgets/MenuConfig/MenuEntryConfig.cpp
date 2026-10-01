// HangHang All Rights Reserved

#include "Widgets/MenuConfig/MenuEntryConfig.h"
#include "UObject/ObjectSaveContext.h"

EMenuEntryValueType UMenuEntryConfig::GetEffectiveValueType() const
{
	return ValueConfig ? ValueConfig->GetValueType() : ValueType;
}

UMenuEntryValueConfig* UMenuEntryConfig::CreateLegacyValueConfig(UObject* Outer) const
{
	UClass* ConfigClass = nullptr;
	switch (ValueType)
	{
	case EMenuEntryValueType::Bool: ConfigClass = UMenuBoolValueConfig::StaticClass(); break;
	case EMenuEntryValueType::Scalar: ConfigClass = UMenuScalarValueConfig::StaticClass(); break;
	case EMenuEntryValueType::String: ConfigClass = UMenuStringValueConfig::StaticClass(); break;
	case EMenuEntryValueType::Integer: ConfigClass = UMenuIntegerValueConfig::StaticClass(); break;
	case EMenuEntryValueType::Culture: ConfigClass = UMenuCultureValueConfig::StaticClass(); break;
	case EMenuEntryValueType::Resolution: ConfigClass = UMenuResolutionValueConfig::StaticClass(); break;
	case EMenuEntryValueType::Editable: ConfigClass = UMenuEditableValueConfig::StaticClass(); break;
	default: return nullptr;
	}

	UMenuEntryValueConfig* Config = NewObject<UMenuEntryValueConfig>(Outer, ConfigClass, NAME_None, RF_Transactional);
	Config->ImportLegacy(*this);
	return Config;
}

void UMenuEntryConfig::UpgradeLegacyValueConfig()
{
	if (!ValueConfig && !IsTemplate())
	{
		ValueConfig = CreateLegacyValueConfig(this);
	}
}

void UMenuEntryConfig::SynchronizeLegacyFields()
{
	if (ValueConfig)
	{
		ValueType = ValueConfig->GetValueType();
		ValueConfig->ExportLegacy(*this);
	}
}

void UMenuEntryConfig::PostLoad()
{
	Super::PostLoad();
	if (IsTemplate()) return;
	UpgradeLegacyValueConfig();
	if (ValueConfig) ValueConfig->ConditionalPostLoad();
	SynchronizeLegacyFields();
}

void UMenuEntryConfig::PreSave(FObjectPreSaveContext SaveContext)
{
	UpgradeLegacyValueConfig();
	SynchronizeLegacyFields();
	Super::PreSave(SaveContext);
}

#if WITH_EDITOR
void UMenuEntryConfig::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	SynchronizeLegacyFields();
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UMenuEntryConfig::PostEditUndo()
{
	Super::PostEditUndo();
	SynchronizeLegacyFields();
}
#endif
