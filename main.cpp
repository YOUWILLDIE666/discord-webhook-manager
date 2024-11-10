#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <cURLpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <cURLpp/Options.hpp>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <boost/filesystem.hpp>

const std::string RESET = "\x1B[0m";
const std::string BOLD = "\x1B[1m";
const std::string UNDERLINE = "\x1B[4m";
const std::string CYAN = "\x1B[36m";
const std::string YELLOW = "\x1B[33m";
const std::string GREEN = "\x1B[32m";
const std::string RED = "\x1B[31m";

int toDecimal(const std::string& hexColor) {
    return std::stoi(hexColor.substr(1), nullptr, 16);
}

bool fileGE(const std::string& filePath) {
    boost::filesystem::path path(filePath);

    if (!boost::filesystem::exists(path)) {
        std::cerr << RED << "FATAL: file does not exist." << RESET << std::endl;
        return false;
    }

    if (!boost::filesystem::is_regular_file(path)) {
        std::cerr << RED << "FATAL: path is not a regular file." << RESET << std::endl;
        return false;
    }

    std::uintmax_t fileSize = boost::filesystem::file_size(path);

    constexpr std::uintmax_t eightMB = 8 * 1024 * 1024; // 8 MB
    return fileSize >= eightMB;
}

static void hell()
{
    std::cout << "Usage:\n"
              << "  discord-webhook.exe [options]\n\n"
              << "Options:\n"
              << "  --delete <url>                Delete the specified webhook.\n"
              << "  --dump <url>                  Dump information about the specified webhook.\n"
              << "  --send <url>                  Send a message (POST) to the specified webhook with the specified parameters.\n"
              << "  --json <file>                 Load options from the specified JSON file.\n"
              << "  --username <name>             Specify the username.\n"
              << "  --content <text>              Specify the message content.\n"
              << "  --avatar-url <url>            Specify the avatar URL.\n"
              << "  --file <path>                 Specify the path to the file to attach.\n\n"
              << "Embed Options:\n"
              << "  --title <title>               Specify the embed title.\n"
              << "  --description <text>          Specify the embed description.\n"
              << "  --color <color>               Specify the embed color.\n"
              << "  --footer <footer>             Specify the embed footer text.\n"
              << "  --footer-icon <url>           Specify the embed footer icon URL.\n\n"
              << "  --help                        Display this help message.\n";
}

static void loadJSON(const std::string& filePath, std::string& webhookUrl, std::string& username,
                     std::string& content, std::string& avatarUrl, std::string& embedTitle,
                     std::string& embedDescription, std::string& embedColor, std::string& embedFooter,
                     std::string& embedFooterIcon, std::string& file,
                     bool& dumpWebhookFlag, bool& deleteWebhookFlag,
                     bool& sendWebhookFlag)
{
    std::ifstream jsonFile(filePath);
    if (!jsonFile.is_open())
    {
        std::cerr << RED << "Error: Could not open JSON file: " << filePath << RESET << std::endl;
        return;
    }

    nlohmann::json jsonData;
    jsonFile >> jsonData;

    if (jsonData.contains("webhook-url")) webhookUrl = jsonData["webhook-url"].get<std::string>();
    if (jsonData.contains("username")) username = jsonData["username"].get<std::string>();
    if (jsonData.contains("content")) content = jsonData["content"].get<std::string>();
    if (jsonData.contains("avatar-url")) avatarUrl = jsonData["avatar-url"].get<std::string>();
    if (jsonData.contains("embed-title")) embedTitle = jsonData["embed-title"].get<std::string>();
    if (jsonData.contains("embed-description")) embedDescription = jsonData["embed-description"].get<std::string>();
    if (jsonData.contains("embed-color"))
    {
        std::string hexColor = jsonData["embed-color"].get<std::string>();
        if (hexColor[0] == '#')
        {
            embedColor = std::to_string(toDecimal(hexColor));
        }
        else
        {
            embedColor = hexColor;
        }
    }
    if (jsonData.contains("embed-footer")) embedFooter = jsonData["embed-footer"].get<std::string>();
    if (jsonData.contains("embed-footer-icon")) embedFooterIcon = jsonData["embed-footer-icon"].get<std::string>();
    if (jsonData.contains("file")) file = jsonData["file"].get<std::string>();
    if (jsonData.contains("dump")) dumpWebhookFlag = jsonData["dump"].get<bool>();
    if (jsonData.contains("delete")) deleteWebhookFlag = jsonData["delete"].get<bool>();
    if (jsonData.contains("send")) sendWebhookFlag = jsonData["send"].get<bool>();
}

