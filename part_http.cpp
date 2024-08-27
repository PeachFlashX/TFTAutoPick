#include "part_http.h"

string url_basic = "https://127.0.0.1:";
string url_port = "";
string my_token = "";
string puuid_me = "";
int nowState = -1;

const string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

string base64_encode(const string& in)
{
    string out;

    int val = 0;
    int valb = -6;
    for (unsigned char c : in)
    {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0)
        {
            out.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6)
    {
        out.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    while (out.size() % 4)
    {
        out.push_back('=');
    }

    return out;
}

bool str2json(const std::string& str, Json::Value& v)
{
    Json::CharReaderBuilder readerBuilder;
    std::istringstream iss(str);

    std::string errs;
    bool parsingSuccessful = Json::parseFromStream(readerBuilder, iss, &v, &errs);
    if (!parsingSuccessful)
    {
        std::cout << "Failed to parse JSON: " << errs << std::endl;
        return false;
    }

    return true;
}
std::string json2str(const Json::Value& v, bool needFormat)
{
    if (needFormat)
    {
        Json::StreamWriterBuilder writer;
        std::string jsonString = Json::writeString(writer, v);
        return jsonString;
    }
    else
    {
        Json::FastWriter fastWriter;
        return fastWriter.write(v);
    }
}
int str2int(string str)
{
    int ans = 0;
    for (int a = 0; a < str.size(); a++)
    {
        ans *= 10;
        ans += str[a] - '0';
    }
    return ans;
}
string int2str(int k)
{
    string ans = "";
    while (k > 0)
    {
        ans += k % 10 + '0';
        k /= 10;
    }
    if (ans.empty())
        ans = "0";
    reverse(ans.begin(), ans.end());
    return ans;
}

string getPortAToken(string& port, string& token)
{
    port.clear();
    token.clear();
    FILE* fp;
    fp = _popen("wmic PROCESS WHERE name='LeagueClientUx.exe' GET commandline", "r");
    char ch;
    string str = "";
    while ((ch = fgetc(fp)) != EOF)
    {
        str += ch;
    }
    fclose(fp);
    int k = str.find("--remoting-auth-token", 0);
    if (k == std::string::npos)
    {
        str.clear();
        return str;
    }
    k += 22;
    for (int a = k; a < str.size(); a++)
    {
        if (str[a] != '"')
            token += str[a];
        else
            break;
    }
    k = str.find("--app-port", k) + 11;
    for (int a = k; a < str.size(); a++)
    {
        if (str[a] != '"')
            port += str[a];
        else
            break;
    }
    return str;
}
void autoAccept(CURL* curl)
{
    //"/lol-matchmaking/v1/ready-check/decline"拒绝
    postTry(curl, "/lol-matchmaking/v1/ready-check/accept");
}

string getTry(CURL* curl, string url)
{
    string readBuffer;
    CURLcode res;

    //  设置目标URL
    curl_easy_setopt(curl, CURLOPT_URL, (url_basic + url_port + url).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

    // 设置写入回调函数
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

    // 设置写入数据的位置
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    // 设置为跟随重定向
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // 执行HTTP请求
    readBuffer.clear();
    res = curl_easy_perform(curl);

    return readBuffer;
}
string postTry(CURL* curl, string url, string data)
{
    string readBuffer;
    CURLcode res;

    //  设置目标URL
    curl_easy_setopt(curl, CURLOPT_URL, (url_basic + url_port + url).c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    // 设置写入回调函数
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

    // 设置写入数据的位置
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    // 设置为跟随重定向
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // post数据部分(待修改)
    // Json::Value data;
    // Json::Value invitation;
    // invitation["summonerNames"] = "终成她的影子";
    // data.append(invitation);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

    // 执行HTTP请求
    res = curl_easy_perform(curl);
    return readBuffer;
}
int getState(CURL* curl)
{
    string readBuffer;
    readBuffer = getTry(curl, "/lol-gameflow/v1/gameflow-phase");
    if (nowState == 5)
        Sleep(400);
    else
        Sleep(1000);
    if (readBuffer == "\"Lobby\"")
        nowState = 0;
    else if (readBuffer == "\"InProgress\"")
        nowState = 1;
    else if (readBuffer == "\"Reconnect\"")
        nowState = 2;
    else if (readBuffer == "\"ReadyCheck\"")
    {
        nowState = 3;
        autoAccept(curl);
    }
    else if (readBuffer == "\"ChampSelect\"")
        nowState = 4;
    else if (readBuffer == "\"Matchmaking\"")
        nowState = 5;
    else
        nowState = -1;
    return 1;
}
void basic_get(CURL* curl, string& gameName, int& gameLeave)
{
    Json::Value data;
    str2json(getTry(curl, "/lol-summoner/v1/current-summoner"), data);
    gameName = data["gameName"].asCString();
    gameLeave = data["summonerLevel"].asInt();
    puuid_me = data["puuid"].asCString();
}

void http_main(bool& open_mode_program, bool& open_mode_accept, string& gameName, int& gameLeave)
{
    if (getPortAToken(url_port, my_token).empty())
    {
        cout << "not found LOL" << endl;
        return;
    }
    string auth_ = "riot:" + my_token;
    cout << auth_ << endl;

    curl_slist* headers = NULL;
    headers = curl_slist_append(headers, ("Authorization:Basic " + base64_encode(auth_)).c_str());
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charset=UTF-8");

    CURL* curl = curl_easy_init();
    if (!curl)
    {
        cerr << "Curl initialization failed" << endl;
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // 禁用证书验证（不推荐用于生产环境）
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // 不验证证书中的主机名

    basic_get(curl, gameName, gameLeave);

    while (open_mode_program)
    {
        if (open_mode_accept)
            getState(curl);
        else
            Sleep(1000);
    }

    return;
}