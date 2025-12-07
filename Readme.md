# M5Stack Tab5 GUI プロジェクト（LVGL / Drawing Camera）

このリポジトリは、M5Stack Tab5 向けの **GUI・LVGL アプリケーション** をまとめたものです。  
Arduino スケッチ単体のサンプルは `Tab5_Arduino`、ネットワーク / Web / MQTT / Camera などのサンプルは `Tab5_Network` を参照してください。

---

## 📋 プロジェクト概要

- **プロジェクト名**: M5Stack Tab5 GUI / LVGL サンプル集  
- **対象デバイス**: M5Stack Tab5 (ESP32-P4)  
- **主な技術**: LVGL, M5GFX / LovyanGFX, C++ アプリケーションフレームワーク  

GUI を用いた以下のようなパターンをカバーしています。

- タッチ対応の高機能 GUI（ボタン・スライダー・リストなど）
- 画像・フォントを多用したフロントエンド UI
- カメラ映像との連携（DrawingCameraApp）
- デスクトップ上でのエミュレーション実行（`platforms/desktop`）

---

## 📁 リポジトリ構造

実ディレクトリ構成（簡略版）は以下の通りです。

```
Tab5_GUI/
├── DrawingCameraApp/            # カメラ連携 GUI アプリ（CMake プロジェクト）
│   ├── app/
│   │   ├── app.cpp, app.h      # アプリケーションエントリ
│   │   ├── apps/               # 個別アプリ（ツール群）
│   │   ├── assets/             # 画像・フォントなどのアセット
│   │   │   ├── assets.h
│   │   │   └── images/*.c
│   │   ├── hal/                # ハードウェア抽象化レイヤー
│   │   │   ├── hal.cpp
│   │   │   └── hal.h
│   │   └── shared/             # 共通ユーティリティ
│   ├── platforms/
│   │   ├── desktop/            # デスクトップ用ビルド設定
│   │   └── tab5/               # Tab5 実機用ビルド設定（ESP-IDF）
│   ├── lv_conf.h               # LVGL 設定
│   ├── CMakeLists.txt
│   └── README.md               # DrawingCameraApp 個別 README
│
├── DroomMachine/                # Arduino 用 GUI デモ（フォント多数）
│   ├── DroomMachine.ino
│   ├── smallFont.h, tinyFont.h
│   ├── midleFont.h, bigFont.h
│   ├── NotoSansBold15.h
│   └── Readme.md
│
├── Lvgl/                        # Arduino + LVGL の基本デモ
│   ├── lvgl.ino                 # Tab5 上で動作する LVGL デモ
│   ├── CMakeLists.txt
│   ├── lv_conf.h                # LVGL 設定
│   ├── ui.c, ui.h               # 自動生成された UI コード
│   ├── ui_*.c                   # フォント・コンポーネント
│   ├── LVGLEditorPro/           # GUI エディタ用プロジェクト (xml)
│   └── SLS_Project/             # Square Line Studio プロジェクト
│
├── tab5_lvgl/                   # カスタム LVGL GUI サンプル一式
│   ├── tab5_lvgl.ino
│   ├── lv_conf.h
│   ├── ui.c, ui.h, ui_*.c
│   ├── SLS_Project/             # Square Line Studio プロジェクト（拡張版）
│   └── README.md
│
├── Docs/                        # 共通ドキュメント・スクリプト
│   ├── Arduino_Basic_Syntax_Guide.md
│   ├── GPIO_Application_Guide.md
│   ├── Manufacturing_Arduino_Samples.md
│   ├── USB_Application_Guide.md
│   ├── compile.sh               # Arduino 用コンパイルスクリプト
│   └── monitor.sh               # シリアルモニタスクリプト
│
├── LICENSE
└── Readme.md                    # このファイル
```

※ 旧 `WIFI/` ディレクトリの Wi-Fi GUI サンプルは、ネットワーク系リポジトリ `Tab5_Network` 側のサンプルと役割分担する形で整理しています。

---

## 🚀 セットアップ（概要）

### 共通

- VS Code / Cursor + CMake / Ninja（`DrawingCameraApp` デスクトップビルド用）
- Arduino IDE / Arduino CLI（`DroomMachine`, `Lvgl`, `tab5_lvgl` など Arduino スケッチ用）
- ESP32-P4 ボードパッケージ（M5Stack Tab5 用）

LVGL や M5 系ライブラリは `Tab5_Arduino` と共通です。

```bash
arduino-cli lib install "M5Unified@0.2.10"
arduino-cli lib install "M5GFX@0.2.15"
arduino-cli lib install "LovyanGFX@1.2.7"
arduino-cli lib install "lvgl@8.3.11"
```

---

## 🎨 代表的なサンプル

- **DroomMachine**
  - 複数フォントを使ったテキスト表示デモ
  - シンプルな GUI 描画ロジックの参考になります

- **Lvgl/lvgl.ino**
  - ボタン・スライダー・ラベルなど、基本的な LVGL ウィジェットを使用したデモ
  - `lv_conf.h` と `ui.c / ui.h` を含む、Tab5 向け LVGL プロジェクトの最小構成例

- **tab5_lvgl**
  - Square Line Studio で設計した画面を Tab5 向けに展開した拡張 GUI デモ
  - 複数画面・画像アセット・カスタムフォントをまとめて扱うサンプル

- **DrawingCameraApp**
  - CMake + ESP-IDF ベースの GUI アプリケーション
  - `platforms/desktop` で PC 上での動作確認が可能
  - `platforms/tab5` で Tab5 実機向けビルドに対応

---

## 📖 関連リポジトリ

- Arduino 単体サンプル: `Tab5_Arduino`
- ネットワーク / Web / MQTT / Camera など: `Tab5_Network`

---

## 📝 ライセンス

このプロジェクトは MIT ライセンスで公開されています。  
詳細は `LICENSE` ファイルを参照してください。

外部ライブラリ（LVGL, M5Unified, M5GFX, LovyanGFX など）は、それぞれの公式リポジトリのライセンスに従います。

---

**最終更新日**: 2025年12月7日  
**対象デバイス**: M5Stack Tab5 (ESP32-P4)  
**開発環境**: Arduino IDE / Arduino CLI + CMake / ESP-IDF