static void dumpWebhook(const std::string& webhookUrl)
{
    try
    {
        cURLpp::Easy request;
        request.setOpt(new cURLpp::options::Url(webhookUrl));
        request.setOpt(new cURLpp::options::CustomRequest("GET"));

        std::ostringstream response;
        request.setOpt(new cURLpp::options::WriteStream(&response));

        request.perform();

        nlohmann::json jsonResponse = nlohmann::json::parse(response.str(), nullptr, false);
        if (jsonResponse.is_discarded())
        {
            std::cerr << RED << "Error: Failed to parse JSON response." << RESET << std::endl;
            return;
        }

        if (!jsonResponse["id"].is_null())
        {
            std::cout << BOLD << CYAN << "Webhook Information:" << RESET << "\n"
                      << BOLD << UNDERLINE << "==============================================" << RESET << "\n"
                      << BOLD << YELLOW << "Token:" << RESET << " " << jsonResponse["token"].get<std::string>() << "\n"
                      << BOLD << YELLOW << "Name:" << RESET << " " << jsonResponse["name"].get<std::string>() << "\n"
                      << BOLD << YELLOW << "Guild ID:" << RESET << " " << jsonResponse["guild_id"].get<std::string>() << "\n"
                      << BOLD << YELLOW << "Channel ID:" << RESET << " " << jsonResponse["channel_id"].get<std::string>() << "\n"
                      << BOLD << YELLOW << "Application ID:" << RESET << " "
                      << (jsonResponse["application_id"].is_null() ? RED + "None" + RESET : jsonResponse["application_id"].get<std::string>()) << "\n"
                      << BOLD << YELLOW << "ID:" << RESET << " " << jsonResponse["id"].get<std::string>() << "\n"
                      << BOLD << YELLOW << "Avatar:" << RESET << " "
                      << (jsonResponse["avatar"].is_null() ? RED + "None" + RESET : jsonResponse["avatar"].get<std::string>()) << "\n"
                      << BOLD << YELLOW << "Webhook URL:" << RESET << " " << jsonResponse["url"].get<std::string>() << "\n"
                      << BOLD << UNDERLINE << "==============================================" << RESET << "\n";
        }
        else
        {
            std::cout << RED << "Error: webhook doesn't exist (Probably deleted)." << RESET << std::endl;
        }
    }
    catch (const cURLpp::RuntimeError& e)
    {
        std::cerr << RED << e.what() << RESET << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << RED << e.what() << RESET << std::endl;
    }
}

