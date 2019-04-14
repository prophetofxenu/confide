#include <iostream>
#include <fstream>
#include <locale>
#include <iomanip>

#include <curl/curl.h>

#include "json.hpp"

#include <cstring>

using json = nlohmann::json;

//#define PAPA_URL "http://localhost:3000"

std::string PAPA_URL("http://localhost:3000");

typedef struct resdata {
	char *mem;
	size_t size;
} ResData;

void init(CURL *curl);
void displayCommands();
bool validCode(std::string s);
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
bool loginUser(CURL *curl, struct resdata *rd, std::string username, std::string password);
bool registerUser(CURL *curl, struct resdata *rd);
//std::string get(CURL *curl, struct resdata *rd);
bool get(CURL *curl, std::string dest, struct resdata *rd, json *j);
bool post(CURL *curl, std::string dest, struct resdata *rd, json* j);
std::string toUpper(std::string s);

int main() {

	/*
	std::ifstream i("test.json");
	json j;
	i >> j;

	std::string s = j.dump();

	for (int i = 0; i < s.length(); i++) {
		std::cout << s[i];
	}
	std::cout << std::endl;
	*/

	std::cout << "--== ConfIDE ==--" << std::endl;

	CURL *curl = curl_easy_init();
	if (curl) {
		init(curl);
		curl_easy_cleanup(curl);
	} else {
		std::cout << "Connection to server cannot be established." << std::endl;
	}

	return 0;

}

void init(CURL *curl) {

	CURLcode res;

	ResData *rd = new ResData;
	// rd->mem = static_cast<char *>(malloc(1));
	rd->mem = nullptr;
	rd->size = 1;

	std::string username, password, name, path;

	std::cout << "Login/Register" << std::endl;
	std::cout << "LOG        REG" << std::endl;
	while (1) {
		std::string usr_input;
		std::cin >> usr_input;
		usr_input = toUpper(usr_input);
		if (usr_input == "LOG") {
			std::cout << "Username: ";
			std::cin >> username;
			std::cout << "Password: ";
			std::cin >> password;
			if (loginUser(curl, rd, username, password)) {
				json response = json::parse(rd->mem);
				if (response["result"] == -1) {
					std::cout << "User info not valid" << std::endl;
					continue;
				} else {
					std::cout << "User info valid" << std::endl;
				}
			} else { continue; }
			break;
		} else if (usr_input == "REG") {
			if (registerUser(curl, rd)) {
				json response = json::parse(rd->mem);
				if (response["result"] == -1) {
					std::cout << "User info not valid" << std::endl;
					continue;
				} else {
					std::cout << "User info valid" << std::endl;
				}
			} else { continue; }
			break;
		} else {
			std::cout << "Invalid input. Try again" << std::endl;
		}
	}

	displayCommands();

	while (1) {
		std::string usr_input;
		std::cin >> usr_input;
		usr_input = toUpper(usr_input);
		if (usr_input.length() < 2) {
			std::cout << "Not a valid request" << std::endl;
			continue;
		}

		if (usr_input.substr(0,2) == "LS") {
			std::cout << "Filename: ";
			std::cin >> name;
			json *j = new json;
			(*j)["username"] = username;
			(*j)["password"] = password;
			(*j)["name"] = name;
			get(curl, std::string("listconfs"), rd, j);
			json response = json::parse(rd->mem);
			if (response["result"] == -1) {
				std::cout << "File transfer incomplete" << std::endl;
			} else {
				std::cout << "File transfer complete" << std::endl;
				std::cout << response["configs"] << std::endl;
			}
		} else if (usr_input.substr(0,2) == "VC") {
			std::cout << "Filename: ";
			std::cin >> name;
			json *j = new json;
			(*j)["username"] = username;
			(*j)["password"] = password;
			(*j)["name"] = name;
			get(curl, std::string("config"), rd, j);
			json response = json::parse(rd->mem);
			if (response["result"] == -1) {
				std::cout << "File transfer incomplete" << std::endl;
			} else {
				std::cout << "File transfer complete" << std::endl;
				std::cout << "Created: " << response["created"] << std::endl;
				std::cout << "Last Modified: " << response["modified"] << std::endl;
				std::cout << response["content"] << std::endl;
			}
		} else if (usr_input.substr(0,2) == "DL") {
			std::cout << "Filename: ";
			std::cin >> name;
			json *j = new json;
			(*j)["username"] = username;
			(*j)["password"] = password;
			(*j)["name"] = name;
			get(curl, std::string("config"), rd, j);
			json response = json::parse(rd->mem);
			if (response["result"] == -1) {
				std::cout << "File transfer incomplete" << std::endl;
			} else {
				std::cout << "File transfer complete" << std::endl;
				std::ofstream o(static_cast<std::string>(response["path"]));
				o << std::setw(4) << response["content"] << std::endl;
			}
		} else if (usr_input.substr(0,2) == "UP") {
			std::cout << "Filename: ";
			std::cin >> name;
			std::cout << "Path: ";
			std::cin >> path;
			std::ifstream i(path);
			json *j = new json;
			json f;
			i >> f;
			(*j)["username"] = username;
			(*j)["password"] = password;
			(*j)["name"] = name;
			(*j)["path"] = path;
			(*j)["content"] = f;
			post(curl, std::string("addconfig"), rd, j);
			json response = json::parse(rd->mem);
			if (response["result"] == -1) {
				std::cout << "File transfer incomplete" << std::endl;
			} else {
				std::cout << "File transfer complete" << std::endl;
			}
		} else if (usr_input.substr(0,4) == "HELP") {
			displayCommands();
		} else if (usr_input.substr(0,4) == "EXIT") {
			break;
		} else {
			std::cout << "Not a valid request" << std::endl;
		}
	}

}

