	// HangHang All Rights Reserved


#include "Subsystems/FrontendUISubsystem.h"

#include "FrontendGameplayTags.h"
#include "Engine/AssetManager.h"
#include "FunctionLibrary/FrontendFunctionLibrary.h"
#include "Widgets/W_PrimaryLayout.h"
#include "Widgets/W_ActivatableBase.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Widgets/W_ConfirmScreen.h"

	UFrontendUISubsystem* UFrontendUISubsystem::Get(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert))
		{
			return UGameInstance::GetSubsystem<UFrontendUISubsystem>(World->GetGameInstance());
		}
	}
	
	return nullptr;
}

bool UFrontendUISubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> FoundClass;
		GetDerivedClasses(GetClass(), FoundClass);

		return FoundClass.IsEmpty();
	}
	return false;
}

void UFrontendUISubsystem::PushSoftWidgetToStackAsync(const FGameplayTag& InWidgetStackTag,
		TSoftClassPtr<UW_ActivatableBase> InSoftWidgetClass,
		TFunction<void(EAsyncPushWidgetState, UW_ActivatableBase*)> AsyncPushStackCallBack)
{
	if (!ensure(!InSoftWidgetClass.IsNull())) { return; }

	TWeakObjectPtr<UFrontendUISubsystem> WeakThis = this;

	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		InSoftWidgetClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[InSoftWidgetClass, WeakThis, InWidgetStackTag, AsyncPushStackCallBack]()
			{
				if (!WeakThis.IsValid())
				{
					return;
				}

				UClass* LoadWidgetClass =  InSoftWidgetClass.Get();
				if (!ensure(LoadWidgetClass && WeakThis->CreatedPrimaryLayout)) { return; }

				UCommonActivatableWidgetContainerBase* FoundWidgetStack =  WeakThis->CreatedPrimaryLayout->FindWidgetStackByTag(InWidgetStackTag);
				if (!ensureMsgf(FoundWidgetStack, TEXT("Widget stack not found for tag %s"), *InWidgetStackTag.ToString())) { return; }

				UW_ActivatableBase* CreatedWidget =  FoundWidgetStack->AddWidget<UW_ActivatableBase>(
					LoadWidgetClass,
					[AsyncPushStackCallBack](UW_ActivatableBase& CreatedWidgetInstance)
					{
						AsyncPushStackCallBack(EAsyncPushWidgetState::OnCreateBeforePush, &CreatedWidgetInstance);
					}
				);
				AsyncPushStackCallBack(EAsyncPushWidgetState::AfterPush, CreatedWidget);
			}
		)
	);
}

void UFrontendUISubsystem::PushConfirmWidgetToModalStackAsync(const EConfirmScreenType InConfirmType,
	const FText& InTitle, const FText& InMessage, TFunction<void(EConfirmScreenButtonType)> ButtonClickCallBack)
{
		UConfirmScreenInfoObject* CreateInfoObject = nullptr;

		switch (InConfirmType)
		{
		case EConfirmScreenType::Ok:
			CreateInfoObject = UConfirmScreenInfoObject::CreateOkScreen(InTitle, InMessage);
			break;

		case EConfirmScreenType::YesNo:
			CreateInfoObject = UConfirmScreenInfoObject::CreateYesNoScreen(InTitle,InMessage);
			break;

		case EConfirmScreenType::OkCancel:
			CreateInfoObject = UConfirmScreenInfoObject::CreateOkCancelScreen(InTitle,InMessage);
			break;
		case EConfirmScreenType::UnKnown:
			break;
		default:
			break;
		}

		if (!ensure(CreateInfoObject)) { return; }

		PushSoftWidgetToStackAsync(
			FrontendGameplayTags::Frontend_WidgetStack_Modal,
			UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(FrontendGameplayTags::Frontend_Widget_Confirm),
			[CreateInfoObject, ButtonClickCallBack](const EAsyncPushWidgetState InPushState, UW_ActivatableBase* PushWidget)
			{
				if (InPushState == EAsyncPushWidgetState::OnCreateBeforePush)
				{
					UW_ConfirmScreen* CreateConfirmScreen = CastChecked<UW_ConfirmScreen>(PushWidget);
					CreateConfirmScreen->InitConfirmScreen(CreateInfoObject, ButtonClickCallBack);
				}
			}
		);
}
