#include "BuzzzEditor.h"

#include "Buzzz/Core/Fragment/BuzzzFragment.h"
#include "BuzzzEditor/Fragment/BuzzzFragmentCustomization.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FBuzzzEditorModule"


void FBuzzzEditorModule::StartupModule()
{
    auto& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomClassLayout(UBuzzzFragment::StaticClass()->GetFName(),
                                             FOnGetDetailCustomizationInstance::CreateStatic(
                                                 &FBuzzzFragmentCustomization::MakeInstance));

    BuzzzStyleSet = MakeShareable(new FSlateStyleSet("BuzzzStyleSet"));
    const FString ResourcePath = IPluginManager::Get().FindPlugin("Buzzz")->GetBaseDir() / TEXT("Resources");
    BuzzzStyleSet->Set("ClassThumbnail.BuzzzContainer",
                       new FSlateImageBrush(ResourcePath / TEXT("Hive.png"),
                                            FVector2D(512, 512)));
    BuzzzStyleSet->Set("ClassThumbnail.BuzzzItem",
                      new FSlateImageBrush(ResourcePath / TEXT("Bee.png"),
                                           FVector2D(512, 512)));


    
    FSlateStyleRegistry::RegisterSlateStyle(*BuzzzStyleSet.Get());
}

void FBuzzzEditorModule::ShutdownModule()
{
    auto& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.UnregisterCustomClassLayout(UBuzzzFragment::StaticClass()->GetFName());
    FSlateStyleRegistry::UnRegisterSlateStyle(*BuzzzStyleSet.Get());
    BuzzzStyleSet.Reset();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBuzzzEditorModule, BuzzzEditor)
