// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class UAVNetSim : ModuleRules
{
	public UAVNetSim(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Json", "JsonUtilities", "AirSim" });

        string EnvFilePath = Path.Combine(ModuleDirectory, "..", "..", ".env");

        string VcpkgRootPath = "E:/UnrealProjects/UAVNetSim/vcpkg_installed";

        if (File.Exists(EnvFilePath))
        {
            foreach (string line in File.ReadAllLines(EnvFilePath))
            {
                string[] parts = line.Split('=');
                if (parts.Length == 2 && parts[0].Trim() == "VCPKG_ROOT")
                {
                    VcpkgRootPath = parts[1].Trim();
                    break;
                }
            }
        }
        // Path to ZeroMQ include and library directories installed by vcpkg
        string VcpkgIncludePath = Path.Combine(VcpkgRootPath, "x64-windows", "include");
        string VcpkgLibraryPath = Path.Combine(VcpkgRootPath, "x64-windows", "lib");
        string VcpkgBinariesPath = Path.Combine(VcpkgRootPath, "x64-windows", "bin");
        RuntimeDependencies.Add(Path.Combine(VcpkgBinariesPath, "libzmq-mt-4_3_5.dll"));

        PublicIncludePaths.Add(VcpkgIncludePath);
        PublicLibraryPaths.Add(VcpkgLibraryPath);
        PublicAdditionalLibraries.Add(Path.Combine(VcpkgLibraryPath, "libzmq-mt-4_3_5.lib"));


        PrivateDependencyModuleNames.AddRange(new string[] {  });

        bEnableExceptions = true;

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
