#include <iostream>
#include <sstream>
#include <string>
#include <cURLpp/cURLpp.hpp>
#include <cURLpp/Easy.hpp>
#include <cURLpp/Options.hpp>
#include <nlohmann/json.hpp>

const std::string RESET = "\x1B[0m";
const std::string BOLD = "\x1B[1m";
const std::string UNDERLINE = "\x1B[4m";
const std::string CYAN = "\x1B[36m";
const std::string YELLOW = "\x1B[33m";
const std::string GREEN = "\x1B[32m";
const std::string RED = "\x1B[31m";

const static void hell()
{
    std::cout << "Usage:\n"
              << "  webhook_sender [options]\n\n"
              << "Options:\n"
              << "  --delete <webhookUrl>    Delete the specified webhook.\n"
              << "  --json <file>            Load webhook parameters from a JSON file.\n"
              << "  --username <name>        Specify the username.\n"
              << "  --content <message>      Specify the message content.\n"
              << "  --avatarUrl <url>        Specify the avatar URL.\n"
              << "  --dump <webhookUrl>      Dump information about the specified webhook.\n"
              << "  --help                   Display this help message.\n";
}

const static void dump(const std::string& webhookUrl)
{
    try
    {
        cURLpp::Easy request;
        request.setOpt(new cURLpp::options::Url(webhookUrl));
        request.setOpt(new cURLpp::options::CustomRequest("GET"));

        std::ostringstream responseStream;
        request.setOpt(new cURLpp::options::WriteStream(&responseStream));

        request.perform();

        nlohmann::json jsonResponse = nlohmann::json::parse(responseStream.str());

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
            std::cout << RED << "Error: Webhook not found." << RESET;
        }
    }
    catch (cURLpp::RuntimeError& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

const static void sendWebhook(const std::string& webhookUrl, const std::string& username, const std::string& content, const std::string& avatarUrl)
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
        std::cout << GREEN << ";1mSuccess!" << RESET << std::endl;
    }
    catch (cURLpp::RuntimeError& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

const static void deleteWebhook(const std::string& webhookUrl)
{
    try
    {
        cURLpp::Easy request;
        request.setOpt(new cURLpp::options::Url(webhookUrl));
        request.setOpt(new cURLpp::options::CustomRequest("DELETE"));

        request.perform();
        std::cout << "Webhook " << RED << "DEMOLISHED" << RESET << " successfully >:3" << std::endl;
    }
    catch (cURLpp::RuntimeError& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
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
    cURLpp::Cleanup;

    std::string webhookUrl;
    std::string username = "Webhook";
    std::string content;
    std::string avatarUrl;
    bool dumpWebhookFlag = false;
    bool deleteWebhookFlag = false;

    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]) == "--help")
        {
            hell();
            return 0;
        }
        else if (std::string(argv[i]) == "--dump" && (i + 1) < argc)
        {
            dumpWebhookFlag = true;
            webhookUrl = argv[++i];
        }
        else if (std::string(argv[i]) == "--delete" && (i + 1) < argc)
        {
            deleteWebhookFlag = true;
            webhookUrl = argv[++i];
        }
        else if (std::string(argv[i]) == "--username" && (i + 1) < argc)
        {
            username = argv[++i];
        }
        else if (std::string(argv[i]) == "--content" && (i + 1) < argc)
        {
            content = argv[++i];
        }
        else if (std::string(argv[i]) == "--avatarUrl" && (i + 1) < argc)
        {
            avatarUrl = argv[++i];
        }
    }

    if (deleteWebhookFlag)
    {
        deleteWebhook(webhookUrl);
    }
    else if (dumpWebhookFlag)
    {
        dump(webhookUrl);
    }
    else if (!content.empty())
    {
        sendWebhook(webhookUrl, username, content, avatarUrl);
    }
    else
    {
        std::cerr << "No valid operation specified. Use --help for usage information." << std::endl;
    }

    return 0;
}