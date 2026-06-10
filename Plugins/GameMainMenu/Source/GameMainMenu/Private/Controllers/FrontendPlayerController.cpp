// HangHang All Rights Reserved


#include "Controllers/FrontendPlayerController.h"
#include "FrontendSettings/FrontendGameUserSettings.h"

void AFrontendPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	RunHardwareBenchmarkAndApply();
}

void AFrontendPlayerController::RunHardwareBenchmarkAndApply()
{
	UFrontendGameUserSettings* UserSettings = UFrontendGameUserSettings::Get();
	if (!UserSettings) return;
	
	if (UserSettings->GetLastCPUBenchmarkResult() == -1.f || UserSettings->GetLastGPUBenchmarkResult() == -1.f)
	{
		UserSettings->RunHardwareBenchmark();
		UserSettings->ApplyHardwareBenchmarkResults();
	}
}