void displayCommands() {
	std::cout << "\nCommands\n--------------------------------------" << std::endl;
	std::cout << "LS - list available files" << std::endl;
	std::cout << "VC - view contents of a file" << std::endl;
	std::cout << "DL - retrieve config file from server" << std::endl;
	std::cout << "UP - upload config file to server" << std::endl;
	std::cout << "EXIT - exits program" << std::endl;
	std::cout << std::endl;
}

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {

	struct resdata *rd = static_cast<struct resdata *>(userdata);
	rd->mem = static_cast<char *>(malloc(nmemb));
	memcpy(rd->mem, ptr, nmemb);
	rd->mem[nmemb] = '\0';
	rd->size = nmemb;
	
	return nmemb;

}

bool loginUser(CURL *curl, struct resdata *rd, std::string username, std::string password) {

	json *j = new json;
	(*j)["username"] = username;
	(*j)["password"] = password;

	return get(curl, std::string("validuser"), rd, j);
}

bool registerUser(CURL *curl, struct resdata *rd) {
	std::string username, password, email;

	std::cout << "Username: ";
	std::cin >> username;
	std::cout << "Password: ";
	std::cin >> password;
	std::cout << "Email: ";
	std::cin >> email;

	json *j = new json;
	(*j)["username"] = username;
	(*j)["password"] = password;
	(*j)["email"] = email;

	return post(curl, std::string("newuser"), rd, j);
}

bool get(CURL *curl, std::string dest, struct resdata *rd, json *j) {

	std::string full_dest = PAPA_URL + "/" + dest + "/" + j->dump();

	curl_easy_setopt(curl, CURLOPT_URL, full_dest.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(rd));

	CURLcode res = curl_easy_perform(curl);

	if (res != CURLE_OK) {
		std::cout << curl_easy_strerror(res) << std::endl;
		return false;
	}
	return true;

}

bool post(CURL *curl, std::string dest, struct resdata *rd, json* j) {
	struct curl_slist *list = nullptr;
	list = curl_slist_append(list, "Content-Type: application/json; charset=utf-8");

	std::string full_dest = PAPA_URL + "/" + dest;

	curl_easy_setopt(curl, CURLOPT_URL, full_dest.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(rd));

	// keep the string alive
	char *s = static_cast<char *>(malloc(j->dump().length() + 1));
	memcpy(s, j->dump().c_str(), j->dump().length());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, s);

	CURLcode res = curl_easy_perform(curl);

	free(s);
	free(list);

	if (res != CURLE_OK) {
		std::cout << curl_easy_strerror(res) << std::endl;
		return false;
	}
	return true;
}

std::string toUpper(std::string s) {
	for (int i = 0; i < s.length(); i++) {
		if (s[i] > 'Z') {
			s[i] -= 32;
		}
	}
	return s;
}