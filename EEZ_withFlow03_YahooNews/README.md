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

## 🚀 クイックスタート

### 1. WiFi設定

```bash
cp secrets.h.example secrets.h
# secrets.h を編集してWiFi情報を設定
```

### 2. コンパイル & アップロード

```bash
arduino-cli compile -b m5stack:esp32:m5stack_tab5
arduino-cli upload -b m5stack:esp32:m5stack_tab5 -p /dev/cu.usbserial-*
```

### 3. 実行

デバイスが再起動し、自動的にニュースを取得・表示します。

## 📚 ドキュメント

詳細なドキュメントは `Docs/` フォルダに整理されています：

### [README.md](./Docs/README.md) - 基本ガイド
- プロジェクト概要と主な機能
- 必要な環境（ハードウェア、ソフトウェア、ライブラリ）
- クイックスタート（3ステップ）
- 使用方法と操作説明
- トラブルシューティング
- 基本的なカスタマイズ

**対象**: すべてのユーザー  
**所要時間**: 5分

---

### [TECHNICAL_GUIDE.md](./Docs/TECHNICAL_GUIDE.md) - 技術仕様
- プログラム構造とファイル構成
- 技術仕様（ハードウェア、ソフトウェア）
- Flow変数とアクション
- データフロー（起動、更新、ループ）
- メモリ管理
- 通信仕様（WiFi、HTTP/HTTPS、RSS）
- パフォーマンス

**対象**: 開発者、技術者  
**所要時間**: 20分

---

### [CUSTOMIZATION_GUIDE.md](./Docs/CUSTOMIZATION_GUIDE.md) - カスタマイズガイド
- 設定変更（初級）
  - RSS URL、取得数、タイムアウト、明るさ
- EEZ Studio UI設計（中級）
  - プロジェクトの開き方、画面設計、ウィジェット、フォント
- コード改造（上級）
  - Flow変数の追加、動的URL、自動更新機能
- テンプレート活用
  - 天気予報、株価表示、IoTダッシュボードへの応用
  - テンプレート化の手順

**対象**: カスタマイズしたいユーザー、UI設計者  
**所要時間**: 30分

---

## 📊 プロジェクト統計

```
コード:
- メインプログラム: 497行（16KB）
- セクション数: 9
- 関数数: 18

ドキュメント:
- 総ページ数: 3ファイル
- 総行数: 1,216行
- 総容量: 34KB
```

## 🎯 学習パス

### 初心者向け（20分）
1. [Docs/README.md](./Docs/README.md) - 概要把握（5分）
2. クイックスタート - 環境構築・実行（10分）
3. 動作確認（5分）

### 開発者向け（50分）
1. [Docs/README.md](./Docs/README.md) - 概要把握（5分）
2. [Docs/TECHNICAL_GUIDE.md](./Docs/TECHNICAL_GUIDE.md) - 技術仕様（20分）
3. [Docs/CUSTOMIZATION_GUIDE.md](./Docs/CUSTOMIZATION_GUIDE.md) - カスタマイズ（25分）

### テンプレート活用（3時間）
1. [Docs/README.md](./Docs/README.md) - 概要把握（5分）
2. [Docs/TECHNICAL_GUIDE.md](./Docs/TECHNICAL_GUIDE.md) - 仕様理解（30分）
3. [Docs/CUSTOMIZATION_GUIDE.md](./Docs/CUSTOMIZATION_GUIDE.md) - テンプレート化（2時間30分）

## 🔗 参考リンク

- [M5Stack Tab5 公式ページ](https://docs.m5stack.com/en/core/M5Stack%20Tab5)
- [M5Unified ライブラリ](https://github.com/m5stack/M5Unified)
- [LVGL 公式サイト](https://lvgl.io/)
- [EEZ Studio 公式サイト](https://www.envox.eu/eez-studio/)
- [Yahoo!ニュース RSS](https://news.yahoo.co.jp/)

## 📝 ライセンス

このプログラムはMITライセンスの下で公開されています。

---

**作成日**: 2026年1月12日  
**最終更新**: 2026年1月12日  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み  
**ドキュメント版**: v2.0（整理版）

