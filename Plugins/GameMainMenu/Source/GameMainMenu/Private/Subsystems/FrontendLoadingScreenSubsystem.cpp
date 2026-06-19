// HangHang All Rights Reserved


#include "Subsystems/FrontendLoadingScreenSubsystem.h"

#include "PreLoadScreenManager.h"
#include "Blueprint/UserWidget.h"
#include "FrontendGameplayTags.h"
#include "FrontendSettings/FrontendLoadingScreenSettings.h"
#include "Interface/FrontendLoadingScreenInterface.h"

bool UFrontendLoadingScreenSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> FoundClass;
		GetDerivedClasses(GetClass(), FoundClass);

		return FoundClass.IsEmpty();
	}

	return false;
}

void UFrontendLoadingScreenSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FCoreUObjectDelegates::PreLoadMapWithContext.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	TryRemoveLoadingScreen();
}

void UFrontendLoadingScreenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &UFrontendLoadingScreenSubsystem::OnMapPreLoad);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UFrontendLoadingScreenSubsystem::OnMapPostLoad);
}

UWorld* UFrontendLoadingScreenSubsystem::GetTickableGameObjectWorld() const
{
	if (UGameInstance* OwningGameInstance = GetGameInstance())
	{
		return OwningGameInstance->GetWorld();
	}

	return nullptr;
}

void UFrontendLoadingScreenSubsystem::Tick(float DeltaTime)
{
	TryUpdateLoadingScreen();
}

ETickableTickType UFrontendLoadingScreenSubsystem::GetTickableTickType() const
{
	if (IsTemplate())
	{
		return ETickableTickType::Never;
	}

	return ETickableTickType::Conditional;
}

bool UFrontendLoadingScreenSubsystem::IsTickable() const
{
	return GetGameInstance() && GetGameInstance()->GetGameViewportClient();
}

TStatId UFrontendLoadingScreenSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UFrontendLoadingScreenSubsystem,STATGROUP_Tickables);
}

UFrontendLoadingScreenSubsystem::~UFrontendLoadingScreenSubsystem()
{
}

void UFrontendLoadingScreenSubsystem::OnMapPreLoad(const FWorldContext& WorldContext, const FString& MapName)
{
	if (WorldContext.OwningGameInstance != GetGameInstance()) return;

	SetTickableTickType(ETickableTickType::Conditional);

	bIsCurrentlyLoadingMap = true;

	HoldLoadingScreenStartUpTime = -1.0;

	TryUpdateLoadingScreen();
}

void UFrontendLoadingScreenSubsystem::OnMapPostLoad(UWorld* LoadedWorld)
{
	if (LoadedWorld && LoadedWorld->GetGameInstance() == GetGameInstance())
	{
		bIsCurrentlyLoadingMap = false;

		TryUpdateLoadingScreen();
	}
}

void UFrontendLoadingScreenSubsystem::TryUpdateLoadingScreen()
{
	if (IsPreLoadScreenActive())
	{
		return;
	}

	if (ShouldShowLoadingScreen())
	{
		UGameViewportClient* ViewportClient = GetGameInstance()->GetGameViewportClient();
		if (ViewportClient)
		{
			const bool bInActualLoadingPhase =
				CurrentLoadingPhase != FrontendGameplayTags::Frontend_Loading_Phase_TextureStreaming;
			ViewportClient->bDisableWorldRendering = bInActualLoadingPhase;
		}

		TryDisplayLoadingScreenIfNone();

		OnLoadingPhaseUpdated.Broadcast(CurrentLoadingPhase);
	}
	else
	{
		TryRemoveLoadingScreen();

		HoldLoadingScreenStartUpTime = -1.0;

		NotifyLoadingScreenVisibilityChanged(false);

		SetTickableTickType(ETickableTickType::Never);
	}
}

bool UFrontendLoadingScreenSubsystem::IsPreLoadScreenActive() const
{
	if (FPreLoadScreenManager* PreLoadScreenManager = FPreLoadScreenManager::Get())
	{
		return PreLoadScreenManager->HasValidActivePreLoadScreen();
	}

	return false;
}

bool UFrontendLoadingScreenSubsystem::ShouldShowLoadingScreen()
{
	const UFrontendLoadingScreenSettings* LoadScreenSettings = GetDefault<UFrontendLoadingScreenSettings>();

	if (GIsEditor && !LoadScreenSettings->bShowLoadingScreenInEditor) return false;

	UGameViewportClient* ViewportClient = GetGameInstance()->GetGameViewportClient();
	if (!ViewportClient) return false;

	if (CheckIsNeedShowLoadingScreen())
	{
		return true;
	}

	const double CurrentTime = FPlatformTime::Seconds();

	if (HoldLoadingScreenStartUpTime < 0.0)
	{
		HoldLoadingScreenStartUpTime = CurrentTime;
	}

	const double ElapsedTime = CurrentTime - HoldLoadingScreenStartUpTime;
	return ElapsedTime < LoadScreenSettings->HoldLoadingScreenTime;
}

