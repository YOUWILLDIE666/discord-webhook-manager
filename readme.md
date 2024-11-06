<div align="center">
    <h2>Building</h2>
</div>

```bat
vcpkg install curlpp nlohmann-json
git clone https://github.com/YOUWILLDIE666/discord-webhook.git && cd discord-webhook
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake
cmake --build .
```

```bat
@REM Arguments that can be passed:
DiscordWebhook.exe  --delete <url>                @REM Delete the specified webhook.
DiscordWebhook.exe  --json <file>                 @REM Load webhook parameters from a JSON file.
DiscordWebhook.exe  --username <name>             @REM Specify the username.
DiscordWebhook.exe  --content <message>           @REM Specify the message content.
DiscordWebhook.exe  --avatarUrl <url>             @REM Specify the avatar URL.
DiscordWebhook.exe  --webhookUrl <url>            @REM Specify the webhook URL.
DiscordWebhook.exe  --dump <url>                  @REM Dump information about the specified webhook.
DiscordWebhook.exe  --help                        @REM Display this help message.
```

<div align="center">
    <h2>Example JSON</h2>
</div>

```json
{
    "webhookUrl": "https://discord.com/api/webhooks/1234567890/abcdefg",
    "username": "YourUsername",
    "content": "Your message here",
    "avatarUrl": "https://example.com/avatar.png"
}
```
