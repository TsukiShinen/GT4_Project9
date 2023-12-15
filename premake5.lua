workspace "GT_ProjectDirectX"
    architecture "x86_64"
    startproject "Engine"

    configurations 
    { 
        "Debug", 
        "Release", 
        "Dist" 
    }

    flags 
    {
		"MultiProcessorCompile"
	}
    
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "Engine"