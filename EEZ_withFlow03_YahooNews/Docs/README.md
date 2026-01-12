# Yahoo News Viewer for M5Stack Tab5

M5Stack Tab5用のYahoo Newsビューアーアプリケーション。EEZ Studioで作成したGUIを使用し、WiFi経由でYahoo NewsのRSSフィードを取得して表示します。

<img src="https://img.shields.io/badge/Platform-M5Stack%20Tab5-blue" alt="Platform">
<img src="https://img.shields.io/badge/LVGL-v8.3.11-green" alt="LVGL">
<img src="https://img.shields.io/badge/EEZ%20Studio-with%20Flow-orange" alt="EEZ Studio">

## 📋 概要

このアプリケーションは、EEZ Studioで作成したGUIとFlow機能を使用し、Yahoo NewsのRSSフィードを取得して画面に表示するサンプルプログラムです。テンプレートとして使用でき、他のRSSフィードやWebサービスとの連携に応用できます。

### 主な機能

- ✅ WiFi接続と状態表示（切断/接続中/接続済）
- ✅ Yahoo News RSS取得（HTTPS通信）
- ✅ 最大10件のニュース表示
- ✅ スクロール可能なテキストエリア
- ✅ 手動更新ボタン
- ✅ 日本語フォント対応
- ✅ EEZ Studio Flow連携
- ✅ WiFi自動再接続

## 🔧 必要な環境

### ハードウェア
- **M5Stack Tab5** (ESP32-P4ベース、7インチディスプレイ)

### ソフトウェア
- Arduino IDE 2.x または Arduino CLI
- M5Stack ボードマネージャー

### ライブラリ

| ライブラリ | バージョン | 説明 |
|-----------|----------|------|
| M5Unified | >= 0.2.10 | M5Stack統合ライブラリ |
| LVGL | = 8.3.11 | Light and Versatile Graphics Library |
| WiFi | 標準 | ESP32 WiFiライブラリ |
| HTTPClient | 標準 | HTTP/HTTPSクライアント |

#### インストール方法

**Arduino IDE:**
```
ライブラリマネージャーから以下をインストール：
- M5Unified (v0.2.10以上)
- lvgl (v8.3.11)
```

**Arduino CLI:**
```bash
arduino-cli lib install "M5Unified@0.2.10"
arduino-cli lib install "lvgl@8.3.11"
```

## ⚡ クイックスタート

### ステップ 1: WiFi設定 (1分)

```bash
cd /path/to/EEZ_withFlow03_YahooNews
cp secrets.h.example secrets.h
```

`secrets.h` を編集：

```cpp
const char* ssid = "あなたのWiFi名";
const char* password = "あなたのWiFiパスワード";
```

### ステップ 2: コンパイル & アップロード (2分)

**Arduino IDE:**
1. `EEZ_withFlow03_YahooNews.ino` を開く
2. ボード: `M5Stack Tab5` を選択
3. アップロードボタンをクリック

**Arduino CLI:**
```bash
# ビルドキャッシュをクリア（初回のみ）
rm -rf ~/Library/Caches/arduino/sketches/*

# コンパイル & アップロード
arduino-cli compile -b m5stack:esp32:m5stack_tab5
arduino-cli upload -b m5stack:esp32:m5stack_tab5 -p /dev/cu.usbserial-*
```

### ステップ 3: 実行確認

デバイスが再起動し、以下の動作が確認できます：

1. **起動** (2秒) - M5Unified、LVGL、EEZ Studio UI初期化
2. **WiFi接続** (5秒) - 画面右上に "WiFi: 接続中..." → "WiFi: 接続済"
3. **ニュース取得** (3秒) - 画面左上に "取得中..." → "取得完了"
4. **表示** - テキストエリアに最大10件のニュースタイトル

🎉 **完了！**

## 💻 使用方法

### 画面レイアウト

```
┌────────────────────────────────────┐
│ [取得完了]              [WiFi:接続済]│ ← ステータス
├────────────────────────────────────┤
│ [更新]                              │ ← 更新ボタン
├────────────────────────────────────┤
│          Yahoo News                │ ← タイトル
├────────────────────────────────────┤
│                                    │
│  1. ニュースタイトル1               │
│  2. ニュースタイトル2               │ ← ニュース
│  3. ニュースタイトル3               │   (スクロール可能)
│  ...                               │
│                                    │
└────────────────────────────────────┘
```

