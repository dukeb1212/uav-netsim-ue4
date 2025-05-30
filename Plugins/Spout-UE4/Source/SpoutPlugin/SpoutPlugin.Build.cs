// Some copyright should be here...

using System;
using System.IO;

namespace UnrealBuildTool.Rules
{
    public class SpoutPlugin : ModuleRules
    {

        private string ModulePath {
            get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../")); }
        }

        private string ThirdPartyPath {
            get { return Path.GetFullPath(Path.Combine(ModulePath, "ThirdParty/")); }
        }

        public string GetUProjectPath()
        {
            return Path.Combine(ModuleDirectory, "../../../..");
        }

        private string CopyToProjectBinaries(string Filepath, ReadOnlyTargetRules Target)
        {
            string BinariesDir = Path.Combine(GetUProjectPath(), "Binaries", Target.Platform.ToString());
            string Filename = Path.GetFileName(Filepath);

            //convert relative path 
            string FullBinariesDir = Path.GetFullPath(BinariesDir);

            if (!Directory.Exists(FullBinariesDir))
            {
                Directory.CreateDirectory(FullBinariesDir);
            }

            string FullExistingPath = Path.Combine(FullBinariesDir, Filename);
            bool ValidFile = false;

            //File exists, check if they're the same
            if (File.Exists(FullExistingPath))
            {
                ValidFile = true;
            }

            //No valid existing file found, copy new dll
            if (!ValidFile)
            {
                File.Copy(Filepath, Path.Combine(FullBinariesDir, Filename), true);
            }
            return FullExistingPath;
        }

        public SpoutPlugin(ReadOnlyTargetRules Target) : base(Target)
        //public SpoutPlugin(TargetInfo Target) : base(Target)
        {
            PrivatePCHHeaderFile = "Private/SpoutPluginPrivatePCH.h";

            PublicDefinitions.Add("_HAS_EXCEPTIONS=1");
            bEnableExceptions = true;

            PublicIncludePaths.AddRange(
                new string[] {
                    Path.Combine(ModuleDirectory, "Public"),
                    // SMODE TECH Embedd Spout code and do not link on external Spout Library
                    // Path.Combine(ThirdPartyPath, "Spout/include")
                    
                    // ... add public include paths required here ...
                }
                );
            

            PrivateIncludePaths.AddRange(
                new string[] {
                    "SpoutPlugin/Private",
                    // ... add other private include paths required here ...
                }
                );

            //AddThirdPartyPrivateStaticDependencies(Target, "Spout");

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "RHI",
                    "RenderCore",
                    "Projects"
                    // ... add other public dependencies that you statically link with here ...
                }
                );

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "Slate", "SlateCore"
                    // ... add private dependencies that you statically link with here ...  
                }
                );


            DynamicallyLoadedModuleNames.AddRange(
                new string[]
                {
                    
                    // ... add any modules that your module loads dynamically here ...
                }
                );

            // Implemented this method for copying DLL to packaged project's Binaries folder
            // https://answers.unrealengine.com/questions/842286/specify-dll-location-using-plugin-in-cooked-projec.html
            //if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
            {
            // SMODE TECH: Do not link on external Spout library anymore
            //    string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "amd64" : "x86";

            //    PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "Spout", "include"));
            //    PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "Spout", "lib", PlatformString, "SpoutLibrary.lib"));

            //    string pluginDLLPath = Path.Combine(ThirdPartyPath, "Spout", "lib", PlatformString, "SpoutLibrary.dll");
            //    string binariesPath = CopyToProjectBinaries(pluginDLLPath, Target);
            //    System.Console.WriteLine("Using Spout DLL: " + binariesPath);
            //    RuntimeDependencies.Add(binariesPath);
            }
        }
    }
}
