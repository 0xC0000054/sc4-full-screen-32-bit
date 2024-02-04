# sc4-full-screen-32-bit

A DLL Plugin for SimCity 4 that makes the DirectX full screen mode use 32-bit color.   

This is an alternative version of my DirectX full screen mode 32-bit color fix for people who are already using
an external launcher or a shortcut with command line arguments to configure the game's graphics settings.

If you are using my [GraphicsOptions](https://github.com/0xC0000054/sc4-graphics-options) DLL Plugin you do not need to install this mod.   
The two mods will conflict, but the worst thing that could happen is that the fix is applied more than once.

The plugin can be downloaded from the Releases tab: https://github.com/0xC0000054/sc4-full-screen-32-bit/releases


## Technical Details

For some reason Maxis hard-coded the DirectX full screen mode to use 16-bit color.
There were existing fixes for this issue, but those required replacing or modifying `SimCity 4.exe`.
This mod patches the affected code in the game's memory and does not modify the original executable.

## System Requirements

* SimCity 4 version 641
* Windows 10 or later

The plugin may work on Windows 7 or later with the [Microsoft Visual C++ 2022 x86 Redistribute](https://aka.ms/vs/17/release/vc_redist.x86.exe) installed, but I do not have the ability to test that.

## Installation

1. Close SimCity 4.
2. Copy `SC4FullScreen32Bit.dll` into the Plugins folder in the SimCity 4 installation directory.
3. Start SimCity 4.

## Troubleshooting

The plugin should write a `SC4FullScreen32Bit.log` file in the same folder as the plugin.    
The log contains status information for the most recent run of the plugin.

# License

This project is licensed under the terms of the MIT License.    
See [LICENSE.txt](LICENSE.txt) for more information.

## 3rd party code

[gzcom-dll](https://github.com/nsgomez/gzcom-dll/tree/master) Located in the vendor folder, MIT License.    
[EABase](https://github.com/electronicarts/EABase) Located in the vendor folder, BSD 3-Clause License.    
[EASTL](https://github.com/electronicarts/EASTL) Located in the vendor folder, BSD 3-Clause License.    
[SC4Fix](https://github.com/nsgomez/sc4fix) - MIT License.     
[Windows Implementation Library](https://github.com/microsoft/wil) - MIT License    

# Source Code

## Prerequisites

* Visual Studio 2022

## Building the plugin

* Open the solution in the `src` folder
* Update the post build events to copy the build output to you SimCity 4 application plugins folder.
* Build the solution

## Debugging the plugin

Visual Studio can be configured to launch SimCity 4 on the Debugging page of the project properties.
I configured the debugger to launch the game in full screen with the following command line:    
`-intro:off -CPUcount:1 -f -CustomResolution:enabled -r2560x1440x32`

You will need to have the debugger open on a second screen, and you may need to adjust the resolution for your primary screen.
