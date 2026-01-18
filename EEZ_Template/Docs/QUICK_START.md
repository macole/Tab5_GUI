# クイックスタートガイド

このガイドでは、EEZ Templateを使って最初のアプリケーションを動かすまでの手順を説明します。

## 📋 必要なもの

### ハードウェア
- ✅ M5Stack Tab5
- ✅ USB-Cケーブル
- ✅ PC (Windows/Mac/Linux)

### ソフトウェア
- ✅ Arduino IDE 2.x
- ✅ M5Unifiedライブラリ (>= 0.2.10)
- ✅ LVGLライブラリ (= 8.3.11)

---

## 🚀 ステップ1: 環境セットアップ

### 1-1. Arduino IDEのインストール

1. [Arduino公式サイト](https://www.arduino.cc/en/software)からダウンロード
2. インストーラーを実行
3. Arduino IDEを起動

### 1-2. ESP32ボードサポートの追加

1. **ファイル** → **環境設定**を開く
2. **追加のボードマネージャのURL**に以下を追加：
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
3. **OK**をクリック
4. **ツール** → **ボード** → **ボードマネージャ**を開く
5. **esp32**で検索
6. **esp32 by Espressif Systems**をインストール（バージョン3.2以上）

### 1-3. 必要なライブラリのインストール

**方法1: ライブラリマネージャを使用（推奨）**

1. **スケッチ** → **ライブラリをインクルード** → **ライブラリを管理**
2. **M5Unified**で検索してインストール
3. **lvgl**で検索して**バージョン8.3.11**をインストール

⚠️ **重要**: LVGL 9.xではなく、必ず8.3.11をインストールしてください！

**方法2: Arduino CLIを使用**

```bash
arduino-cli lib install "M5Unified@0.2.10"
arduino-cli lib install "lvgl@8.3.11"
```

---

## 🔧 ステップ2: プロジェクトを開く

### 2-1. プロジェクトを開く

1. Arduino IDEを起動
2. **ファイル** → **開く**
3. `EEZ_Template/EEZ_Template.ino`を選択
4. **開く**をクリック

### 2-2. ボード設定

**ツール**メニューで以下を設定：

```
ボード: "M5Tab5"
Upload Speed: 921600
Flash Mode: QIO
Flash Frequency: 80MHz
Partition Scheme: Default 4MB with spiffs
PSRAM: Enabled ← 必須！
USB CDC On Boot: Enabled
USB Mode: Hardware CDC and JTAG
```

⚠️ **PSRAM: Enabled**を忘れずに！これがないと動作しません。

### 2-3. シリアルポートの選択

1. M5Stack Tab5をUSBケーブルでPCに接続
2. **ツール** → **シリアルポート**で接続されたポートを選択
   - Mac: `/dev/cu.usbmodem*****`
   - Windows: `COM*`
   - Linux: `/dev/ttyACM*`

---

## ▶️ ステップ3: コンパイル・アップロード

### 3-1. コンパイル

1. **検証（✓）**ボタンをクリック
2. コンパイル完了を待つ（初回は数分かかる場合があります）

**コンパイル成功例**：
```
スケッチはプログラムストレージの 4% を使用
グローバル変数はRAMの 2% を使用
```

### 3-2. アップロード

1. **アップロード（→）**ボタンをクリック
2. アップロード完了を待つ

**アップロード成功例**：
```
Writing at 0x00000000... (100%)
Wrote 12345678 bytes at 0x00000000 in 12.3 seconds
Hard resetting via RTS pin...
```

### 3-3. 動作確認

Tab5の画面に以下が表示されれば成功です：
- ✅ "Hello, world!" のテキスト
- ✅ ボタン
- ✅ アーク（円形プログレスバー）
- ✅ スライダー

---

## 📊 ステップ4: シリアルモニタでログを確認

### 4-1. シリアルモニタを開く

1. **ツール** → **シリアルモニタ**
2. 右下のボーレートを**115200**に設定

### 4-2. 起動ログの確認

正常に動作している場合、以下のようなログが表示されます：

```
╔═══════════════════════════════════════╗
║  M5Stack Tab5                         ║
║  EEZ Studio Template Project          ║
╚═══════════════════════════════════════╝

📊 System Information:
   LVGL Version: 8.3.11
   Free Heap: 423456 bytes (413.53 KB)
   Free PSRAM: 8388608 bytes (8.00 MB)

🔧 Initializing LVGL display...
✅ Display buffer allocated: 1843200 bytes (1.76 MB)
✅ LVGL display initialized
🔧 Initializing LVGL touch...
✅ LVGL touch initialized

🎨 Initializing EEZ-Studio UI...
✅ EEZ-Studio UI initialized

╔═══════════════════════════════════════╗
║  Setup Completed Successfully!        ║
╚═══════════════════════════════════════╝
   Free Heap after setup: 412345 bytes (402.68 KB)
   Free PSRAM after setup: 6545408 bytes (6.24 MB)

🚀 Application started!
```

---

## 🎮 ステップ5: 基本操作を試す

### 5-1. ボタン操作

- 画面下部の**Button1**をタッチ
- カウンターが自動的に増加開始
- もう一度タッチすると停止

### 5-2. スライダー操作

- 画面右側の**スライダー**をドラッグ
- 画面の明るさがリアルタイムで変化

### 5-3. 他のボタン

- 画面下部の2つのボタンをタッチ
- シリアルモニタでイベントログを確認

---

## 🐛 トラブルシューティング

### 問題1: コンパイルエラー "lv_conf.h not found"

**原因**: lv_conf.hが見つからない

**解決策**:
1. プロジェクトフォルダに`lv_conf.h`があることを確認
2. Arduino IDEを再起動
3. プロジェクトを開き直す

### 問題2: "PSRAM allocation failed"

**原因**: PSRAMが有効になっていない

**解決策**:
```
ツール → PSRAM → Enabled
```

### 問題3: 画面が真っ暗

**チェックリスト**:
1. ✅ ボード設定が"M5Tab5"になっているか
2. ✅ PSRAM: Enabledになっているか
3. ✅ アップロードが完了しているか
4. ✅ Tab5が起動しているか（電源ボタンを押す）

### 問題4: タッチが反応しない

**解決策**:
1. Tab5を再起動
2. 画面を清掃（指紋や汚れを除去）
3. ファームウェアを再アップロード

### 問題5: シリアルモニタに何も表示されない

**チェックリスト**:
1. ✅ ボーレートが115200になっているか
2. ✅ 正しいシリアルポートを選択しているか
3. ✅ **USB CDC On Boot: Enabled**になっているか

---

## 🎯 次のステップ

基本動作が確認できたら、次は以下を試してみましょう：

### 初級

1. **メッセージを変更する**
   - "Hello, world!" を別のテキストに変更
   - カウンターの最大値を変更

### 中級

2. **WiFi機能を有効にする**
   - [WiFi設定ガイド](../README.md#wifi機能の有効化オプション)を参照
   - インターネットからデータを取得

3. **EEZ StudioでUIを編集**
   - 新しいボタンを追加
   - 色やレイアウトを変更

### 上級

4. **独自の機能を追加**
   - [カスタマイズガイド](CUSTOMIZATION_GUIDE.md)を参照
   - センサーデータの表示
   - データの保存

---

## 📖 関連ドキュメント

- [README.md](../README.md) - プロジェクト概要
- [CUSTOMIZATION_GUIDE.md](CUSTOMIZATION_GUIDE.md) - カスタマイズ方法
- [TECHNICAL_GUIDE.md](TECHNICAL_GUIDE.md) - 技術詳細

---

## 💡 ヒント

### Arduino CLIを使う場合

```bash
# コンパイル
arduino-cli compile \
  --fqbn esp32:esp32:m5stack_tab5:PSRAM=enabled \
  EEZ_Template/EEZ_Template.ino

# アップロード
arduino-cli upload -p /dev/cu.usbmodem* \
  --fqbn esp32:esp32:m5stack_tab5:PSRAM=enabled \
  EEZ_Template/EEZ_Template.ino

# シリアルモニタ
arduino-cli monitor -p /dev/cu.usbmodem* -c baudrate=115200
```

### 開発Tips

- **こまめに保存**: Ctrl+S（Win）/ Cmd+S（Mac）
- **こまめにテスト**: 小さな変更ごとにコンパイル・テスト
- **シリアルログ活用**: デバッグにはSerial.printlnを使う
- **バージョン管理**: Gitでコードを管理する

---

**おめでとうございます！🎉**

これでEEZ Templateの基本的な使い方をマスターしました。

次は[CUSTOMIZATION_GUIDE.md](CUSTOMIZATION_GUIDE.md)を読んで、独自のアプリケーションを作成してみましょう！
