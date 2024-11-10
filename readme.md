<div align="center">
    <h1>Discord Webhook Manager (CLI)</h1>
    <h2>Building</h2>
</div>

```bat
@REM Also make sure to set the VCPKG_ROOT as a system variable
vcpkg install curlpp nlohmann-json boost-filesystem boost-system
git clone https://github.com/YOUWILLDIE666/discord-webhook.git && cd discord-webhook
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake
cmake --build .
```

<div align="center">
    <h2>Help</h2>
</div>

```bat
@REM General options:
discord-webhook.exe  --delete <url>                @REM Delete the specified webhook.
discord-webhook.exe  --dump <url>                  @REM Dump information about the specified webhook.
discord-webhook.exe  --send <url>                  @REM Send a POST request to the specified webhook with the specified parameters.
discord-webhook.exe  --json <file>                 @REM Load webhook parameters from a JSON file.
discord-webhook.exe  --username <name>             @REM Specify the username.
discord-webhook.exe  --content <text>              @REM Specify the message content.
discord-webhook.exe  --avatar-url <url>            @REM Specify the avatar URL.
discord-webhook.exe  --file <path>                 @REM Specify the path to the file to attach.

@REM Embed options:
discord-webhook.exe  --title <title>               @REM Specify the embed title.
discord-webhook.exe  --description <text>          @REM Specify the embed description.
discord-webhook.exe  --color <color>               @REM Specify the embed color
discord-webhook.exe  --footer <footer>             @REM Specify the embed footer text.
discord-webhook.exe  --footer-icon <url>           @REM Specify the embed footer icon URL.

discord-webhook.exe  --help                        @REM Display this help message.
```

<div align="center">
    <h2>JSON Example</h2>
</div>

```json
{
    "webhook-url": "https://discord.com/api/webhooks/123/abc",
    "username": "Username (any)",
    "content": "Message example",
    "avatar-url": "https://example.com/avatar_icon.png",
    "embed-title": "Embed Title",
    "embed-description": "This is an embed description.",
    "embed-color": "16711680", // decimal
    "embed-color": "#FF0000",  // hex
    "embed-footer": "Footer text",
    "embed-footer-icon": "https://example.com/footer_icon.png",
    "file": "path/to/file.txt",
    "dump": false,
    "delete": false,
    "send": true
}
```
