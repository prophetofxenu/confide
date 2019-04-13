#include <iostream>
#include <fstream>
#include <locale>

#include <curl/curl.h>

#include "json.hpp"

#include <cstring>

using json = nlohmann::json;

#define PAPA_URL "http://localhost:3000/newuser"


void init(CURL *curl);
bool validCode(std::string s);
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
bool verifyUser(CURL *curl);
void get(CURL *curl);
void post(CURL *curl);

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
		CURLcode res;
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		res = curl_easy_perform(curl);
		std::cout << res << std::endl;
		curl_easy_cleanup(curl);
	} else {
		std::cout << "Connection to server cannot be established." << std::endl;
	}

	return 0;

}

void init(CURL *curl) {

	std::string usr_input;
	std::cin >> usr_input;

	if (usr_input.substr(0,2) == "DL") {
		if (!verifyUser(curl)) {
			std::cout << "User info not valid" << std::endl;
		}
		get(curl);
	} else if (usr_input.substr(0,2) == "UP") {
		int fileIndex = 2;
		for (; usr_input[fileIndex] == ' '; fileIndex++);
		// post(curl, j);
	} else if (usr_input.substr(0,4) == "HELP") {
		std::cout << "Commands\n--------------------------------------" << std::endl;
		std::cout << "DL - retrieve config file from server" << std::endl;
		std::cout << "Syntax: DL [FILENAME]" << std::endl;
		std::cout << "UP - upload config file to server" << std::endl;
		std::cout << "Syntax: UP [FILENAME]" << std::endl;
	} else if (usr_input.substr(0,3) == "PROP") {

	} else {
		std::cout << "Not a valid request" << std::endl;
		init(curl);
	}

}

bool validCode(std::string s) {
	if (s == "GET" || s == "POST" || s == "HELP") { return true; }
}

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {


	
	return nmemb;

}

bool verifyUser(CURL *curl) {
	std::string username, password, email, jsonString;

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

	post(curl, j);

	return true;
	
}

void get(CURL *curl) {

}

void post(CURL *curl, json* j) {
	struct curl_slist *list = nullptr;
	list = curl_slist_append(list, "Content-Type: application/json; charset=utf-8");

	curl_easy_setopt(curl, CURLOPT_URL, PAPA_URL);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

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
	}
}