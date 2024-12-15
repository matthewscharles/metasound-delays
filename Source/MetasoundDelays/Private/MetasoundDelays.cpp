#include "MetasoundDelays/Public/MetasoundDelays.h"
#include "MetasoundFrontendRegistries.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FMetasoundDelaysModule"

void FMetasoundDelaysModule::StartupModule()
{
    // Initialization
    // UE_LOG(LogTemp, Log, TEXT("MetasoundDelays module started..."));
}

void FMetasoundDelaysModule::ShutdownModule()
{
    // Cleanup
    // UE_LOG(LogTemp, Log, TEXT("MetasoundDelays module shutting down..."));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMetasoundDelaysModule, MetasoundDelays);