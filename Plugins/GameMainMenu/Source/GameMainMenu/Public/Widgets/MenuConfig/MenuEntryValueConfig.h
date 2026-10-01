// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonNumericTextBlock.h"
#include "Widgets/MenuConfig/MenuConfigTypes.h"
#include "MenuEntryValueConfig.generated.h"

class UListDataObject_Base;
class UMenuEntryConfig;

/** Type-specific parameters owned by an entry asset and edited inline. */
UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced, CollapseCategories)
class GAMEMAINMENU_API UMenuEntryValueConfig : public UObject
{
	GENERATED_BODY()

public:
	virtual EMenuEntryValueType GetValueType() const PURE_VIRTUAL(UMenuEntryValueConfig::GetValueType, return EMenuEntryValueType::Bool;);
	virtual UListDataObject_Base* CreateDataObject(const UMenuEntryConfig& Entry) const PURE_VIRTUAL(UMenuEntryValueConfig::CreateDataObject, return nullptr;);
	virtual void ImportLegacy(const UMenuEntryConfig& Entry) PURE_VIRTUAL(UMenuEntryValueConfig::ImportLegacy, );
	virtual void ExportLegacy(UMenuEntryConfig& Entry) const PURE_VIRTUAL(UMenuEntryValueConfig::ExportLegacy, );

#if WITH_EDITOR
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditUndo() override;
#endif
};

UCLASS(meta = (DisplayName = "Bool (Toggle)"))
class GAMEMAINMENU_API UMenuBoolValueConfig : public UMenuEntryValueConfig
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	bool bDefaultValue = false;

	virtual EMenuEntryValueType GetValueType() const override { return EMenuEntryValueType::Bool; }
	virtual UListDataObject_Base* CreateDataObject(const UMenuEntryConfig& Entry) const override;
	virtual void ImportLegacy(const UMenuEntryConfig& Entry) override;
	virtual void ExportLegacy(UMenuEntryConfig& Entry) const override;
};

UCLASS(meta = (DisplayName = "Scalar (Slider)"))
class GAMEMAINMENU_API UMenuScalarValueConfig : public UMenuEntryValueConfig
{
	GENERATED_BODY()
public:
	/** Default in the output range, stored as a numeric string. Empty means no explicit default. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	FString DefaultValue;

	/** Minimum value displayed by the slider. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	float ScalarMin = 0.f;

	/** Maximum value displayed by the slider. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	float ScalarMax = 1.f;

	/** Minimum underlying value sent to the settings binding. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	float ScalarOutMin = 0.f;

	/** Maximum underlying value sent to the settings binding. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	float ScalarOutMax = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	float StepSize = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	ECommonNumericType NumericType = ECommonNumericType::Percentage;

	virtual EMenuEntryValueType GetValueType() const override { return EMenuEntryValueType::Scalar; }
	virtual UListDataObject_Base* CreateDataObject(const UMenuEntryConfig& Entry) const override;
	virtual void ImportLegacy(const UMenuEntryConfig& Entry) override;
	virtual void ExportLegacy(UMenuEntryConfig& Entry) const override;
};

UCLASS(meta = (DisplayName = "String (Choice)"))
class GAMEMAINMENU_API UMenuStringValueConfig : public UMenuEntryValueConfig
{
	GENERATED_BODY()
public:
	/** Empty means no explicit default. FullscreenMode uses Fullscreen, WindowedFullscreen or Windowed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	FString DefaultValue;

	/** Stored keys and localized labels. Empty FullscreenMode options use built-in window modes. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	TMap<FString, FText> AvailableOptions;

	virtual EMenuEntryValueType GetValueType() const override { return EMenuEntryValueType::String; }
	virtual UListDataObject_Base* CreateDataObject(const UMenuEntryConfig& Entry) const override;
	virtual void ImportLegacy(const UMenuEntryConfig& Entry) override;
	virtual void ExportLegacy(UMenuEntryConfig& Entry) const override;
};

UCLASS(meta = (DisplayName = "Integer (Choice)"))
class GAMEMAINMENU_API UMenuIntegerValueConfig : public UMenuEntryValueConfig
{
	GENERATED_BODY()
public:
	/** Includes zero. FullscreenMode: 0 = fullscreen, 1 = borderless, 2 = windowed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	int32 DefaultValue = 0;

	/** Empty FullscreenMode options use built-in window modes. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	TMap<int32, FText> AvailableOptions;

	virtual EMenuEntryValueType GetValueType() const override { return EMenuEntryValueType::Integer; }
	virtual UListDataObject_Base* CreateDataObject(const UMenuEntryConfig& Entry) const override;
	virtual void ImportLegacy(const UMenuEntryConfig& Entry) override;
	virtual void ExportLegacy(UMenuEntryConfig& Entry) const override;
};

UCLASS(meta = (DisplayName = "Culture (Language)"))
class GAMEMAINMENU_API UMenuCultureValueConfig : public UMenuEntryValueConfig
{
	GENERATED_BODY()
public:
	/** Culture code such as en or zh-Hans. Empty means no explicit default. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	FString DefaultValue;

	virtual EMenuEntryValueType GetValueType() const override { return EMenuEntryValueType::Culture; }
	virtual UListDataObject_Base* CreateDataObject(const UMenuEntryConfig& Entry) const override;
	virtual void ImportLegacy(const UMenuEntryConfig& Entry) override;
	virtual void ExportLegacy(UMenuEntryConfig& Entry) const override;
};

UCLASS(meta = (DisplayName = "Resolution"))
class GAMEMAINMENU_API UMenuResolutionValueConfig : public UMenuEntryValueConfig
{
	GENERATED_BODY()
public:
	/** Resolution string such as (X=1920,Y=1080). Empty means no explicit default. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	FString DefaultValue;

	virtual EMenuEntryValueType GetValueType() const override { return EMenuEntryValueType::Resolution; }
	virtual UListDataObject_Base* CreateDataObject(const UMenuEntryConfig& Entry) const override;
	virtual void ImportLegacy(const UMenuEntryConfig& Entry) override;
	virtual void ExportLegacy(UMenuEntryConfig& Entry) const override;
};

UCLASS(meta = (DisplayName = "Editable (Text Input)"))
class GAMEMAINMENU_API UMenuEditableValueConfig : public UMenuEntryValueConfig
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	FText HintText;

	/** Zero means no explicit limit. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	int32 MaxCharCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	bool bIsPassword = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	EEditableCharFilter CharFilter = EEditableCharFilter::All;

	virtual EMenuEntryValueType GetValueType() const override { return EMenuEntryValueType::Editable; }
	virtual UListDataObject_Base* CreateDataObject(const UMenuEntryConfig& Entry) const override;
	virtual void ImportLegacy(const UMenuEntryConfig& Entry) override;
	virtual void ExportLegacy(UMenuEntryConfig& Entry) const override;
};
