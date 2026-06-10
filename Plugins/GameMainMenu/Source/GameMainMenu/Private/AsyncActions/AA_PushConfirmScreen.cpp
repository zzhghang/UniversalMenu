// HangHang All Rights Reserved


#include "AsyncActions/AA_PushConfirmScreen.h"

#include "Subsystems/FrontendUISubsystem.h"

UAA_PushConfirmScreen* UAA_PushConfirmScreen::PushConfirmScreen(const UObject* WorldContextObject,
                                                                const EConfirmScreenType InConfirmType, const FText InTitle, const FText InMessage)
{
	if (GEngine)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			UAA_PushConfirmScreen* Node = NewObject<UAA_PushConfirmScreen>();
			
			Node->CachedOwningWorld = World;
			Node->CachedConfirmType = InConfirmType;
			Node->CachedTitle = InTitle;
			Node->CachedMessage = InMessage;

			Node->RegisterWithGameInstance(World);

			return Node;
		}
	}

	return nullptr;
}

void UAA_PushConfirmScreen::Activate()
{
	UFrontendUISubsystem::Get(CachedOwningWorld.Get())->PushConfirmWidgetToModalStackAsync(
		CachedConfirmType,
		CachedTitle,
		CachedMessage,
		[this](const EConfirmScreenButtonType ClickedButtonType)
		{
			OnConfirmScreenButtonClicked.Broadcast(ClickedButtonType);

			SetReadyToDestroy();
		}
	);
}