const static void sendWebhook(const std::string& webhookUrl, const std::string& username, const std::string& content,
                              const std::string& avatarUrl, const std::string& embedTitle, const std::string& embedDescription,
                              const std::string& embedColor, const std::string& embedFooter, const std::string& embedFooterIcon,
                              const std::string& filePath)
{
    if (fileGE(filePath))
    {
        std::cerr << RED << "ERROR: file is >=8MB." << RESET << std::endl;
    }
    nlohmann::json jsonPayload;
    jsonPayload["username"] = username;
    jsonPayload["content"] = content;
    if (!avatarUrl.empty())
    {
        jsonPayload["avatar_url"] = avatarUrl;
    }

    if (!(embedTitle.empty() || embedDescription.empty() || embedColor.empty() || embedFooter.empty() || embedFooterIcon.empty()))
    {
        nlohmann::json embed;
        if (!embedTitle.empty()) embed["title"] = embedTitle;
        if (!embedDescription.empty()) embed["description"] = embedDescription;
        if (!embedColor.empty()) embed["color"] = embedColor;
        if (!embedFooter.empty()) embed["footer"]["text"] = embedFooter;
        if (!embedFooterIcon.empty()) embed["footer"]["icon_url"] = embedFooterIcon;

        jsonPayload["embeds"] = { embed };
    }

    // use cURL since I almost killed myself while trying to implement the same thing with cURLpp (using formport)

    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, webhookUrl.c_str());

        curl_mime *form = curl_mime_init(curl);

        curl_mimepart *part = curl_mime_addpart(form);
        curl_mime_name(part, "payload_json");
        std::string jsonString = jsonPayload.dump();
        curl_mime_data(part, jsonString.c_str(), CURL_ZERO_TERMINATED);

        if (!filePath.empty())
        {
            part = curl_mime_addpart(form);
            curl_mime_name(part, "file");
            curl_mime_filedata(part, filePath.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            std::cout << GREEN << "Success!" << RESET << std::endl;
        }

        curl_mime_free(form);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

const static void deleteWebhook(const std::string& webhookUrl)
{
    try
    {
        cURLpp::Easy request;
        request.setOpt(new cURLpp::options::Url(webhookUrl));
        request.setOpt(new cURLpp::options::CustomRequest("DELETE"));
        request.setOpt(new cURLpp::options::Verbose(false));

        request.perform();
        std::cout << "Webhook " << UNDERLINE << RED << "DEMOLISHED" << RESET << " successfully >:3" << std::endl;
    }
    catch (const cURLpp::RuntimeError& e)
    {
        std::cerr << RED << e.what() << RESET << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << RED << e.what() << RESET << std::endl;
    }
}

int main(int argc, char* argv[])
{
#ifdef _WIN32
#include <Windows.h>
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hConsole, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hConsole, dwMode);
#endif // defined(_WIN32)
    cURLpp::Cleanup();

    std::string webhookUrl;
    std::string username = "Webhook";
    std::string content;
    std::string avatarUrl;
    std::string filePath;
    std::string embedTitle;
    std::string embedDescription;
    std::string embedColor;
    std::string embedFooter;
    std::string embedFooterIcon;

    bool dumpWebhookFlag = false;
    bool deleteWebhookFlag = false;
    bool sendWebhookFlag = false;

    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]) == "--help")
        {
            hell();
            return 0;
        }
        else if (std::string(argv[i]) == "--json")
        {
            loadJSON(argv[++i], webhookUrl, username, content, avatarUrl, embedTitle, embedDescription,
            embedColor, embedFooter, embedFooterIcon, filePath, dumpWebhookFlag, deleteWebhookFlag, sendWebhookFlag);
        }
        else if (std::string(argv[i]) == "--dump")
        {
            dumpWebhookFlag = true;
            webhookUrl = argv[++i];
        }
        else if (std::string(argv[i]) == "--delete")
        {
            deleteWebhookFlag = true;
            webhookUrl = argv[++i];
        }
        else if (std::string(argv[i]) == "--send")
        {
            if (i + 1 < argc)
            {
                sendWebhookFlag = true;
                webhookUrl = argv[++i];
            }
            else
            {
                std::cerr << RED << "FATAL: --send requires an argument." << RESET << std::endl;
                return 1;
            }
        }
        else if (std::string(argv[i]) == "--username")
        {
            if (i + 1 < argc)
            {
                username = argv[++i];
            }
            else
            {
                std::cerr << RED << "FATAL: --username requires an argument." << RESET << std::endl;
                return 1;
            }
        }
        else if (std::string(argv[i]) == "--content")
        {
            if (i + 1 < argc)
            {
                content = argv[++i];
            }
            else
            {
                std::cerr << RED << "FATAL: --content requires an argument." << RESET << std::endl;
                return 1;
            }
        }
        else if (std::string(argv[i]) == "--avatar-url")
        {
            if (i + 1 < argc)
            {
                avatarUrl = argv[++i];
            }
            else
            {
                std::cerr << RED << "FATAL: --avatar-url requires an argument." << RESET << std::endl;
                return 1;
            }
        }
        else if (std::string(argv[i]) == "--file")
        {
            if (i + 1 < argc)
            {
                filePath = argv[++i];
            }
            else
            {
                std::cerr << RED << "FATAL: --file requires an argument." << RESET << std::endl;
                return 1;
            }
        }
        else if (std::string(argv[i]) == "--title")
        {
            if (i + 1 < argc)
            {
                embedTitle = argv[++i];
            }
            else
            {
                std::cerr << RED << "FATAL: --title requires an argument." << RESET << std::endl;
                return 1;
            }
        }
        else if (std::string(argv[i]) == "--description")
        {
            if (i + 1 < argc)
            {
                embedDescription = argv[++i];
            }
            else
            {
                std::cerr << RED << "FATAL: --description requires an argument." << RESET << std::endl;
                return 1;
            }
        }
        else if (std::string(argv[i]) == "--color")
        {
            if (i + 1 < argc)
            {
                embedColor = argv[++i];
            }
            else
            {
                std::cerr << RED << "FATAL: --color requires an argument." << RESET << std::endl;
                return 1;
            }
        }
        else if (std::string(argv[i]) == "--footer")
        {
            if (i + 1 < argc)
            {
                embedFooter = argv[++i];
            }
            else
            {
                std::cerr << RED << "FATAL: --footer requires an argument." << RESET << std::endl;
                return 1;
            }
        }
        else if (std::string(argv[i]) == "--footer-icon")
        {
            if (i + 1 < argc)
            {
                embedFooterIcon = argv[++i];
            }
            else
            {
                std::cerr << RED << "FATAL: --footer-icon requires an argument." << RESET << std::endl;
                return 1;
            }
        }
        else
        {
            std::cerr << RED << "FATAL: unknown argument '" << argv[i] << "'" << RESET << std::endl;
            return 1;
        }
    }

    if (deleteWebhookFlag)
    {
        deleteWebhook(webhookUrl);
    }
    else if (dumpWebhookFlag)
    {
        dumpWebhook(webhookUrl);
    }
    else if (sendWebhookFlag)
    {
        sendWebhook(webhookUrl, username, content, avatarUrl, embedTitle, embedDescription, embedColor, embedFooter, embedFooterIcon, filePath);
    }
    else
    {
        std::cerr << RED << "No valid operation specified. Use --help for usage information." << RESET << std::endl;
    }

    return 0;
}