<div align="center">
    <h2>Building</h2>
</div>

```bat
@REM Also make sure to set the VCPKG_ROOT as a system variable
vcpkg install curlpp nlohmann-json
git clone https://github.com/YOUWILLDIE666/discord-webhook.git && cd discord-webhook
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake
cmake --build .
```

```bat
@REM Arguments that can be passed:
discord-webhook.exe  --delete <url>                @REM Delete the specified webhook.
discord-webhook.exe  --json <file>                 @REM Load webhook parameters from a JSON file.
discord-webhook.exe  --username <name>             @REM Specify the username.
discord-webhook.exe  --content <message>           @REM Specify the message content.
discord-webhook.exe  --avatarUrl <url>             @REM Specify the avatar URL.
discord-webhook.exe  --webhookUrl <url>            @REM Specify the webhook URL.
discord-webhook.exe  --dump <url>                  @REM Dump information about the specified webhook.
discord-webhook.exe  --help                        @REM Display this help message.
```

<div align="center">
    <h2>Example JSON</h2>
</div>

```json
{
    "webhookUrl": "https://discord.com/api/webhooks/1234567890/abcdefg",
    "username": "Your username",
    "content": "Your message here",
    "avatarUrl": "https://example.com/avatar.png"
}
```
