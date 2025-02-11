#pragma once

#include "globals.h"
#include "imgui.h"
#include "raylib.h"
#include "user.h"
#include "logger.h"
#include <cstring>
#include <string>
#include <iostream>
#include <filesystem>
#include <stdlib.h>

using namespace ImGui;

inline bool settings_open = false;
inline bool style_editor_open = false;
inline bool demo_window_open = false;

const int b_size = 20;

inline char firstNameBuff[b_size];
inline char lastNameBuff[b_size];

inline User slctdUser("", "");
inline int slctdID = 0;

void HandleMainMenuBar();
void HandleSettingsMenu();
void HandleMainWindow();
void HandleUserSelect();
void HandleSelectedUser();
void HandleNewUserPopup();

inline void HandleNewUserPopup() {
	if (BeginPopupModal("Neuer Nutzer", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		Text("Nachname:");
		SameLine();
		InputText("##Last Name", lastNameBuff, b_size);
		SameLine();
		Text("Vorname:");
		SameLine();
		InputText("##First Name", firstNameBuff, b_size);
		if (Button("Verwerfen")) {
			lastNameBuff[0] = '\0';
			firstNameBuff[0] = '\0';
			CloseCurrentPopup();
		}
		SameLine();
		if (Button("Anwenden")) {
			globals::users.emplace_back(lastNameBuff, firstNameBuff);
			lastNameBuff[0] = '\0';
			firstNameBuff[0] = '\0';
			slctdUser = globals::users.back();
			slctdID = (int)globals::users.size() - 1;
			logger::log("USER_CREATION", "Created new User: " + slctdUser.firstName + " " + slctdUser.lastName);
			CloseCurrentPopup();
		}
		EndPopup();
	}
}

inline void HandleUserSelect() {
	SeparatorText("Benutzer");
	if (Button("Neuer Nutzer")) {
		OpenPopup("Neuer Nutzer");
	}
	ImVec2 listboxSize = GetContentRegionAvail();
	listboxSize.y -= 19;
	BeginListBox("##Nutzer", listboxSize);
	for (int x = 0; x < globals::users.size(); x++) {
		const bool is_selected = (slctdID == x);
		std::string fullName = globals::users[x].lastName + " " + globals::users[x].firstName;
		if (Selectable(fullName.c_str(), is_selected)) {
			slctdID = x;
			slctdUser = globals::users[slctdID];
		}
	}
	EndListBox();
	HandleNewUserPopup();
}

inline void HandleSelectedUser() {
	SeparatorText("Ausgewaehlter Benutzer");
	if (globals::users.size() == 0)
		return;
	if (Button("Anwenden")) {
		globals::users[slctdID] = slctdUser;
		logger::log("USER_WINDOW", "Saved User: " + slctdUser.firstName + " " + slctdUser.lastName);
	}
	SameLine();
	if (Button("Verwerfen")) {
		slctdUser = globals::users[slctdID];
	}
	
	Text("Nachname:");
	SameLine();
	char buff[b_size];
	strncpy_s(buff, slctdUser.lastName.c_str(), b_size);
	SetNextItemWidth(150);
	InputTextWithHint("##Last Name", "Nachname", buff, b_size);
	slctdUser.lastName = buff;
	
	SameLine();
	Text("Vorname:");
	SameLine();
	strncpy_s(buff, slctdUser.firstName.c_str(), b_size);
	SetNextItemWidth(150);
	InputTextWithHint("##First Name", "Vorname", buff, b_size);
	slctdUser.firstName = buff;
	
	char keyBuff[255];
	Text("Kennung :");
	SameLine();
	strncpy_s(keyBuff, slctdUser.key.c_str(), 255);
	SetNextItemWidth(400);
	InputTextWithHint("##Key", "chip kennung", keyBuff, 255);
	slctdUser.key = keyBuff;
	if (Button("Chip Scannen")) {
		int retValue = system("lesegeraet.exe");
		if (retValue == 0) {
			std::ifstream keyFile("tmpk");
			std::string key;
			keyFile >> key;
			keyFile.close();
			slctdUser.key = key;
			std::remove("tmpk");
		}
		else {
			logger::log("KEY_SCANNER", "Either no Scanner connected or no Chip to be read!");
		}
	}
	SetItemTooltip("Scanne einen Chip mit einem ACR122U NFC Reader.\nDer Reader erkennt den Chip, wenn die Anzeige LED Gruen leuchtet");
}

inline void HandleMainWindow() {
	ImVec2 WindowSize((float)GetRenderWidth(), (float)GetRenderHeight() - 19);
	SetNextWindowPos(ImVec2(0, 19));
	SetNextWindowSize(WindowSize);
	Begin("##MainWindow", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	BeginChild("Users", ImVec2(200, WindowSize.y));
	HandleUserSelect();
	EndChild();

	SameLine();

	BeginChild("Selected User");
	HandleSelectedUser();
	EndChild();
	End();
}

inline void HandleMainMenuBar() {
	BeginMainMenuBar(); // Starting the MainMenuBar
#ifdef VERSION
	Text(VERSION);
#endif
	if (MenuItem("Nutzer Speichern")) {
		if (!std::filesystem::exists("./users/"))
			std::filesystem::create_directories("./users/");
		SaveUsers(globals::users, "./users/users.bin", "");
		logger::log("SAVING", "Saved " + std::to_string(globals::users.size()) + " Users to file");
	}
	SetItemTooltip("Speichert alle Nutzer ab.");
	if (MenuItem("Nutzer Laden")) {
		if (std::filesystem::exists("./users/users.bin")) {
			globals::users.clear();
			LoadUsers(globals::users, "./users/users.bin", "");
			logger::log("LOADING", "loaded " + std::to_string(globals::users.size()) + " Users from file");
			if (globals::users.size() > 0) {
				slctdUser = globals::users[0];
			}
		}
	}
	SetItemTooltip("Laedt alle Nutzer.");
	// Manage Settings Menu
	if (MenuItem("Settings")) {
		settings_open = !settings_open;
	}
	if (settings_open) {
		HandleSettingsMenu();
	}
	// Manage Style Editor Window
	if (MenuItem("Style Editor")) {
		style_editor_open = !style_editor_open;
	}
	SetItemTooltip("Oeffnet den Style Editor zum bearbeiten eigener Praeferenzen");
	if (style_editor_open) {
		Begin("Style Editor", &style_editor_open);
		ShowStyleEditor();
		if (Button("Close"))
			style_editor_open = false;
		End();
	}
	EndMainMenuBar(); // Ending the MainMenuBar
}

inline void HandleSettingsMenu() {
	Begin("Settings", &settings_open,
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_AlwaysAutoResize); // Starting Settings Window

	if (Checkbox("Fullscreen", &globals::fullscreen)) {
		if (globals::fullscreen) {
			globals::borderless = false;
			globals::maximized = false;
		}
		globals::wasResized = true;
	}
	SameLine();
	if (Checkbox("Borderless", &globals::borderless)) {
		if (globals::borderless) {
			globals::fullscreen = false;
			globals::maximized = false;
		}
		globals::wasResized = true;
	}
	SameLine();
	if (Checkbox("Maximized", &globals::maximized)) {
		if (globals::maximized) {
			globals::fullscreen = false;
			globals::borderless = false;
		}
		globals::wasResized = true;
	}
	SeparatorText("Resolution");
	if (SliderInt("Width", &globals::width, MIN_WIDTH,
		GetMonitorWidth(GetCurrentMonitor()))) {
		globals::wasResized = true;
	}
	if (SliderInt("Height", &globals::height, MIN_HEIGHT,
		GetMonitorHeight(GetCurrentMonitor()))) {
		globals::wasResized = true;
	}
	SeparatorText("Misc Settings");
	if (SliderInt("FPS", &globals::fps, 30, 320)) {
		globals::wasResized = true;
	}
	if (Checkbox("VSYNC", &globals::vsync)) {
		globals::wasResized = true;
	}
	SeparatorText("");
	if (Button("Close"))
		settings_open = false;
	End(); // Ending Settings Window
}
