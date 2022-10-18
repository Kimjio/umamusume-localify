# umamusume-localify
## 우마무스메 Windows 클라이언트 현지화 패치

[English](README.md)
[中国人](README.zh-Hans.md)

### Usage:
1. `umamusume.exe` 가 있는 곳에 `version.dll`, `config.json` 파일, `dicts` 에서 참조하는 번역 파일을 넣습니다.
2. 게임을 실행하면 됩니다.

### Config:
- `enableConsole` Enable the console for printing debug infomations (true/false)
- `enableLogger` Output uncovered text entries into `dump.txt` (true/false)
- `dumpStaticEntries` Requires ^, Dump hardcoded text entries into `dump.txt`
- `maxFps` Max fps limit (-1 = Unmodified/0 = Unlimited/>0 = Lock to #fps)
    - Note: VSync is enabled
- `unlockSize` Allow game to use resolutions that bigger than `1080p` (true/false)
- `uiScale` Custom UI scale
- `replaceFont` Replace all font to default font, solves missing word issue (true/false)
- `autoFullscreen` The game will be fullscreen if screen aspect ratio is correct (true/false)
    - If screen aspect ratio is 16:9, the game will be fullscreen when playing story/live with landscape.
    - If screen aspect ratio is 9:16, the game will be fullscreen in most case.
- `dicts` A list of dicts that read by this (Path)

### Known issue
- None

# Resources
- `static.json` upgrade tool - https://github.com/AirJerryWhite/i18upgrade

# Credits
## Translation
### schinese
- @AirJerryWhite
- @Yaklo
- @july20026
- @RikkaTakanaishi
- @羲羲
### tchinese
- @yotv2000tw
