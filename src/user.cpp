#include "user.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

static constexpr char XOR_KEY = 0x5A;

User::User(std::string lastName, std::string firstName) {
	this->firstName = firstName;
	this->lastName = lastName;
	this->key = "";
}

// Use XOR_KEY to encrypt a string to make it not human readable
static std::string obfuscate(const std::string& str) {
	std::string result = str;
	for (char& c : result) {
		c ^= XOR_KEY;
	}
	return result;
}

// Get a readable string out of a encrypted string using the XOR_KEY
static std::string deobfuscate(const std::string& str) {
	return obfuscate(str);
}

// Saves a string properly to a file storing its length and contents
bool SaveString(std::ofstream& file, const std::string& str) {
	size_t len = str.size();
	file.write(reinterpret_cast<const char*>(&len), sizeof(len));
	file.write(str.data(), len);
	return true;
}

// Loading a string properly from a file with its proper length and contents
std::string LoadString(std::ifstream& file) {
	size_t len;
	file.read(reinterpret_cast<char*>(&len), sizeof(len));
	std::string str(len, '\0');
	file.read(&str[0], len);
	return str;
}

bool SaveUsers(std::vector<User>& users, std::string path, std::string filename) {
	// Checking if there are users to save
	if (users.size() == 0)
		return false;
	// Building the path
	if (filename != "")
		path += "/" + filename;
	std::ofstream file(path, std::ios::binary);
	// Checking if file couldn't be opened or got corrupted
	if (!file.is_open() || !file.good())
		return false;
	// Saving all users
	size_t size = users.size();
	file.write(reinterpret_cast<const char*>(&size), sizeof(size));
	for (auto u : users) {
		SaveString(file, obfuscate(u.lastName));
		SaveString(file, obfuscate(u.firstName));
		SaveString(file, obfuscate(u.key));
	}
	// Closing file
	file.close();
	return true;
}

bool LoadUsers(std::vector<User>& users, std::string path, std::string filename) {
	// Building the path
	if(filename != "")
		path += "/" + filename;
	
	std::ifstream file(path, std::ios::binary);
	// Checking if file couldn't be opened or got corrupted
	if (!file.is_open() || !file.good())
		return false;
	// Loading all users
	size_t size = 0;
	file.read(reinterpret_cast<char*>(&size), sizeof(size));
	if (size == 0)
		return false;
	for (int x = 0; x < size; x++) {
		users.emplace_back("", "");
		users.back().lastName = deobfuscate(LoadString(file));
		users.back().firstName = deobfuscate(LoadString(file));
		users.back().key = deobfuscate(LoadString(file));
	}
	// Closing the file
	file.close();
	return true;
}
