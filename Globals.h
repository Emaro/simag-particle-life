#pragma once

namespace Globals
{

	// Application
	static const int WINDOW_INIT_WIDTH = 1580;
	static const int WINDOW_INIT_HEIGHT = 920;

	static const int MAX_PARTICLE_SYSTEMS = 2;
	static const int MAX_TASKS = 5;

	// Graphics
	static const int  USE_MULTISAMPLING_SAMPLES = 2; // 0 - disabled, otherwise 2,4,8,16; disable it, if your system is slow
	static const bool FORCE_VSYNC = false;           // should be true for whole SimAg-course or FORCE_FRAMERATE must be set
	static const int  FORCE_FRAMERATE = 60;          // -1 to disable, but please keep it as it is for SimAg

	// GUI
	static const float GUI_FONT_SCALE = 1.0f; // 1.0f, 1.5f, 2.0f, 2.5f, ...
	static const bool  GUI_ENABLE_FILE_STREAM_FOR_PS = false;
	static const bool  GUI_ENABLE_FILE_MENU = false;
	static const bool  GUI_ENABLE_DEBUG_MENU = true;

	// Path to "./Data/"
#ifdef _WIN32
	static const char* PATH_DATA = "../Data/";
#else
	static const char* PATH_DATA = "../Data/";
#endif

}

// ----------------------------------------------------------------------------


#ifdef _MSC_VER
#pragma warning (disable: 4505) // unreferenced local function has been removed
#pragma warning (disable: 4101) // unreferenced local variable has been removed
#endif

// ----------------------------------------------------------------------------
