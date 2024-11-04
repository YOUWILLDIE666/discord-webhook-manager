#include <iostream>
#include <fstream>
#include <string>
#include <cURLpp/cURLpp.hpp>
#include <cURLpp/Easy.hpp>
#include <cURLpp/Options.hpp>
#include <nlohmann/json.hpp>

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

    bool deleteWebhook = false;
    std::string webhookUrl;
    std::string username;
    std::string content;

    for (int i=1; i < argc; ++i)
    {
        if (std::string(argv[i]) == "--delete" && (i + 1) < argc) {
            deleteWebhook = true;
            webhookUrl = argv[i + 1];
            break;
        }
        else if (std::string(argv[i]) == "--json-file" && (i + 1) < argc) {
            std::string filepath = argv[i + 1];
            std::ifstream jsonFile(filepath);
            if (!jsonFile.is_open()) {
                std::cerr << "Could not open the file: " << filepath << std::endl;
                return 1;
            }

            try {
                nlohmann::json json;
                jsonFile >> json;

                webhookUrl = json.at("webhookUrl").get<std::string>();
                username = json.at("username").get<std::string>();
                content = json.at("content").get<std::string>();
            }
            catch (nlohmann::json::exception& e) {
                std::cerr << "JSON parse error: " << e.what() << std::endl;
                return 1;
            }
        }
    }

    if (deleteWebhook) {
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
            std::cerr << e.what() << std::endl;
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
    else {
        if (webhookUrl.empty())
        {
            std::cout << "Enter webhook URL: ";
            std::getline(std::cin, webhookUrl);
        }

        std::cout << "Enter username: ";
        std::getline(std::cin, username);

        std::cout << "Enter message: ";
        std::getline(std::cin, content);

        std::string jsonPayload = R"({"username": ")" + username + R"(", "content": ")" + content + R"("})";
        std::cout << jsonPayload << std::endl;

        try
        {
            cURLpp::Easy request;

            request.setOpt(new cURLpp::options::Url(webhookUrl));
            request.setOpt(new cURLpp::options::CustomRequest("POST"));
            request.setOpt(new cURLpp::options::HttpHeader({ "Content-Type: application/json" }));
            request.setOpt(new cURLpp::options::PostFields(jsonPayload));
            request.setOpt(new cURLpp::options::PostFieldSize(jsonPayload.size()));

            request.perform();

            std::cout << "\x1B[32;1mSuccess!\x1B[0m" << std::endl;
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

    return 0;
}