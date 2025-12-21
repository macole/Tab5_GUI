# M5Stack Tab5 GUI プロジェクト

このリポジトリは、M5Stack Tab5 向けの **GUI・LVGL アプリケーション** をまとめたものです。  
Arduino スケッチ単体のサンプルは `Tab5_Arduino`、ネットワーク / Web / MQTT / Camera などのサンプルは `Tab5_Network` を参照してください。

---

## 📋 プロジェクト概要

- **プロジェクト名**: M5Stack Tab5 GUI / LVGL サンプル集  
- **対象デバイス**: M5Stack Tab5 (ESP32-P4)  
- **主な技術**: LVGL, M5Unified / M5GFX, Arduino IDE  
- **総プロジェクト数**: 2個

GUI を用いた以下のようなアプリケーションを提供しています。

- LVGL を使用したタッチ対応の高機能 GUI（ボタン・スライダー・リストなど）
- Square Line Studio でデザインした UI の実装例
- 画像・フォントを多用したフロントエンド UI
- RTC 連携による時計アプリケーション

---

## 📁 リポジトリ構造

実ディレクトリ構成は以下の通りです。

```
Tab5_GUI/
├── tab5_arduino_basic/          # LVGL ベーシックアプリケーション
│   ├── tab5_arduino_basic.ino   # メインプログラム
│   ├── ui.c, ui.h               # Square Line Studio 生成 UI コード
│   ├── ui_Screen1.c             # 画面1の実装
│   ├── ui_helpers.c/h           # UI ヘルパー関数
│   ├── ui_events.h               # イベント定義
│   ├── ui_comp_hook.c           # コンポーネントフック
│   ├── ui_font_*.c              # フォントデータ
│   ├── lv_conf.h                # LVGL 設定ファイル
│   ├── CMakeLists.txt           # CMake 設定
│   ├── partitions.csv           # パーティション設定
│   ├── SLS_Project/             # Square Line Studio プロジェクト
│   │   ├── Tab5_Basic.spj
│   │   ├── Tab5_Basic.sll
│   │   └── Themes.slt
│   └── README.md                # 詳細なドキュメント
│
├── tab5_flip_clock/             # パタパタ時計 / ニキシー管時計アプリ
│   ├── tab5_flip_clock.ino      # メインプログラム
│   ├── img_flip.h               # パタパタ時計用画像データ
│   ├── img_nixie_tube.h         # ニキシー管時計用画像データ
│   └── Readme.md                # 詳細なドキュメント
│
├── LICENSE
└── Readme.md                    # このファイル
```

---

## 🚀 セットアップ

### 必要な環境

- **Arduino IDE 2.x** または **Arduino CLI**
- **ESP32 Arduino Core >= 3.2** (M5Stack Tab5 用ボードパッケージ)
- **M5Stack Tab5** (ESP32-P4)

### ライブラリのインストール

Arduino IDE のライブラリマネージャーから、または Arduino CLI で以下のライブラリをインストールしてください。

```bash
arduino-cli lib install "M5Unified@0.2.10"
arduino-cli lib install "lvgl@8.3.11"
```

**注意**: M5Unified は M5GFX を含む統一ライブラリです。M5GFX を個別にインストールする必要はありません。

### 追加のライブラリ（tab5_flip_clock 用）

```bash
arduino-cli lib install "ArtronShop_RX8130CE"
```

### ボード設定

Arduino IDE または Arduino CLI で以下の設定を使用してください：

- **Board**: ESP32P4 Dev Module
- **USB CDC on boot**: Enabled
- **Flash Size**: 16MB (128Mb)
- **Partition Scheme**: Custom（各プロジェクト付属の`partitions.csv`を使用）
- **PSRAM**: Enabled
- **Upload Mode**: UART / Hardware CDC
- **USB Mode**: Hardware CDC and JTAG

Arduino CLI での設定例：

```bash
arduino-cli compile \
  --fqbn esp32:esp32:esp32p4:PSRAM=enabled,FlashSize=16M,PartitionScheme=custom,CDCOnBoot=cdc,USBMode=hwcdc,UploadSpeed=921600 \
  tab5_arduino_basic/tab5_arduino_basic.ino

arduino-cli upload -p /dev/cu.usbmodem21201 \
  --fqbn esp32:esp32:esp32p4:PSRAM=enabled,FlashSize=16M,PartitionScheme=custom,CDCOnBoot=cdc,USBMode=hwcdc,UploadSpeed=921600 \
  tab5_arduino_basic/tab5_arduino_basic.ino
```