### 操作方法

- **更新ボタン**: 画面左上のボタンをタップすると最新ニュースを取得
- **スクロール**: テキストエリアをスワイプしてスクロール
- **WiFi監視**: 10秒ごとに自動チェック、切断時は自動再接続

## 🐛 トラブルシューティング

### WiFiに接続できない

**症状**: `WiFi connection timeout!`

**対策**:
1. `secrets.h` のSSIDとパスワードを確認
2. WiFiルーターが2.4GHz帯で動作しているか確認（5GHzは非対応）
3. WiFiルーターの近くで試す
4. デバイスを再起動

### コンパイルエラー（ビルドキャッシュ）

**症状**: `riscv32-esp-elf-g++: fatal error: cannot specify '-o' with '-c'`

**対策**:
```bash
# ビルドキャッシュをクリア
rm -rf ~/Library/Caches/arduino/sketches/*
# 再コンパイル
arduino-cli compile -b m5stack:esp32:m5stack_tab5
```

### コンパイルエラー（ファイル不足）

**症状**: `ui.h: No such file or directory`

**対策**:
- EEZ Studio生成ファイルが全て存在するか確認
- 必要ファイル: `ui.h`, `ui.c`, `screens.h`, `screens.c`, `vars.h`, `eez-flow.h`, `eez-flow.cpp`

### 画面が真っ暗

**症状**: プログラムは動作するが画面に何も表示されない

**対策**:
1. シリアルモニターでエラーを確認（ボーレート: 115200）
2. USB給電が十分か確認（外部電源推奨）
3. デバイスを再起動

### ニュースが表示されない

**症状**: WiFi接続は成功するがニュースが表示されない

**対策**:
1. シリアルモニターで "HTTP Response code: 200" を確認
2. "Display updated" メッセージを確認
3. Yahoo News RSSが利用可能か確認

## ⚙️ 基本的なカスタマイズ

### RSS URLの変更

```cpp
// EEZ_withFlow03_YahooNews.ino の47行目付近
#define YAHOO_NEWS_RSS_URL "https://news.yahoo.co.jp/rss/topics/domestic.xml"
```

**利用可能なYahoo News RSS**:
- 主要ニュース: `/rss/topics/top-picks.xml`
- 国内ニュース: `/rss/topics/domestic.xml`
- 国際ニュース: `/rss/topics/world.xml`
- 経済ニュース: `/rss/topics/business.xml`
- IT・科学: `/rss/topics/it.xml`

### ニュース取得数の変更

```cpp
// EEZ_withFlow03_YahooNews.ino の48行目付近
#define MAX_NEWS_ITEMS 20  // 最大20件に変更
```

### WiFiタイムアウトの変更

```cpp
// EEZ_withFlow03_YahooNews.ino の39-40行目付近
#define WIFI_CONNECT_TIMEOUT_MS 30000  // 30秒に変更
#define WIFI_CHECK_INTERVAL_MS 5000    // 5秒ごとにチェック
```

## 📚 詳細ドキュメント

より詳しい情報は、以下のドキュメントを参照してください：

- **[TECHNICAL_GUIDE.md](./TECHNICAL_GUIDE.md)** - 技術仕様、プログラム構造、Flow変数
- **[CUSTOMIZATION_GUIDE.md](./CUSTOMIZATION_GUIDE.md)** - 高度なカスタマイズ、EEZ Studio、テンプレート活用

## 🔗 参考リンク

- [M5Stack Tab5 公式ページ](https://docs.m5stack.com/en/core/M5Stack%20Tab5)
- [M5Unified ライブラリ](https://github.com/m5stack/M5Unified)
- [LVGL 公式サイト](https://lvgl.io/)
- [EEZ Studio 公式サイト](https://www.envox.eu/eez-studio/)
- [Yahoo!ニュース RSS](https://news.yahoo.co.jp/)

## 📝 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2026

---

**作成日**: 2026年1月12日  
**最終更新**: 2026年1月12日  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み
