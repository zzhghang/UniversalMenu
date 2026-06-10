// HangHang All Rights Reserved


#include "AsyncActions/AA_PushSoftWidget.h"

#include "Subsystems/FrontendUISubsystem.h"
#include "Widgets/W_ActivatableBase.h"


UAA_PushSoftWidget* UAA_PushSoftWidget::PushSoftWidget(const UObject* WorldContextObject,
                                                       APlayerController* OwningPlayerController, TSoftClassPtr<UW_ActivatableBase> SoftWidgetClass,
                                                       UPARAM(meta = (Categories = "Frontend.WidgetStack")) FGameplayTag WidgetStackTag, bool bFocus)
{
	checkf(!SoftWidgetClass.IsNull(),TEXT("PushSoftWidgetToStack was passed a null soft widget class"));
	
	if (GEngine)
	{
		// 从世界上下文对象获取世界实例
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			// 创建新的异步操作实例
			UAA_PushSoftWidget* Node  = NewObject<UAA_PushSoftWidget>();

			// 缓存所有参数
			Node->CachedOwningWorld = World;
			Node->CachedOwningPlayerController = OwningPlayerController;
			Node->CachedSoftWidgetClass = SoftWidgetClass;
			Node->CachedWidgetStackTag = WidgetStackTag;
			Node->bCachedFocus = bFocus; 

			// 注册到游戏实例
			Node->RegisterWithGameInstance(World);

			return Node;
		}
		
	}

	return nullptr;
}

void UAA_PushSoftWidget::Activate()
{
	const UFrontendUISubsystem* FrontendUISubsystem = UFrontendUISubsystem::Get(CachedOwningWorld.Get());

	FrontendUISubsystem->PushSoftWidgetToStackAsync(CachedWidgetStackTag, CachedSoftWidgetClass, 
		[this](const EAsyncPushWidgetState InPushState, UW_ActivatableBase* PushWidget)
		{
			switch (InPushState)
			{
			case EAsyncPushWidgetState::OnCreateBeforePush:
				PushWidget->SetOwningPlayer(CachedOwningPlayerController.Get());
				OnWidgetCreatedBeforePush.Broadcast(PushWidget);
				break;

			case EAsyncPushWidgetState::AfterPush:
				OnAfterPush.Broadcast(PushWidget);
				if (bCachedFocus)
				{
					if (UWidget* WidgetToFocus = PushWidget->GetDesiredFocusTarget())
					{
						WidgetToFocus->SetFocus();
					}
				}
				SetReadyToDestroy();
				break;
			default:
				break;
			}
		}
	);
}