**注意**: PSRAM=enabled と PartitionScheme=custom は必須です。これらの設定を省くと、バックライトやフレームバッファ初期化に失敗し、画面が真っ暗になる場合があります。

---

## 📊 プロジェクト一覧（全2個）

### LVGL GUIアプリケーション（2個）
| No. | プログラム名 | 状態 | 主要機能 |
|-----|-------------|------|----------|
| 1 | tab5_arduino_basic | ✅ 完了 | LVGL基本GUI（ボタン、スライダー、アーク） |
| 2 | tab5_flip_clock | ✅ 完了 | パタパタ時計/ニキシー管時計（RTC連携） |

---

## 🎨 サンプルアプリケーション

### tab5_arduino_basic

**LVGL を使用したベーシックな GUI アプリケーション**

- Square Line Studio でデザインした UI の実装例
- ボタンによる自動カウント機能の ON/OFF
- アーク（円形プログレスバー）とラベルによるカウンター表示
- スライダーによる画面の明るさ調整
- M5Unified を使用したシンプルな実装

**主な機能**:
- LVGL と M5Unified の統合
- タッチパネル入力のサポート
- DMA 転送による高速描画
- SPIRAM を使用した描画バッファの確保
- 90度回転ディスプレイのサポート

詳細は [`tab5_arduino_basic/README.md`](tab5_arduino_basic/README.md) を参照してください。

### tab5_flip_clock

**パタパタ時計 / ニキシー管時計アプリケーション**

- RTC（RX8130CE）から正確な時刻を取得
- 2つの表示モードを切り替え可能
  - **パタパタ時計モード**: 数字がフリップするアニメーション効果付き
  - **ニキシー管時計モード**: ニキシー管の温かみのあるオレンジ色の表示
- 日付表示（年/月/日と曜日）
- 時刻表示（時:分:秒）
- 最適化された描画（変更があった数字のみ更新）

**主な機能**:
- RTC（RX8130CE）からの時刻取得
- UTC から日本時間（JST）への自動変換
- 画面サイズに基づく自動スケーリング
- 滑らかな画像拡大（黒い筋なし）
- コロンの点滅表示（1秒ごと）

詳細は [`tab5_flip_clock/Readme.md`](tab5_flip_clock/Readme.md) を参照してください。

---

## 📊 プロジェクト統計

- **総プログラム数**: 2個
- **ドキュメント数**: 4個（README.md + 各プロジェクトReadme.md + 参考元リポジトリ情報）
- **動作確認率**: 100%
- **カテゴリ数**: 1カテゴリ（LVGL GUIアプリケーション）

### 技術スタック
- **GUI フレームワーク**: LVGL 8.3.11
- **ハードウェアライブラリ**: M5Unified 0.2.10
- **開発環境**: Arduino IDE 2.x / Arduino CLI
- **デザインツール**: Square Line Studio（tab5_arduino_basic用）

### 主な機能
- **タッチパネル対応**: マルチタッチ、ジェスチャー認識
- **高速描画**: DMA転送、SPIRAM使用
- **RTC連携**: 正確な時刻表示（tab5_flip_clock）
- **画像表示**: PNG/JPEG対応、スムーズスケーリング
- **アニメーション**: フリップ効果、点滅表示

### 🎓 教育価値

このプロジェクトは、M5Stack Tab5でのLVGL GUI開発の実践的なリファレンスとして、以下の用途で活用できます：

1. **教育・学習用途**
   - LVGL GUI開発の基礎学習
   - Square Line Studioの実践的な使用例
   - タッチパネルUI開発の実装パターン
   - RTC連携アプリケーションの実装方法

2. **研究・開発用途**
   - GUI アプリケーション開発の基盤
   - 時計・カレンダーアプリの参考実装
   - LVGL 8.3とM5Unifiedの統合パターン

3. **商用用途**
   - 製品UI開発の参考実装
   - カスタマイズ可能なGUIテンプレート
   - Square Line Studioワークフローの確立

---

## 🔧 トラブルシューティング

### よくある問題と解決策

#### 1. **画面が真っ暗**
**原因**: PSRAM設定またはパーティション設定が不適切  
**解決策**:
- ボード設定で`PSRAM: Enabled`を確認
- `PartitionScheme: Custom`を選択
- 各プロジェクトの`partitions.csv`を使用

#### 2. **LVGLコンパイルエラー**
**原因**: LVGLバージョンの不一致  
**解決策**:
```bash
# LVGL 9.xをアンインストール
arduino-cli lib uninstall lvgl

# LVGL 8.3.11をインストール
arduino-cli lib install "lvgl@8.3.11"
```

