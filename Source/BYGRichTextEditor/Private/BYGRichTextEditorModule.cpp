// Copyright Brace Yourself Games. All Rights Reserved.

#include "BYGRichTextEditorModule.h"

#include "PropertyEditorModule.h"
#include "ISettingsModule.h"
#include "Framework/Docking/TabManager.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructure.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"

#include "BYGRichTextRuntimeSettings.h"
#include "Customization/BYGRichTextStyleCustomization.h"
#include "Customization/BYGRichTextStylesheetCustomization.h"
#include "BYGRichTextUIStyle.h"
#include "TestWindow/BYGRichTextTestWindow.h"
#include "Customization/BYGStyleDisplayTypeCustomization.h"
#include "Customization/BYGTextJustifyCustomization.h"

#include "KismetCompilerModule.h"
#include "BYGStylesheetCompiler.h"

#define LOCTEXT_NAMESPACE "BYGRichTextEditorModule"

namespace BYGRichTextModule
{
	static const FName TestTabName = FName( TEXT( "Test" ) );
}

TSharedRef<SDockTab> SpawnTestTab( const FSpawnTabArgs& Args )
{
	return SNew( SDockTab )
		.Icon( FBYGRichTextUIStyle::GetBrush( "BYGRichText.TabIcon" ) )
		.TabRole( ETabRole::NomadTab )
		.Label( NSLOCTEXT( "BYGRichText", "TabTitle", "BYG Rich Text Test" ) )
		[
			SNew( SBYGRichTextTestWindow )
		];
}

void FBYGRichTextEditor::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked< FPropertyEditorModule >( "PropertyEditor" );

	PropertyModule.RegisterCustomClassLayout( "BYGRichTextStylesheet", FOnGetDetailCustomizationInstance::CreateStatic( &FBYGRichTextStylesheetCustomization::MakeInstance ) );
	PropertyModule.RegisterCustomClassLayout( "BYGRichTextStyle", FOnGetDetailCustomizationInstance::CreateStatic( &FBYGRichTextStyleCustomization::MakeInstance ) );

	PropertyModule.NotifyCustomizationModuleChanged();

	// Register widget blueprint compiler we do this no matter what.
	IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>( "KismetCompiler" );
	KismetCompilerModule.GetCompilers().Add( &StylesheetCompiler );

	FKismetCompilerContext::RegisterCompilerForBP( UBlueprint::StaticClass(), []( UBlueprint* InBlueprint, FCompilerResultsLog& InMessageLog, const FKismetCompilerOptions& InCompileOptions )
	{
		return MakeShared<FBYGStylesheetCompilerContext>( CastChecked<UBlueprint>( InBlueprint ), InMessageLog, InCompileOptions );
	} );


	FBYGRichTextUIStyle::Initialize();

	FCoreDelegates::OnPostEngineInit.AddRaw( this, &FBYGRichTextEditor::OnPostEngineInit );

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner( BYGRichTextModule::TestTabName, FOnSpawnTab::CreateStatic( &SpawnTestTab ) )
		.SetDisplayName( NSLOCTEXT( "BYGRichText", "TestTab", "BYG Rich Text Test" ) )
		.SetTooltipText( NSLOCTEXT( "BYGRichText", "TestTooltipText", "Open the BYG Rich Text Test tab." ) )
		.SetGroup( WorkspaceMenu::GetMenuStructure().GetDeveloperToolsMiscCategory() )
		.SetIcon( FSlateIcon( FBYGRichTextUIStyle::GetStyleSetName(), "BYGRichText.TabIcon" ) );
}

void FBYGRichTextEditor::OnPostEngineInit()
{
	if ( ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" ) )
	{
		SettingsModule->RegisterSettings( "Project", "Plugins", "BYGRichText",
			LOCTEXT( "RuntimeSettingsName", "BYG Rich Text" ),
			LOCTEXT( "RuntimeSettingsDescription", "Configure the BYG Rich Text plugin" ),
			GetMutableDefault<UBYGRichTextRuntimeSettings>() );
	}
}

void FBYGRichTextEditor::ShutdownModule()
{
	FDefaultGameModuleImpl::ShutdownModule();

	if ( FSlateApplication::IsInitialized() )
	{
		FGlobalTabmanager::Get()->UnregisterNomadTabSpawner( BYGRichTextModule::TestTabName );
	}


	IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>( "KismetCompiler" );
	KismetCompilerModule.GetCompilers().Remove( &StylesheetCompiler );

	auto& PropertyModule = FModuleManager::LoadModuleChecked< FPropertyEditorModule >( "PropertyEditor" );
	PropertyModule.UnregisterCustomClassLayout( "BYGRichTextStylesheet" );
	PropertyModule.UnregisterCustomClassLayout( "BYGRichTextStyle" );
	PropertyModule.UnregisterCustomClassLayout( "BYGRichTextBlock" );

	FBYGRichTextUIStyle::Shutdown();

	if ( UObjectInitialized() )
	{
		if ( ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" ) )
		{
			SettingsModule->UnregisterSettings( "Project", "Plugins", "BYGRichText" );
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_GAME_MODULE( FBYGRichTextEditor, BYGRichTextEditor );


