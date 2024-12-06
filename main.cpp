#include <fstream>
#include <sstream>
#include <cURLpp/cURLpp.hpp>
#include <cURLpp/Easy.hpp>
#include <cURLpp/Options.hpp>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

#ifdef USEBOOST

# include <boost/filesystem.hpp>

#else // ^^^ defined(USEBOOST) ^^^ VVV !defined(USEBOOST) VVV

# include <filesystem>

#endif // ^^^ !defined(USEBOOST) ^^^

const std::string RESET = "\x1B[0m";
const std::string BOLD = "\x1B[1m";
const std::string UNDERLINE = "\x1B[4m";
const std::string CYAN = "\x1B[36m";
const std::string YELLOW = "\x1B[33m";
const std::string GREEN = "\x1B[32m";
const std::string RED = "\x1B[31m";

int toDecimal(const std::string& hexColor)
{
    return std::stoi(hexColor.substr(1), nullptr, 16);
}

bool fileGE(const std::string& filePath)
{

#ifdef USEBOOST

    boost::filesystem::path path(filePath);

#else // ^^^ defined(USEBOOST) ^^^ VVV !defined(USEBOOST) VVV

    std::filesystem::path path(filePath);

#endif // ^^^ !defined(USEBOOST) ^^^

#ifdef USEBOOST

    if (!boost::filesystem::exists(path))

#else // ^^^ defined(USEBOOST) ^^^ VVV !defined(USEBOOST) VVV

    if (!std::filesystem::exists(path))

#endif // ^^^ !defined(USEBOOST) ^^^

    {
        std::cerr << RED << "FATAL: file does not exist." << RESET << std::endl;
        return false;
    }

#ifdef USEBOOST

    if (!boost::filesystem::is_regular_file(path))

#else // ^^^ defined(USEBOOST) ^^^ VVV !defined(USEBOOST) VVV

    if (!std::filesystem::is_regular_file(path))

#endif // ^^^ !defined(USEBOOST) ^^^

    {
        std::cerr << RED << "FATAL: path is not a regular file." << RESET << std::endl;
        return false;
    }

#ifdef USEBOOST

    std::uintmax_t fileSize = boost::filesystem::file_size(path);

#else // ^^^ defined(USEBOOST) ^^^ VVV !defined(USEBOOST) VVV

    std::uintmax_t fileSize = std::filesystem::file_size(path);

#endif // ^^^ !defined(USEBOOST) ^^^

    constexpr std::uintmax_t eightMB = 8 * 1024 * 1024; // 8 MB
    return fileSize >= eightMB;
}

inline void hell()
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
              << "  --file <path>                 Specify the path to the file to attach (must be less than 8MB).\n\n"
              << "Embed Options:\n"
              << "  --title <title>               Specify the embed title.\n"
              << "  --description <text>          Specify the embed description.\n"
              << "  --color <color>               Specify the embed color.\n"
              << "  --footer <footer>             Specify the embed footer text.\n"
              << "  --footer-icon <url>           Specify the embed footer icon URL.\n\n"
              << "  --help                        Display this help message.\n";
}

inline void loadJSON(const std::string& filePath, std::string& webhookUrl, std::string& username,
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

    webhookUrl = jsonData.value("webhook-url", webhookUrl);
    username = jsonData.value("username", username);
    content = jsonData.value("content", content);
    avatarUrl = jsonData.value("avatar-url", avatarUrl);
    embedTitle = jsonData.value("embed-title", embedTitle);
    embedDescription = jsonData.value("embed-description", embedDescription);
    embedColor = jsonData.value("embed-color", embedColor);
    embedFooter = jsonData.value("embed-footer", embedFooter);
    embedFooterIcon = jsonData.value("embed-footer-icon", embedFooterIcon);
    file = jsonData.value("file", file);
    dumpWebhookFlag = jsonData.value("dump", dumpWebhookFlag);
    deleteWebhookFlag = jsonData.value("delete", deleteWebhookFlag);
    sendWebhookFlag = jsonData.value("send", sendWebhookFlag);

    if (!embedColor.empty() && embedColor[0] == '#')
    {
        embedColor = std::to_string(toDecimal(embedColor));
    }
}

inline void dumpWebhook(const std::string& webhookUrl)
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

