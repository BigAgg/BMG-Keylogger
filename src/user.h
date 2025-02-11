#pragma once

#include <string>
#include <vector>
#include <fstream>

class User;

class User {
public:
	User(std::string lastName, std::string firstName);

	std::string lastName, firstName;
	std::string key; // Read NFC after encoding using MD5
};

// Saving Users inside a given vector to a file
extern bool SaveUsers(std::vector<User>& users, std::string path, std::string filename);
// Loading users into a given vector from given filepath
extern bool LoadUsers(std::vector<User>& users, std::string path, std::string filename);

