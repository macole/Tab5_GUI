# M5Stack Tab5 GUI プロジェクト

このリポジトリは、M5Stack Tab5 向けの **GUI・LVGL アプリケーション** をまとめたものです。  
Arduino スケッチ単体のサンプルは `Tab5_Arduino`、ネットワーク / Web / MQTT / Camera などのサンプルは `Tab5_Network` を参照してください。

---

## 📋 プロジェクト概要

- **プロジェクト名**: M5Stack Tab5 GUI / LVGL サンプル集  
- **対象デバイス**: M5Stack Tab5 (ESP32-P4)  
- **主な技術**: LVGL, M5Unified / M5GFX, Arduino IDE  

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

## 📖 関連リポジトリ

- **Arduino 単体サンプル**: `Tab5_Arduino`
- **ネットワーク / Web / MQTT / Camera など**: `Tab5_Network`

---

## 📝 ライセンス

このプロジェクトは MIT ライセンスで公開されています。  
詳細は `LICENSE` ファイルを参照してください。

外部ライブラリ（LVGL, M5Unified, M5GFX, LovyanGFX など）は、それぞれの公式リポジトリのライセンスに従います。

---

## 🔗 参考資料

- [LVGL 公式ドキュメント](https://docs.lvgl.io/)
- [M5Unified GitHub](https://github.com/m5stack/M5Unified)
- [M5GFX GitHub](https://github.com/m5stack/M5GFX)
- [Square Line Studio](https://squareline.io/)
- [M5Stack Tab5 公式ドキュメント](https://docs.m5stack.com/)


