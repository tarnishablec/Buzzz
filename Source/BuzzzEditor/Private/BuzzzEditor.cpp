#include "BuzzzEditor.h"

#include "Core/Fragment/BuzzzFragment.h"
#include "Fragment/BuzzzFragmentCustomization.h"

#define LOCTEXT_NAMESPACE "FBuzzzEditorModule"

void FBuzzzEditorModule::StartupModule()
{
    auto& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomClassLayout(UBuzzzFragment::StaticClass()->GetFName(),
                                             FOnGetDetailCustomizationInstance::CreateStatic(
                                                 &FBuzzzFragmentCustomization::MakeInstance));
}

void FBuzzzEditorModule::ShutdownModule()
{
    auto& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.UnregisterCustomClassLayout(UBuzzzFragment::StaticClass()->GetFName());
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBuzzzEditorModule, BuzzzEditor)
