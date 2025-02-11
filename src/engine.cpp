#include "engine.h"

#include "globals.h"
#include "imGuiMenus.h"
#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"
#include "logger.h"
#include <algorithm>
#include <fstream>
#include <string>


namespace engine {

	std::fstream logfile;
	FILE logfile_c;
	bool running = false;

	const char* SETTINGS_FILE = "settings.ini";

	void loadDefaultSettings() {
		globals::width = 1080;
		globals::height = 720;
		globals::fps = 30;
		globals::vsync = true;
		globals::fullscreen = false;
		globals::borderless = false;
		globals::maximized = false;
	}

	void loadSettings() {
		std::ifstream file(SETTINGS_FILE);
		// Load Default settings if file could not be found or is corrupted
		if (!file.is_open() || !file.good()) {
			loadDefaultSettings();
			return;
		}
		// Reading file string by string
		std::string str;
		while (file >> str) {
			// Lower the read string
			std::transform(str.begin(), str.end(), str.begin(), ::tolower);
			// Checking what option is loaded
			if (str == "resolution") {
				std::string tmp, val;
				file >> tmp >> val;
				tmp = "";
				for (auto&& c : val) {
					if (c == 'x') {
						globals::width = std::stoi(tmp);
						tmp = "";
						continue;
					}
					tmp += c;
				}
				globals::height = std::stoi(tmp);
			}
			else if (str == "fps") {
				std::string tmp, val;
				file >> tmp >> val;
				globals::fps = std::stoi(val);
			}
			else if (str == "vsync") {
				std::string tmp, val;
				file >> tmp >> val;
				std::transform(val.begin(), val.end(), val.begin(), ::tolower);
				if (val == "true") {
					globals::vsync = true;
				}
				else if (val == "false") {
					globals::vsync = false;
				}
				else
					globals::vsync = std::stoi(val);
			}
			else if (str == "fullscreen") {
				std::string tmp, val;
				file >> tmp >> val;
				std::transform(val.begin(), val.end(), val.begin(), ::tolower);
				if (val == "true")
					globals::fullscreen = true;
				else if (val == "false")
					globals::fullscreen = false;
				else
					globals::fullscreen = std::stoi(val);
			}
			else if (str == "borderless") {
				std::string tmp, val;
				file >> tmp >> val;
				std::transform(val.begin(), val.end(), val.begin(), ::tolower);
				if (val == "true")
					globals::borderless = true;
				else if (val == "false")
					globals::borderless = true;
				else
					globals::borderless = std::stoi(val);
			}
			else if (str == "maximized") {
				std::string tmp, val;
				file >> tmp >> val;
				std::transform(val.begin(), val.end(), val.begin(), ::tolower);
				if (val == "true")
					globals::maximized = true;
				else if (val == "false")
					globals::maximized = false;
				else
					globals::maximized = std::stoi(val);
			}
			else if (str == "file_path") {
				std::string tmp;
				std::filesystem::path val;
				file >> tmp >> val;
				globals::filePath = val.string();
				logger::log("LOADING", "Loaded file Path: " + globals::filePath);
			}
		}
	}

	void saveSettings() {
		std::ofstream file(SETTINGS_FILE);
		if (file.is_open() && file.good()) {
			// Saving the Resolution
			file << "Resolution = " << std::to_string(globals::width) << "x"
				<< std::to_string(globals::height) << "\n";
			// Saving FPS:
			file << "FPS = " << std::to_string(globals::fps) << "\n";
			// Saving vsync hint:
			file << "VSYNC = " << std::to_string(globals::vsync) << "\n";
			// Saving window states
			file << "Fullscreen = " << std::to_string(globals::fullscreen) << "\n";
			file << "Borderless = " << std::to_string(globals::borderless) << "\n";
			file << "Maximized = " << std::to_string(globals::maximized) << "\n";
			std::filesystem::path p(globals::filePath);
			file << "File_Path = " << p;
		}
	}

	void loadGuiStyle() {
		ImGuiStyle& style = ImGui::GetStyle();
		std::ifstream file("imgui.style");
		if (file.is_open() && file.good()) {
			file.read((char*)&style, sizeof(ImGuiStyle));
		}
	}

	void saveGuiStyle() {
		ImGuiStyle& style = ImGui::GetStyle();
		std::ofstream file("imgui.style");
		if (file.is_open() && file.good()) {
			file.write((char*)&style, sizeof(ImGuiStyle));
		}
	}

