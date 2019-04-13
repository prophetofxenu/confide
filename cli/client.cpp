#include <iostream>
#include <fstream>

#include <curl/curl.h>

#include "json.hpp"

using json = nlohmann::json;

#define PAPA_URL "http://google.com"


void init();
bool validCode(std::string s);

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
		init();
		CURLcode res;
		curl_easy_setopt(curl, CURLOPT_URL, PAPA_URL);
		res = curl_easy_perform(curl);
		std::cout << res << std::endl;
		curl_easy_cleanup(curl);
	} else {
		std::cout << "Connection to server cannot be established." << std::endl;
	}

	return 0;

}

void init() {

	std::string usr_input;

	std::cout << "HELP - list of available commands" << std::endl;
	std::cin >> usr_input;
	while (!validCode(usr_input)) {
		std::cout << "Not a valid request" << std::endl;
		std::cin >> usr_input;
	}

	

	if (usr_input == "GET") {
	
	} else if (usr_input == "POST") {

	} else if (usr_input == "HELP") {

	}

}

bool validCode(std::string s) {
	if (s == "GET" || s == "POST" || s == "HELP") { return true; }
}
