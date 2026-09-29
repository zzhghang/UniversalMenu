// HangHang All Rights Reserved

#pragma once

#include "UserSettings/EnhancedInputUserSettings.h"
#include "OptionsKeyBindingProfile.generated.h"

/** Unequipped, in-memory profile shared by all key binding entries in a settings session. */
UCLASS(Transient)
class GAMEMAINMENU_API UOptionsKeyBindingProfile : public UEnhancedPlayerMappableKeyProfile
{
	GENERATED_BODY()

public:
	void InitializeFromProfile(const UEnhancedPlayerMappableKeyProfile& Source)
	{
		ProfileIdentifierString = Source.GetProfileIdString();
		// UE's profile serialization intentionally excludes the runtime mapping table.
		PlayerMappedKeys = Source.GetPlayerMappingRows();
	}
};
