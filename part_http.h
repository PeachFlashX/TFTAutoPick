#pragma once
#include "curl/curl.h"
#include "json/json.h"
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <windows.h>

using namespace std;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
string base64_encode(const string& in);
bool str2json(const std::string& str, Json::Value& v);
string json2str(const Json::Value& v, bool needFormat);
int str2int(string str);
string int2str(int k);

string getPortAToken(string& port, string& token);
void autoAccept(CURL* curl);
string getTry(CURL* curl, string url);
string postTry(CURL* curl, string url, string data = "1");
int getState(CURL* curl);
void basic_get(CURL* curl, string& gameName, int& gameLeave);

void http_main(bool& open_mode_program, bool& open_mode_accept, string& gameName, int& gameLeave);