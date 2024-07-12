// Some copyright should be here...

using UnrealBuildTool;

using System.IO; // to search additional files

public class FFMPEG : ModuleRules
{
	public FFMPEG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);

		bEnableUndefinedIdentifierWarnings = false;
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);


        // going level upper
        string parentDirectory = Path.GetDirectoryName(ModuleDirectory);

        string ParentPath = Path.Combine(parentDirectory, "ThirdParty");

 
                foreach (string File in Directory.EnumerateFiles(ParentPath, "*.*", SearchOption.AllDirectories))
                {
                    RuntimeDependencies.Add(File);
                }
        
    }
}
