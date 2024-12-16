#include "MetasoundEchoes/Public/MetasoundEchoes.h"
#include "MetasoundFrontendRegistries.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FMetasoundEchoesModule"

void FMetasoundEchoesModule::StartupModule()
{
    // Initialization
    // UE_LOG(LogTemp, Log, TEXT("MetasoundEchoes module started..."));
}

void FMetasoundEchoesModule::ShutdownModule()
{
    // Cleanup
    // UE_LOG(LogTemp, Log, TEXT("MetasoundEchoes module shutting down..."));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMetasoundEchoesModule, MetasoundEchoes);