bool UFrontendLoadingScreenSubsystem::CheckIsNeedShowLoadingScreen()
{
	if (bIsCurrentlyLoadingMap)
	{
		CurrentLoadingPhase = FrontendGameplayTags::Frontend_Loading_Phase_LevelLoad;
		return true;
	}

	UWorld* OwningWorld = GetGameInstance()->GetWorld();

	if (!OwningWorld)
	{
		CurrentLoadingPhase = FrontendGameplayTags::Frontend_Loading_Phase_WorldInit;
		return true;
	}

	if (!OwningWorld->HasBegunPlay())
	{
		CurrentLoadingPhase = FrontendGameplayTags::Frontend_Loading_Phase_BeginPlay;
		return true;
	}

	if (!OwningWorld->GetFirstPlayerController())
	{
		CurrentLoadingPhase = FrontendGameplayTags::Frontend_Loading_Phase_PlayerController;
		return true;
	}

	//todo other check

	CurrentLoadingPhase = FrontendGameplayTags::Frontend_Loading_Phase_TextureStreaming;
	return false;
}

void UFrontendLoadingScreenSubsystem::TryDisplayLoadingScreenIfNone()
{
	if (CachedLoadingScreenWidget)
	{
		return;
	}
	
	const UFrontendLoadingScreenSettings* LoadScreenSettings = GetDefault<UFrontendLoadingScreenSettings>();

	TSubclassOf<UUserWidget> LoadWidgetClass = LoadScreenSettings->GetSoftLoadScreenWidgetClassCheck();

	UUserWidget* CreatedLoadingWidget = UUserWidget::CreateWidgetInstance(*GetGameInstance(),LoadWidgetClass,NAME_None);

	if (!ensure(CreatedLoadingWidget))
	{
		return;
	}

	CachedLoadingScreenWidget = CreatedLoadingWidget->TakeWidget();

	UGameViewportClient* ViewportClient = GetGameInstance()->GetGameViewportClient();
	if (!ViewportClient) return;

	ViewportClient->AddViewportWidgetContent(
		CachedLoadingScreenWidget.ToSharedRef(),1000
	);

	NotifyLoadingScreenVisibilityChanged(true);
}

void UFrontendLoadingScreenSubsystem::TryRemoveLoadingScreen()
{
	if (!CachedLoadingScreenWidget)
	{
		return ;
	}
	
	UGameViewportClient* ViewportClient = GetGameInstance()->GetGameViewportClient();
	if (ViewportClient)
	{
		ViewportClient->RemoveViewportWidgetContent(
			CachedLoadingScreenWidget.ToSharedRef()
		);
	}

	CachedLoadingScreenWidget.Reset();
}

void UFrontendLoadingScreenSubsystem::NotifyLoadingScreenVisibilityChanged(bool bIsVisible)
{
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance) return;

	for (ULocalPlayer* LocalPlayer : GameInstance->GetLocalPlayers())
	{
		if (!LocalPlayer)
		{
			continue;
		}

		if (APlayerController* PC = LocalPlayer->GetPlayerController(GameInstance->GetWorld()))
		{
			//PC
			if (PC->Implements<UFrontendLoadingScreenInterface>())
			{
				if (bIsVisible)
				{
					IFrontendLoadingScreenInterface::Execute_OnLoadingScreenActivated(PC);
				}
				else
				{
					IFrontendLoadingScreenInterface::Execute_OnLoadingScreenDeactivated(PC);
				}
			}
			//Pawn
			if (APawn* OwningPawn = PC->GetPawn())
			{
				if (OwningPawn->Implements<UFrontendLoadingScreenInterface>())
				{
					if (bIsVisible)
					{
						IFrontendLoadingScreenInterface::Execute_OnLoadingScreenActivated(OwningPawn);
					}
					else
					{
						IFrontendLoadingScreenInterface::Execute_OnLoadingScreenDeactivated(OwningPawn);
					}
				}
			}
			//todo other object to notify
			
		}
	}
}

FLoadingPhaseDisplayInfo UFrontendLoadingScreenSubsystem::GetLoadingPhaseDisplayInfo(FGameplayTag PhaseTag) const
{
	const UFrontendLoadingScreenSettings* LoadScreenSettings = GetDefault<UFrontendLoadingScreenSettings>();

	if (const ULoadingScreenPhaseMapping* PhaseMapping = LoadScreenSettings->PhaseMapping.Get())
	{
		return PhaseMapping->FindDisplayInfo(PhaseTag);
	}

	return ULoadingScreenPhaseMapping::StaticClass()->GetDefaultObject<ULoadingScreenPhaseMapping>()->FindDisplayInfo(PhaseTag);
}
