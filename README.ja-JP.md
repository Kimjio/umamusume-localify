# umamusume-localify
## ウマ娘 プリティーダービー ローカライズパッチ

[韓国語](README.ko-KR.md)
[中国語](README.zh-Hans.md)

### 使用方法:
1. `umamusume.exe`の格納先に`version.dll`、`config.json`と`dicts`を配置します。 
2. ゲームを起動で適用されます

### 設定:
- `enableConsole` デバッグ情報を表示するコンソールを有効化します。 (true/false)
- `enableLogger` 未翻訳の文字列を`dump.txt`に出力します。 (true/false)
- `dumpStaticEntries` ハードコードされた文字列を`dump.txt`に出力します。
- `maxFps` 最大FPS制限を設定します。 (-1 = 通常/0 = 無制限/>0 = #fpsでFPS値を固定)
    - 注意: VSyncは有効化されています
- `unlockSize` `1080p`以上の解像度を使用可能にします。 (true/false)
- `uiScale` UIのスケールを変更します。
- `replaceFont` すべてのフォントをデフォルトフォントに置き換えます。これを使用すると単語の欠落問題が解決します。 (true/false)
- `autoFullscreen` 画面のアスペクト比が正しい場合にゲームをフルスクリーンで表示します。 (true/false)
    - 画面のアスペクト比が16:9の場合、ストーリーまたはライブを横で表示するとフルスクリーンになります。
    - 画面のアスペクト比が9:16の場合、ほとんどの表示がフルスクリーンで表示されます。
- `dicts` 翻訳ファイルのリストを設定します (パス)

### 既知の問題
- 特になし

# リソース
- `static.json` upgrade tool - https://github.com/AirJerryWhite/i18upgrade

# クレジット
## 翻訳
### schinese
- @AirJerryWhite
- @Yaklo
- @july20026
- @RikkaTakanaishi
- @羲羲
### tchinese
- @yotv2000tw
