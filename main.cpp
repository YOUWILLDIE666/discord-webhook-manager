#include <iostream>
#include <fstream>
#include <string>
#include <cURLpp/cURLpp.hpp>
#include <cURLpp/Easy.hpp>
#include <cURLpp/Options.hpp>
#include <nlohmann/json.hpp>

void hell()
{
    std::cout << "Usage:\n"
              << "  webhook_sender [options]\n\n"
              << "Options:\n"
              << "  --delete <webhookUrl>    Delete the specified webhook.\n"
              << "  --json <file>            Load webhook parameters from a JSON file.\n"
              << "  --username <name>        Specify the username.\n"
              << "  --content <message>      Specify the message content.\n"
              << "  --avatarUrl <url>        Specify the avatar URL.\n"
              << "  --help                   Display this help message.\n";
}

void sendWebhook(const std::string& webhookUrl, const std::string& username, const std::string& content, const std::string& avatarUrl)
{
    nlohmann::json jsonPayload;
    jsonPayload["username"] = username;
    jsonPayload["content"] = content;
    if (!avatarUrl.empty())
    {
        jsonPayload["avatar_url"] = avatarUrl;
    }

    std::string payloadString = jsonPayload.dump();
    std::cout << "Payload: " << payloadString << std::endl;

    try
    {
        cURLpp::Easy request;
        request.setOpt(new cURLpp::options::Url(webhookUrl));
        request.setOpt(new cURLpp::options::CustomRequest("POST"));
        request.setOpt(new cURLpp::options::HttpHeader({"Content-Type: application/json"}));
        request.setOpt(new cURLpp::options::PostFields(payloadString));
        request.setOpt(new cURLpp::options::PostFieldSize(payloadString.size()));

        request.perform();
        std::cout << "\x1B[32;1mSuccess!\x1B[0m" << std::endl;
    }
    catch (cURLpp::RuntimeError& e)
    {
        std::cerr << "cURL Error: " << e.what() << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void deleteWebhook(const std::string& webhookUrl)
{
    try
    {
        cURLpp::Easy request;
        request.setOpt(new cURLpp::options::Url(webhookUrl));
        request.setOpt(new cURLpp::options::CustomRequest("DELETE"));
        request.perform();
        std::cout << "Webhook \x1B[31;4mDEMOLISHED\x1B[0m >:3" << std::endl;
    }
    catch (cURLpp::RuntimeError& e)
    {
        std::cerr << "cURL Error: " << e.what() << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
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
#endif
    cURLpp::Cleanup cleanup;

    bool deleteWebhookFlag = false;
    std::string webhookUrl;
    std::string username;
    std::string content;
    std::string avatarUrl;

    for (int i=1; i < argc; ++i)
    {
        if (std::string(argv[i]) == "--delete" && (i + 1) < argc)
        {
            deleteWebhookFlag = true;
            webhookUrl = argv[i + 1];
        }
        else if (std::string(argv[i]) == "--json" && (i + 1) < argc)
        {
            std::string filepath = argv[i + 1];
            std::ifstream jsonFile(filepath);
            if (!jsonFile.is_open()) 
            {
                std::cerr << "Could not open the file: " << filepath << std::endl;
                return 1;
            }
            try
            {
                nlohmann::json json;
                jsonFile >> json;

                webhookUrl = json.at("webhookUrl").get<std::string>();
                username = json.at("username").get<std::string>();
                content = json.at("content").get<std::string>();
                avatarUrl = json.value("avatarUrl", "");
            }
            catch (nlohmann::json::exception& e)
            {
                std::cerr << "JSON Error: " << e.what() << std::endl;
                return 1;
            }
        }
        else if (std::string(argv[i]) == "--username" && (i + 1) < argc)
        {
            username = argv[i + 1];
        }
        else if (std::string(argv[i]) == "--content" && (i + 1) < argc)
        {
            content = argv[i + 1];
        }
        else if (std::string(argv[i]) == "--avatarUrl" && (i + 1) < argc) 
        {
            avatarUrl = argv[i + 1];
        }
        else if (std::string(argv[i]) == "--help")
        {
            hell();
            return 0;
        }
    }

    if (deleteWebhookFlag)
    {
        if (webhookUrl.empty())
        {
            std::cerr << "Webhook URL must be provided for deletion." << std::endl;
            return 1;
        }
        deleteWebhook(webhookUrl);
    }
    else
    {
        if (webhookUrl.empty())
        {
            std::cout << "Enter webhook URL: ";
            std::getline(std::cin, webhookUrl);
        }

        if (username.empty())
        {
            std::cout << "Enter username: ";
            std::getline(std::cin, username);
        }

        if (content.empty())
        {
            std::cout << "Enter message: ";
            std::getline(std::cin, content);
        }

        if (avatarUrl.empty())
        {
            std::cout << "Enter avatar URL (leave blank for default): ";
            std::getline(std::cin, avatarUrl);
        }

        sendWebhook(webhookUrl, username, content, avatarUrl);
    }

    return 0;
}