#include "BuzzzEditor.h"

#include "Buzzz/Core/Fragment/BuzzzFragment.h"
#include "BuzzzEditor/Fragment/BuzzzFragmentCustomization.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FBuzzzEditorModule"

TSharedPtr<FSlateStyleSet> StyleSet = nullptr;

void FBuzzzEditorModule::StartupModule()
{
    auto& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomClassLayout(UBuzzzFragment::StaticClass()->GetFName(),
                                             FOnGetDetailCustomizationInstance::CreateStatic(
                                                 &FBuzzzFragmentCustomization::MakeInstance));

    StyleSet = MakeShareable(new FSlateStyleSet("BuzzzStyleSet"));
    const FString ResourcePath = IPluginManager::Get().FindPlugin("Buzzz")->GetBaseDir() / TEXT("Resources");
    StyleSet->Set("ClassThumbnail.BuzzzContainer",
                  new FSlateImageBrush(ResourcePath / TEXT("Hive.png"), FVector2D(256, 256)));
    FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
}

void FBuzzzEditorModule::ShutdownModule()
{
    auto& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.UnregisterCustomClassLayout(UBuzzzFragment::StaticClass()->GetFName());

    FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
    StyleSet.Reset();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBuzzzEditorModule, BuzzzEditor)
