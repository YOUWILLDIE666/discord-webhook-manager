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
DiscordWebhook.exe --delete webhookl_url
DiscordWebhook.exe --json input.json
DiscordWebhook.exe --username "YourUsername" --content "Your message here" --avatarUrl "https://example.com/avatar.png"
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