#### 3. **タッチが反応しない**
**原因**: タッチドライバの初期化失敗  
**解決策**:
- `lv_conf.h`のタッチ設定を確認
- `M5.Touch.isEnabled()`でタッチ検出を確認
- プログラムを再起動

#### 4. **色がおかしい**
**原因**: 色深度設定の不一致  
**解決策**:
- `lv_conf.h`で`LV_COLOR_DEPTH 16`を確認
- `LV_COLOR_16_SWAP 1`を設定
- M5GFXの`setSwapBytes(true)`を確認

#### 5. **画像が表示されない（tab5_flip_clock）**
**原因**: 画像データのサイズが大きすぎる  
**解決策**:
- PSRAM設定を確認
- 画像データが正しくインクルードされているか確認

#### 6. **RTC時刻がずれる（tab5_flip_clock）**
**原因**: RTCの初期化失敗またはバッテリー切れ  
**解決策**:
- RTC（RX8130CE）のI2C接続を確認
- RTCバッテリーを確認
- プログラムで時刻を再設定

### デバッグ方法
```bash
# シリアルモニターでデバッグ情報を確認
arduino-cli monitor -p /dev/cu.usbmodem21201
```

---

## 📖 関連リポジトリ

- **Arduino 単体サンプル**: `Tab5_Arduino`
- **ネットワーク / Web / MQTT / Camera など**: `Tab5_Network`

---

## 📝 ライセンス

このプロジェクトは MIT ライセンスで公開されています。  
詳細は `LICENSE` ファイルを参照してください。

外部ライブラリ（LVGL, M5Unified, M5GFX, LovyanGFX など）は、それぞれの公式リポジトリのライセンスに従います。

---

## 🔗 参考リンク集

このプロジェクトのサンプルは、以下の資料やWebサイトを参考に作成されています。

### 公式ドキュメント・ツール
- [LVGL 公式ドキュメント](https://docs.lvgl.io/)
- [LVGL GitHub](https://github.com/lvgl/lvgl)
- [M5Unified GitHub](https://github.com/m5stack/M5Unified)
- [M5GFX GitHub](https://github.com/m5stack/M5GFX)
- [Square Line Studio](https://squareline.io/)
- [M5Stack Tab5 公式ドキュメント](https://docs.m5stack.com/)

### サンプル別参考リンク

#### tab5_flip_clock

- **元リポジトリ**: [gijin77/M5Stack-Tab5-Nixie-Flip-Clock](https://github.com/gijin77/M5Stack-Tab5-Nixie-Flip-Clock)
- **オリジナルコード**: 2025/05/23 By JK1VCK
- **ブログ記事**: [gijin77のブログ - M5Stack Tab5でニキシー管時計](https://gijin77.blog.jp/archives/44667829.html)
- **RTC ライブラリ**: [ArtronShop_RX8130CE GitHub](https://github.com/artronshop/ArtronShop_RX8130CE)

パタパタ時計 / ニキシー管時計アプリケーションは、上記のリポジトリを参考に実装されています。

#### tab5_arduino_basic

- **元リポジトリ**: [nikthefix/M5Stack_Tab5_Arduino_Basic_LVGL_Demo](https://github.com/nikthefix/M5Stack_Tab5_Arduino_Basic_LVGL_Demo)
- **LVGL公式ドキュメント**: [LVGL Documentation](https://docs.lvgl.io/)
- **LVGL チュートリアル**: [LVGL Getting Started](https://docs.lvgl.io/master/get-started/index.html)
- **M5Stack公式ドキュメント**: [M5Stack Tab5 Documentation](https://docs.m5stack.com/)
- **Square Line Studio**: [Square Line Studio](https://squareline.io/)
- **Square Line Studio ドキュメント**: [SLS Documentation](https://docs.squareline.io/)

LVGLとM5Unifiedを使用したベーシックなGUIアプリケーションは、上記のリポジトリと公式ドキュメントを参考に実装されています。

---

## 📚 参考資料

上記の「参考リンク集」を参照してください。

LVGLとM5Unifiedを使用したベーシックなGUIアプリケーションは、上記のリポジトリと公式ドキュメントを参考に実装されています。

---

**作成日**: 2025年12月6日  
**最終更新**: 2025年12月21日（プロジェクト一覧表を追加、プロジェクト統計を追加）  
**対象デバイス**: M5Stack Tab5 (ESP32-P4)  
**開発環境**: Arduino IDE 2.x / Arduino CLI  
**動作確認**: ✅ 全2プロジェクト正常動作確認済み