	void setup(bool logging) {
		if (logging) {
			//logger::startlogging(logfile, true);
			logger::startlogging_c(logfile_c, true);
		}

  logger::log("BMG-Keylogger", VERSION);
#ifdef DEBUG
  logger::log("DEBUG", "Running in Debug Mode!");
#endif

		loadSettings();
		SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
		if (globals::vsync)
			SetConfigFlags(FLAG_VSYNC_HINT);
		InitWindow(globals::width, globals::height, "BMG-Keylogger");
		SetWindowIcon(LoadImage("BMG-Keylogger.png"));
		HandleWindowResized();
		rlImGuiSetup(true);
		auto& imguiIO = ImGui::GetIO();
		imguiIO.IniFilename = NULL;
		imguiIO.LogFilename = NULL;
		loadGuiStyle();
		globals::running = true;
		running = true;
	}

	void input() {
		if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_W))
			globals::running = false;
	}

	void update() { float deltatime = GetFrameTime(); }

	void render() {
		BeginDrawing();
		ClearBackground(BLACK);
		rlImGuiBegin();

		HandleMainMenuBar();
		HandleMainWindow();

		rlImGuiEnd();
		DrawFPS(GetScreenWidth() - 90, 0);
		EndDrawing();
	}

	void run() {
		while (!WindowShouldClose() && globals::running) {
			input();
			update();
			render();
			if (IsWindowResized()) {
				globals::wasResized = true;
				if (IsWindowMaximized())
					globals::maximized = true;
				if (!IsWindowMaximized()) {
					globals::width = GetScreenWidth();
					globals::height = GetScreenHeight();
					globals::maximized = false;
				}
				globals::fullscreen = false;
				globals::borderless = false;
			}
			if (globals::wasResized && IsMouseButtonUp(MOUSE_BUTTON_LEFT))
				HandleWindowResized();
		}
	}

	void shutdown() {
		running = false;
		saveGuiStyle();
		rlImGuiShutdown();
		CloseWindow();
		logfile.close();
	}

	void HandleWindowResized() {
		globals::wasResized = false;
		// Setting up fps
		SetTargetFPS(globals::fps);
		// Handle VSYNC hint
		if (globals::vsync)
			SetWindowState(FLAG_VSYNC_HINT);
		else
			ClearWindowState(FLAG_VSYNC_HINT);
		// Get current Monitor
		int device = GetCurrentMonitor();
		// Handle Screen modes
		if (globals::fullscreen) {
			if (IsWindowMaximized()) {
				RestoreWindow();
			}
			if (IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE)) {
				ToggleBorderlessWindowed();
			}
			SetWindowSize(GetMonitorWidth(device), GetMonitorHeight(device));
			if (!IsWindowFullscreen())
				ToggleFullscreen();
		}
		else if (globals::borderless) {
			if (IsWindowMaximized()) {
				RestoreWindow();
			}
			if (IsWindowFullscreen()) {
				ToggleFullscreen();
				SetWindowSize(globals::width, globals::height);
			}
			if (!IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE)) {
				SetWindowSize(GetMonitorWidth(device), GetMonitorHeight(device));
				ToggleBorderlessWindowed();
			}
		}
		else if (globals::maximized) {
			if (IsWindowFullscreen()) {
				ToggleFullscreen();
				SetWindowSize(globals::width, globals::height);
			}
			if (IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE)) {
				ToggleBorderlessWindowed();
				SetWindowSize(globals::width, globals::height);
			}
			if (!IsWindowMaximized()) {
				MaximizeWindow();
			}
		}
		else {
			if (globals::width >= GetMonitorWidth(device))
				globals::width = GetMonitorWidth(device);
			if (globals::height >= GetMonitorHeight(device))
				globals::height = GetMonitorHeight(device);
			if (globals::width < MIN_WIDTH)
				globals::width = MIN_WIDTH;
			if (globals::height < MIN_HEIGHT)
				globals::height = MIN_HEIGHT;
			if (globals::width == GetMonitorWidth(device) &&
				globals::height == GetMonitorHeight(device)) {
				globals::maximized = true;
				globals::fullscreen = false;
				globals::borderless = false;
				HandleWindowResized();
				return;
			}
			if (IsWindowMaximized())
				RestoreWindow();
			if (IsWindowFullscreen())
				ToggleFullscreen();
			if (IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE))
				ToggleBorderlessWindowed();
			SetWindowSize(globals::width, globals::height);
			SetWindowPosition(GetMonitorWidth(device) / 2 - globals::width / 2,
				GetMonitorHeight(device) / 2 - globals::height / 2);
		}
	}

	bool isRunning() {
		return running;
	}
}