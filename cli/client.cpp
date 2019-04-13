#include <iostream>
#include <fstream>
#include <locale>
#include <iomanip>

#include <curl/curl.h>

#include "json.hpp"

#include <cstring>

using json = nlohmann::json;

#define PAPA_URL "http://localhost:3000/newuser"


struct resdata {
	char *mem;
	size_t size;
};

void init(CURL *curl);
bool validCode(std::string s);
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
bool loginUser(CURL *curl, struct resdata *rd);
bool registerUser(CURL *curl, struct resdata *rd);
std::string get(CURL *curl, struct resdata *rd);
bool post(CURL *curl, struct resdata *rd, json* j);

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

	struct resdata *rd;
	rd->mem = static_cast<char *>(malloc(1));
	rd->size = 1;

	std::string usr_input;
	std::cin >> usr_input;
	if (usr_input.length() < 2) {
		std::cout << "Not a valid request" << std::endl;
		init(curl);
		return;
	}
	if (usr_input.substr(0,2) == "DL") {
		if (!loginUser(curl, rd)) {
			std::cout << "User info not valid" << std::endl;
			init(curl);
			return;
		}
		auto j = json::parse(get(curl, rd));
		std::ofstream o(static_cast<std::string>(j["path"]));
		o << std::setw(4) << j << std::endl;
	} else if (usr_input.substr(0,2) == "UP") {
		if (usr_input.length() < 4) {
			std::cout << "User info not valid" << std::endl;
			init(curl);
			return;
		}
		int fileIndex = 2, endIndex;
		for (; usr_input[fileIndex] == ' '; fileIndex++);
		for (endIndex = fileIndex; usr_input[endIndex] != ' ' || usr_input[endIndex] != '\0'; endIndex++);
		std::ifstream i(usr_input.substr(fileIndex, endIndex-fileIndex));
		json j;
		i >> j;
		post(curl, rd, &j);
	} else if (usr_input.substr(0,4) == "HELP") {
		std::cout << "Commands\n--------------------------------------" << std::endl;
		std::cout << "DL - retrieve config file from server" << std::endl;
		std::cout << "Syntax: DL [FILENAME]" << std::endl;
		std::cout << "UP - upload config file to server" << std::endl;
		std::cout << "Syntax: UP [FILENAME]" << std::endl;
	} else {
		std::cout << "Not a valid request" << std::endl;
		init(curl);
	}

}

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {

	struct resdata *rd = static_cast<struct resdata *>(userdata);
	rd->mem = static_cast<char *>(realloc(rd->mem, nmemb));
	memcpy(rd->mem, ptr, nmemb);
	rd->size = nmemb;
	
	return nmemb;

}

bool loginUser(CURL *curl, struct resdata *rd) {
	std::string username, password;

	std::cout << "Username: ";
	std::cin >> username;
	std::cout << "Password: ";
	std::cin >> password;

	json *j = new json;
	(*j)["username"] = username;
	(*j)["password"] = password;

	return post(curl, rd, j);
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

	return post(curl, rd, j);
}

std::string get(CURL *curl, struct resdata *rd) {
	curl_easy_setopt(curl, CURLOPT_URL, PAPA_URL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(rd));

	CURLcode res = curl_easy_perform(curl);

	if (res != CURLE_OK) {
		std::cout << curl_easy_strerror(res) << std::endl;
		return rd->mem;
	}
	return rd->mem;
}

bool post(CURL *curl, struct resdata *rd, json* j) {
	struct curl_slist *list = nullptr;
	list = curl_slist_append(list, "Content-Type: application/json; charset=utf-8");

	curl_easy_setopt(curl, CURLOPT_URL, PAPA_URL);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(rd));

	// keep the string alive
	char *s = static_cast<char *>(malloc(j->dump().length() + 1));
	memcpy(s, j->dump().c_str(), j->dump().length());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, s);
	//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonString.c_str());
	//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"username\":\"yeet\"}");

	CURLcode res = curl_easy_perform(curl);

	free(s);
	free(list);

	if (res != CURLE_OK) {
		std::cout << curl_easy_strerror(res) << std::endl;
		return false;
	}
	return true;
}