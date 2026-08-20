// HangHang All Rights Reserved

#include "FrontendGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Widgets/Multiplayer/MenuSessionData.h"

namespace
{
	constexpr const TCHAR* MenuSessionSlotName = TEXT("MenuSession");
	constexpr int32 MenuSessionSlotIndex = 0;
}

UMenuSessionData* UFrontendGameInstance::GetOrCreateMenuSessionData()
{
	if (!CachedMenuSessionData)
	{
		CachedMenuSessionData = Cast<UMenuSessionData>(
			UGameplayStatics::CreateSaveGameObject(UMenuSessionData::StaticClass()));
	}
	return CachedMenuSessionData;
}

UMenuSessionData* UFrontendGameInstance::LoadMenuSessionData()
{
	CachedMenuSessionData = Cast<UMenuSessionData>(
		UGameplayStatics::LoadGameFromSlot(MenuSessionSlotName, MenuSessionSlotIndex));
	return CachedMenuSessionData;
}

void UFrontendGameInstance::SaveMenuSessionData()
{
	if (CachedMenuSessionData)
	{
		UGameplayStatics::SaveGameToSlot(CachedMenuSessionData, MenuSessionSlotName, MenuSessionSlotIndex);
	}
}
