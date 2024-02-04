////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-full-screen-32-bit, a DLL Plugin for SimCity 4
// that makes the DirectX full screen mode use 32-bit color.
//
// Copyright (c) 2024 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
////////////////////////////////////////////////////////////////////////////

#include "version.h"
#include "Logger.h"
#include "SC4VersionDetection.h"
#include "cIGZApp.h"
#include "cIGZCOM.h"
#include "cIGZFrameWork.h"
#include "cRZAutoRefCount.h"
#include "cRZBaseString.h"
#include "cRZCOMDllDirector.h"
#include "cISC4App.h"
#include "SC4Preferences.h"

#include <filesystem>
#include <memory>
#include <string>
#include <Windows.h>
#include "wil/resource.h"
#include "wil/win32_helpers.h"

#include "EASTLConfigSC4.h"
#include "EASTL\vector.h"

static constexpr uint32_t kFullScreen32BitDirectorID = 0x1C859C1D;

static constexpr std::string_view PluginLogFileName = "SC4FullScreen32Bit.log";

namespace
{
	void OverwriteMemory(uintptr_t address, uint8_t newValue)
	{
		DWORD oldProtect;
		// Allow the executable memory to be written to.
		THROW_IF_WIN32_BOOL_FALSE(VirtualProtect(
			reinterpret_cast<LPVOID>(address),
			sizeof(newValue),
			PAGE_EXECUTE_READWRITE,
			&oldProtect));

		// Patch the memory at the specified address.
		*((uint8_t*)address) = newValue;
	}
}

class FullScreen32BitDllDirector : public cRZCOMDllDirector
{
public:

	FullScreen32BitDllDirector()
	{
		std::filesystem::path dllFolderPath = GetDllFolderPath();

		std::filesystem::path logFilePath = dllFolderPath;
		logFilePath /= PluginLogFileName;

		Logger& logger = Logger::GetInstance();
		logger.Init(logFilePath, LogLevel::Error);
		logger.WriteLogFileHeader("SC4FullScreen32Bit v" PLUGIN_VERSION_STR);
	}

	uint32_t GetDirectorID() const
	{
		return kFullScreen32BitDirectorID;
	}

	bool OnStart(cIGZCOM* pCOM)
	{
		FixFullScreen32BitColorDepth();

		return true;
	}

private:

	void FixFullScreen32BitColorDepth()
	{
		// Maxis hard-coded the DirectX driver to use 16-bit color depth when in full screen mode, so we patch the
		// game's memory to fix that.
		// This fix is based on the patched SimCity 4 executable at https://github.com/dege-diosg/dgVoodoo2/issues/3

		Logger& logger = Logger::GetInstance();

		cIGZFrameWork* const pFramework = RZGetFrameWork();

		cIGZApp* const pApp = pFramework->Application();

		if (pApp)
		{
			cRZAutoRefCount<cISC4App> pSC4App;

			if (pApp->QueryInterface(GZIID_cISC4App, pSC4App.AsPPVoid()))
			{
				const SC4Preferences* prefs = pSC4App->GetPreferences();
				const SC4VideoPreferences& videoPrefs = prefs->videoPreferences;

				if (videoPrefs.bFullScreen && videoPrefs.bitDepth == 32)
				{
					const uint16_t gameVersion = SC4VersionDetection::GetInstance().GetGameVersion();

					if (gameVersion == 641)
					{
						try
						{
							// Replace the hard-coded value of 16 with 32.
							OverwriteMemory(0x887738, 32);
							logger.WriteLine(LogLevel::Info, "Forced the DirectX full screen color depth to 32-bit.");
						}
						catch (const std::exception& e)
						{
							logger.WriteLineFormatted(
								LogLevel::Error,
								"Failed to force the DirectX full screen color depth to 32-bit: %s",
								e.what());
						}
					}
					else
					{
						logger.WriteLineFormatted(
							LogLevel::Error,
							"Unable to force the DirectX full screen color depth to 32-bit. Requires "
							"game version 641, found game version %d.",
							gameVersion);
					}
				}
				else
				{
					logger.WriteLine(
						LogLevel::Info,
						"Skipped because the game is running in windowed mode or the user selected 16-bit color.");
				}
			}
		}
	}

	std::filesystem::path GetDllFolderPath()
	{
		wil::unique_cotaskmem_string modulePath = wil::GetModuleFileNameW(wil::GetModuleInstanceHandle());

		std::filesystem::path temp(modulePath.get());

		return temp.parent_path();
	}
};

cRZCOMDllDirector* RZGetCOMDllDirector() {
	static FullScreen32BitDllDirector sDirector;
	return &sDirector;
}