const inline void sendWebhook(const std::string& webhookUrl, const std::string& username, const std::string& content,
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

    if (!embedTitle.empty() || !embedDescription.empty() || !embedColor.empty() || !embedFooter.empty() || !embedFooterIcon.empty())
    {
        nlohmann::json embed;
        if (!embedTitle.empty()) embed["title"] = embedTitle;
        if (!embedDescription.empty()) embed["description"] = embedDescription;
        if (!embedColor.empty()) embed["color"] = embedColor;
        if (!embedFooter.empty()) embed["footer"]["text"] = embedFooter;
        if (!embedFooterIcon.empty()) embed["footer"]["icon_url"] = embedFooterIcon;

        jsonPayload["embeds"] = { embed };
    }

    CURL *curl = curl_easy_init();
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
        CURLcode res = curl_easy_perform(curl);

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

const inline void deleteWebhook(const std::string& webhookUrl)
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

int main(int argc, const char* argv[])
{
#ifdef _WIN32

#   include <Windows.h>
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

    std::unordered_map<std::string, std::function<void(int)>> argmap = {
        {"--help", [](int) { hell(); }},
        {"--json", [&](int i)
        {
            if (i + 1 < argc)
            {
                loadJSON(argv[++i], webhookUrl, username, content, avatarUrl, embedTitle, embedDescription,
                         embedColor, embedFooter, embedFooterIcon, filePath, dumpWebhookFlag, deleteWebhookFlag, sendWebhookFlag);
            }
            else
            {
                std::cerr << RED << "FATAL: --json requires an argument." << RESET << std::endl;
                exit(1);
            }
        }},
        {"--dump", [&](int i)
        {
            if (i + 1 < argc)
            {
                dumpWebhookFlag = true;
                webhookUrl = argv[++i];
            }
            else
            {
                std::cerr << RED << "FATAL: --dump requires an argument." << RESET << std::endl;
                exit(1);
            }
        }},
        {"--delete", [&](int i)
        {
            if (i + 1 < argc)
            {
                deleteWebhookFlag = true;
                webhookUrl = argv[++i];
            }
            else
            {
                std::cerr << RED << "FATAL: --delete requires an argument." << RESET << std::endl;
                exit(1);
            }
        }},
        {"--send", [&](int i)
        {
            if (i + 1 < argc)
            {
                sendWebhookFlag = true;
                webhookUrl = argv[++i];
            }
            else
            {
                std::cerr << RED << "FATAL: --send requires an argument." << RESET << std::endl;
                exit(1);
            }
        }},
        {"--username", [&](int i)
        {
            if (i + 1 < argc)
            {
                username = argv[++i];
            }
            else
            {
                std::cerr << RED << "FATAL: --username requires an argument." << RESET << std::endl;
                exit(1);
            }
        }},
        {"--content", [&](int i)
        {
            if (i + 1 < argc)
            {
                content = argv[++i];
            } else
            {
                std::cerr << RED << "FATAL: --content requires an argument." << RESET << std::endl;
                exit(1);
            }
        }},
        {"--avatar-url", [&](int i)
        {
            if (i + 1 < argc)
            {
                avatarUrl = argv[++i];
            }
            else
            {
                std::cerr << RED << "FATAL: --avatar-url requires an argument." << RESET << std::endl;
                exit(1);
            }
        }},
        {"--file", [&](int i)
        {
            if (i + 1 < argc)
            {
                filePath = argv[++i];
            }
            else
            {
                std::cerr << RED << "FATAL: --file requires an argument." << RESET << std::endl;
                exit(1);
            }
        }},
        {"--title", [&](int i)
        {
            if (i + 1 < argc)
            {
                embedTitle = argv[++i];
            }
            else
            {
                std::cerr << RED << "FATAL: --title requires an argument." << RESET << std::endl;
                exit(1);
            }
        }},
        {"--description", [&](int i)
        {
            if (i + 1 < argc)
            {
                embedDescription = argv[++i];
            }
            else
            {
                std::cerr << RED << "FATAL: --description requires an argument." << RESET << std::endl;
                exit(1);
            }
        }},
        {"--color", [&](int i)
        {
            if (i + 1 < argc)
            {
                embedColor = argv[++i];
            }
            else
            {
                std::cerr << RED << "FATAL: --color requires an argument." << RESET << std::endl;
                exit(1);
            }
        }},
        {"--footer", [&](int i)
        {
            if (i + 1 < argc)
            {
                embedFooter = argv[++i];
            }
            else
            {
                std::cerr << RED << "FATAL: --footer requires an argument." << RESET << std::endl;
                exit(1);
            }
        }},
        {"--footer-icon", [&](int i)
        {
            if (i + 1 < argc)
            {
                embedFooterIcon = argv[++i];
            }
            else
            {
                std::cerr << RED << "FATAL: --footer-icon requires an argument." << RESET << std::endl;
                exit(1);
            }
        }},
    };

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (argmap.find(arg) != argmap.end())
        {
            argmap[arg](i);
        }
        else
        {
            std::cerr << RED << "FATAL: unknown argument \"" << arg << "\"" << RESET << std::endl;
            exit(1);
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
