using UnrealBuildTool;

public class BYGRichTextEditor : ModuleRules
{
	public BYGRichTextEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePathModuleNames.AddRange(
			new string[] {
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"UnrealEd",
                "UMG",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
                "SlateCore",
				"BYGRichText",
                "EditorStyle",
				"SlateCore",
                "Projects",
				"PropertyEditor",
				"EditorSubsystem",
				"EditorWidgets",
				"DeveloperSettings",
				"RenderCore",
				"FunctionalTesting",
				"DataValidation",
				"KismetCompiler"
            }
        );

		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
			}
		);
	}
}
