# WIFI - WiFi接続デモ

M5Stack Tab5のWiFi機能を使用して、WiFiネットワークに接続するデモプログラムです。

## 📋 概要

このプログラムは、M5Stack Tab5のWiFi機能を使用して、指定されたWiFiネットワークに接続し、取得したIPアドレスを画面に表示します。STAモード（ステーションモード）とAPモード（アクセスポイントモード）の両方に対応しています。

## 🔧 必要なハードウェア

- M5Stack Tab5

## 📚 必要なライブラリ

- **M5Unified**: M5Stack統合ライブラリ
  ```bash
  arduino-cli lib install "M5Unified@0.2.10"
  ```
- **WiFi**: ESP32標準ライブラリ（Arduino IDEに含まれています）

## 🚀 セットアップ

1. **ボード設定**
   - Board: ESP32P4 Dev Module
   - USB CDC on boot: Enabled
   - Flash Size: 16MB (128Mb)
   - Partition Scheme: Custom
   - PSRAM: Enabled

2. **WiFi設定**
   - `secrets.h.example`をコピーして`secrets.h`を作成してください：
   ```bash
   cp secrets.h.example secrets.h
   ```
   - `secrets.h`ファイル内の`ssid`と`password`を実際の値に編集してください：
   ```cpp
   const char *ssid = "YOUR_WIFI_SSID";
   const char *password = "YOUR_WIFI_PASSWORD";
   ```
   - **重要**: `secrets.h`は`.gitignore`に追加されているため、Gitにコミットされません。認証情報が漏れる心配はありません。

3. **プログラムのアップロード**
   - Arduino IDEまたはArduino CLIでプログラムをアップロード

## 💻 使用方法

1. WiFiのSSIDとパスワードを設定
2. プログラムをアップロード
3. デバイスがWiFiネットワークに接続すると、画面に接続情報が表示されます

## 📊 機能

- **STAモード**: WiFiネットワークに接続（デフォルト）
- **APモード**: アクセスポイントとして動作（コメントアウト済み）
- **接続状態表示**: 画面に接続状態とIPアドレスを表示
- **SDIO2ピン設定**: Tab5専用のSDIO2ピン設定に対応

## 🔍 プログラムの動作

1. **初期化**
   - M5Stackの初期化
   - WiFiのSDIO2ピンを設定（Tab5専用）
   - WiFiモードをSTAに設定

2. **WiFi接続**
   - 指定されたSSIDとパスワードで接続を試行
   - 接続中は画面に「.」を表示
   - 接続成功後、IPアドレスを表示

## 📝 コードの主要なポイント

### SDIO2ピンの設定
```cpp
WiFi.setPins(SDIO2_CLK, SDIO2_CMD, SDIO2_D0, SDIO2_D1, SDIO2_D2, SDIO2_D3, SDIO2_RST);
```

### WiFiモードの設定
```cpp
WiFi.mode(WIFI_STA);  // ステーションモード
WiFi.begin(ssid, password);  // 接続開始
```

### 接続待機
```cpp
while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Display.print(".");
}
```

### APモードの使用（オプション）
APモードを使用する場合は、STAモードのコードをコメントアウトし、APモードのコードのコメントを外してください。

## 🔧 トラブルシューティング

### 接続できない場合
- SSIDとパスワードが正しいか確認してください
- WiFiネットワークが2.4GHz帯であることを確認してください（5GHzは対応していません）
- シリアルモニターでエラーメッセージを確認してください

### IPアドレスが表示されない場合
- WiFi接続が完了するまで待機してください
- シリアルモニターで接続状態を確認してください

## 📄 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2025 macole

詳細はプロジェクトルートの`LICENSE`ファイルを参照してください。

---

**作成日**: 2025年11月  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み

