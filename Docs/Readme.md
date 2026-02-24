# M5Stack Tab5 GUI プロジェクト

このリポジトリは、M5Stack Tab5 向けの **GUI・LVGL アプリケーション** をまとめたものです。  
Arduino スケッチ単体のサンプルは `Tab5_Arduino`、ネットワーク / Web / MQTT / Camera などのサンプルは `Tab5_Network` を参照してください。

---

## 📋 プロジェクト概要

GUI を用いた以下のようなアプリケーションを提供しています。

- LVGL を使用したタッチ対応の高機能 GUI（ボタン・スライダー・リストなど）
- EEZ Studio でデザインした UI の実装例
- **EEZ 01 - Canvas お絵かき**（EEZ_01）: Canvas 指描き ＋ Button1 でクリア
- **EEZ 02 - Chart デモ**（EEZ_02）: Chart デモグラフ ＋ slider1 で画面明るさ制御（0～255、初期値 255）
- **EEZ 03 - 縦表示・画像対応**（EEZ_03）: 縦表示、GUI/PNG 色合わせ、EEZ Flow フック、点滅防止
- Native変数の実装例
- WiFi連携によるNTP時刻同期アプリケーション

---

## 📁 リポジトリ構造

実ディレクトリ構成は以下の通りです。

```
Tab5_GUI/
├── EEZ_Template/                # EEZ テンプレート（開発用ベース）
│   ├── EEZ_Template.ino         # メインプログラム
│   ├── LV8wF_Template.eez-project # EEZ Studio プロジェクト
│   ├── lv_conf.h                # LVGL 設定ファイル
│   ├── LICENSE                  # ライセンスファイル
│   ├── README.md                # プロジェクト概要
│   └── src/
│       └── ui/                  # EEZ Studio 生成ファイル
│           ├── ui.c, ui.h       # UI初期化
│           ├── screens.c, screens.h # スクリーン定義
│           ├── vars.h           # Flow 変数定義
│           ├── eez-flow.cpp, eez-flow.h # Flow エンジン
│           ├── actions.h        # アクション定義
│           ├── images.c, images.h # 画像データ
│           ├── styles.c, styles.h # スタイル定義
│           ├── fonts.h, structs.h # その他の定義ファイル
│
├── EEZ_01/                      # EEZ 01 - Canvas お絵かき
│   ├── EEZ_01.ino               # メインプログラム（Canvas描画・Button1クリア）
│   ├── EEZ_01.eez-project       # EEZ Studio プロジェクト
│   ├── lv_conf.h                # LVGL 設定
│   ├── README.md                # プロジェクト概要・Template変更点
│   └── src/ui/                  # EEZ Studio 生成ファイル
│
├── EEZ_02/                      # EEZ 02 - Chart デモ
│   ├── EEZ_02.ino               # メインプログラム（chart1デモ・slider1明るさ）
│   ├── EEZ_02.eez-project       # EEZ Studio プロジェクト
│   ├── lv_conf.h                # LVGL 設定
│   ├── README.md                # プロジェクト概要・Template変更点
│   └── src/ui/                  # EEZ Studio 生成ファイル
│
├── EEZ_03/                      # EEZ 03 - 縦表示・画像対応
│   ├── EEZ_03.ino               # メインプログラム（縦表示・色・Flowフック・pushImage）
│   ├── EEZ_03.eez-project       # EEZ Studio プロジェクト
│   ├── lv_conf.h                # LVGL 設定（LV_COLOR_16_SWAP 0）
│   ├── README.md                # 本ドキュメント（縦表示・色・点滅・Flow）
│   └── src/ui/                  # EEZ Studio 生成ファイル
│
├── LV8wF_Clock/                 # LV8wF 時計（WiFi/NTP）
│   ├── LV8wF_Clock.ino          # メインプログラム
│   ├── LV8wF_Clock.eez-project # EEZ Studio プロジェクト
│   ├── lv_conf.h                # LVGL 設定ファイル
│   ├── README.md                # プロジェクト概要
│   ├── CHANGES.md               # 変更履歴
│   └── src/
│       └── ui/                  # EEZ Studio 生成ファイル
│           ├── ui.c, ui.h
│           ├── screens.c, screens.h
│           ├── eez-flow.cpp, eez-flow.h
│           ├── actions.h
│           ├── images.c, images.h
│           ├── styles.c, styles.h
│           ├── fonts.h, structs.h
│           └── vars.h
│
├── LV8wF_Native/                # LV8wF Native 変数
│   ├── LV8wF_Native.ino         # メインプログラム
│   ├── LV8wF_Native.eez-project # EEZ Studio プロジェクト
│   ├── lv_conf.h                # LVGL 設定ファイル
│   ├── LICENSE                  # ライセンスファイル
│   ├── README.md                # プロジェクト概要
│   └── src/
│       └── ui/                  # EEZ Studio 生成ファイル
│           ├── ui.c, ui.h
│           ├── screens.c, screens.h
│           ├── eez-flow.cpp, eez-flow.h
│           ├── actions.h
│           ├── images.c, images.h
│           ├── styles.c, styles.h
│           ├── fonts.h, structs.h
│           └── vars.h           # Native変数定義
│
├── Docs/                        # 共通ドキュメント
│   ├── Readme.md                # このファイル（プロジェクト概要）
│   ├── GUIDE.md                 # 完全開発ガイド（クイックスタート、概要、カスタマイズ、技術詳細）
│   └── TROUBLESHOOTING.md       # トラブルシューティングガイド
│
├── LICENSE
└── Readme.md                    # ルートREADME（このファイルへのリンク）
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

### ボード設定

Arduino IDE または Arduino CLI で以下の設定を使用してください：

- **Board**: ESP32P4 Dev Module
- **USB CDC on boot**: Enabled
- **Flash Size**: 16MB (128Mb)
- **Partition Scheme**: Custom（各プロジェクト付属の`partitions.csv`を使用）
- **PSRAM**: Enabled
- **Upload Mode**: UART / Hardware CDC
- **USB Mode**: Hardware CDC and JTAG

---

## 📊 プロジェクト一覧（全6個）

### 開発テンプレート（1個）
| No. | 表示名 | フォルダ名 | 開発ツール | 主要機能 |
|-----|--------|------------|-----------|----------|
| 1 | EEZ テンプレート | EEZ_Template | EEZ Studio | 開発用ベース（シンプル構造、カスタマイズ容易） |

### LVGL GUIアプリケーション（5個）
| No. | 表示名 | フォルダ名 | 開発ツール | 主要機能 |
|-----|--------|------------|-----------|----------|
| 2 | EEZ 01 - Canvas お絵かき | EEZ_01 | EEZ Studio | Canvas 指描き、Button1 でクリア、滑らか描画 |
| 3 | EEZ 02 - Chart デモ | EEZ_02 | EEZ Studio | Chart デモグラフ（2系列）、slider1 で画面明るさ 0～255（初期値 255） |
| 4 | EEZ 03 - 縦表示・画像対応 | EEZ_03 | EEZ Studio | 縦表示、GUI/PNG 色合わせ、EEZ Flow フック、pushImage で点滅防止 |
| 5 | LV8wF 時計 | LV8wF_Clock | EEZ Studio | WiFi/NTP時計（WiFi接続、NTP時刻同期、リアルタイム時計表示） |
| 6 | LV8wF Native 変数 | LV8wF_Native | EEZ Studio | Native変数サンプル（counter、flag、light変数、自動更新機能） |

---

## 🎨 サンプルアプリケーション

### EEZ テンプレート（EEZ_Template）

**開発用ベース**

- LVGL + EEZ Studio Flowの統合テンプレート
- シンプルで拡張しやすい最小限の実装
- 詳細なコメントとドキュメント
- カスタマイズしやすい構造
- 機能追加のための実装ガイド付き

**主な特徴**:
- 完全ドキュメント化（詳細ガイド付き）
- M5Unified統合とSPIRAM最適化
- DMA転送による高速描画
- Flow言語の基本実装
- シンプルで理解しやすいコード構造
- スムーズなタッチ操作対応
- WiFi、センサー、通信機能の追加ガイド完備
- MIT ライセンス

**用途**:
- 新規アプリケーション開発の出発点
- EEZ Studioの学習
- LVGL + M5Unifiedの統合パターン
- IoTダッシュボードのベース
- Webサービス連携アプリのテンプレート

詳細は [`../EEZ_Template/README.md`](../EEZ_Template/README.md) を参照してください。

### EEZ 01 - Canvas お絵かき（EEZ_01）

**Canvas 指描きアプリケーション**

- EEZ テンプレートをベースに Canvas 指描きと Button1 クリアを追加
- Canvas1 上で指で線を描画（黒・太さ 4px、滑らか描画）
- Button1 を押すと Canvas1 を白でクリア（EEZ アクション `action_clear_canvas1`）
- LVGL の Image 系はデフォルトでクリック無効のため、Canvas に `LV_OBJ_FLAG_CLICKABLE` を付与
- 更新間隔・タッチ読み取り間隔を短くして線を滑らかに表示

**主な機能**:
- Canvas 指描き（SPIRAM バッファ、991×676）
- Button1 でクリア
- 滑らか描画のための LVGL 設定（ループ遅延・入力読み取り間隔）

詳細は [`../EEZ_01/README.md`](../EEZ_01/README.md) を参照してください。

### EEZ 02 - Chart デモ（EEZ_02）

**Chart デモ ＋ 画面明るさ制御アプリケーション**

- EEZ テンプレートをベースに chart1 デモグラフと slider1 で画面明るさ制御を追加
- chart1: 起動時に赤・青 2 系列の折れ線グラフ（各 24 点）を表示
- slider1: 値（0～255）で画面の明るさを変更。初期値 255。`M5.Display.setBrightness()` で反映

**主な機能**:
- chart1 デモグラフ（Y 軸 0～100、分割線 横 5・縦 8）
- slider1 で画面明るさ制御（0～255、初期値 255）

詳細は [`../EEZ_02/README.md`](../EEZ_02/README.md) を参照してください。

### EEZ 03 - 縦表示・画像対応（EEZ_03）

**縦表示・色補正・EEZ Flow 安定化**

- 縦向き 720×1280 表示（LVGL rotated=0、M5.Display.setRotation(3)）
- GUI と取り込み PNG の両方が正しい色で表示されるよう、LV_COLOR_16_SWAP 0 とフラッシュ時のバイトスワップを実施
- 画面点滅防止のため pushImage（同期転送）を使用
- EEZ Flow の stopScript / replacePage / scpiComponentInit フックをオーバーライドし、assert によるリブートを防止

詳細は [`../EEZ_03/README.md`](../EEZ_03/README.md) を参照してください。

### LV8wF 時計（LV8wF_Clock）

**WiFi/NTP時計アプリケーション**

- EEZ Studioで設計したGUIとFlow機能を使用
- WiFi接続とNTP時刻同期機能
- リアルタイム時計表示（時刻、日付、曜日）
- WiFi状態とIPアドレスの表示

**主な機能**:
- WiFi自動接続（30秒タイムアウト）
- NTP時刻同期（JST、UTC+9）
- リアルタイム時計表示（0.5秒ごと更新）
- text_areaウィジェットの動的検索
- WiFi接続状態の表示
- IPアドレス表示

**技術スタック**:
- LVGL 8.3.11 + EEZ Studio
- M5Unified 0.2.10
- WiFi（ESP32標準）
- NTP（time.h）

詳細は [`../LV8wF_Clock/README.md`](../LV8wF_Clock/README.md) を参照してください。

### LV8wF Native 変数（LV8wF_Native）

**Native変数実装サンプルアプリケーション**

- EEZ StudioとFlow機能を使用したNative変数の実装例
- C++コードから直接制御可能な変数
- 自動更新機能の実装例

**主な機能**:
- Native変数の実装（`counter`、`flag`、`light`）
- 画面明るさ制御（`light`変数で0-255の範囲で制御）
- 自動アーク更新（`flag`が`true`の時に`arc1`を0-100まで自動更新）
- カウンター表示（`counter`の値を`text_area`に自動表示）
- スムーズなタッチ操作対応

**技術スタック**:
- LVGL 8.3.11 + EEZ Studio
- M5Unified 0.2.10
- Native変数のgetter/setter実装パターン

詳細は [`../LV8wF_Native/README.md`](../LV8wF_Native/README.md) を参照してください。

---

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
- [EEZ Studio 公式サイト](https://www.envox.eu/eez-studio/)
- [EEZ Studio GitHub](https://github.com/eez-open/studio)
- [M5Stack Tab5 公式ドキュメント](https://docs.m5stack.com/)

### サンプル別参考リンク

#### EEZ テンプレート、EEZ 01 - Canvas お絵かき、EEZ 02 - Chart デモ、EEZ 03 - 縦表示・画像対応、LV8wF 時計、LV8wF Native 変数

- **開発**: オリジナル実装（2026年1月）
- **EEZ Studio 公式サイト**: [EEZ Studio](https://www.envox.eu/eez-studio/)
- **EEZ Studio ドキュメント**: [EEZ Studio Wiki](https://github.com/eez-open/studio/wiki)
- **EEZ Studio GitHub**: [EEZ Studio](https://github.com/eez-open/studio)
- **LVGL公式ドキュメント**: [LVGL Documentation](https://docs.lvgl.io/)
- **LVGL チュートリアル**: [LVGL Getting Started](https://docs.lvgl.io/master/get-started/index.html)
- **M5Stack公式ドキュメント**: [M5Stack Tab5 Documentation](https://docs.m5stack.com/)

EEZ Studio と Flow 機能を使用したアプリケーション開発のテンプレートとリファレンス実装です。EEZ 01 - Canvas お絵かき、EEZ 02 - Chart デモ、EEZ 03 - 縦表示・画像対応、LV8wF 時計（WiFi/NTP）、LV8wF Native 変数など、実用的な機能の実装例を提供しています。

---

## 📖 詳細ドキュメント

- **[開発ガイド](GUIDE.md)** - 完全開発ガイド
  - クイックスタート（環境セットアップから動作確認まで）
  - プロジェクト概要（アーキテクチャ、ファイル構造、データフロー）
  - カスタマイズガイド（UI変更、機能追加、WiFi実装、Native変数）
  - 技術詳細（LVGL統合、EEZ Flow統合、描画パイプライン、パフォーマンス最適化）

- **[トラブルシューティング](TROUBLESHOOTING.md)** - よくある問題と解決方法
  - 画面が表示されない
  - コンパイルエラー
  - メモリ不足
  - WiFi/NTP接続の問題
  - タッチパネルの問題
  - 段階的デバッグ手順

