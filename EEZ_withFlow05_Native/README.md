# EEZ Studio Template for M5Stack Tab5

M5Stack Tab5向けのEEZ Studio + LVGLアプリケーション開発用テンプレートプロジェクトです。

## 📋 概要

このテンプレートは、EEZ StudioとLVGL 8.3.11を使用したM5Stack Tab5アプリケーションを迅速に開発するための基礎を提供します。カスタムGUI、Flow言語による動作制御など、実用的なアプリケーション開発に必要な要素が含まれています。

### 主な特徴

- ✅ **完全ドキュメント化** - 詳細なコメントとガイド
- ✅ **M5Unified統合** - M5Stack統合ライブラリを使用
- ✅ **LVGL 8.3.11対応** - 高機能GUIライブラリ
- ✅ **EEZ Studio Flow** - 視覚的なFlow言語サポート
- ✅ **シンプル構造** - 必要最小限の実装
- ✅ **SPIRAM最適化** - 全画面バッファで高速描画
- ✅ **DMA転送** - 効率的な画面更新
- ✅ **カスタマイズ容易** - わかりやすい構造

## 🔧 ハードウェア要件

- **M5Stack Tab5** (ESP32-P4)
  - 10.1インチディスプレイ（1280x720）
  - 静電容量式タッチスクリーン
  - 8MB PSRAM（必須）

## 📚 必要なライブラリ

| ライブラリ | バージョン | 必須/オプション |
|------------|----------|----------------|
| M5Unified | >= 0.2.10 | 必須 |
| LVGL | 8.3.11 | 必須 |

### Arduino IDEでのインストール

```
スケッチ → ライブラリをインクルード → ライブラリを管理
```

1. **M5Unified**で検索してインストール（バージョン0.2.10以上）
2. **lvgl**で検索して**バージョン8.3.11**をインストール

⚠️ **重要**: LVGL 9.xは互換性がありません。必ず8.3.11を使用してください。

## 🚀 クイックスタート

### 1. プロジェクトを開く

Arduino IDEで`EEZ_Template.ino`を開きます。

### 2. ボード設定

**ツール**メニューで以下を設定：

```
ボード: M5Tab5
Upload Speed: 921600
Flash Mode: QIO
Flash Frequency: 80MHz
Partition Scheme: Default 4MB with spiffs
PSRAM: Enabled (必須!)
```

### 3. コンパイル・アップロード

1. **検証（✓）**ボタンでコンパイル
2. **アップロード（→）**ボタンでTab5に転送

### 4. 動作確認

- シリアルモニタを開く（115200 bps）
- 起動メッセージと初期化ログを確認
- 画面にGUIが表示されることを確認

## 📁 プロジェクト構造

```
EEZ_Template/
├── EEZ_Template.ino          # メインプログラム
├── LVGLv8withFlow.eez-project # EEZ Studioプロジェクトファイル
├── lv_conf.h                  # LVGL設定ファイル
├── README.md                  # このファイル
├── secrets.h.example          # WiFi設定テンプレート
├── .gitignore                 # Git除外設定
├── Docs/                      # ドキュメントフォルダ
│   ├── QUICK_START.md        # クイックスタートガイド
│   ├── CUSTOMIZATION_GUIDE.md # カスタマイズガイド
│   └── TECHNICAL_GUIDE.md    # 技術詳細
├── ui.c / ui.h                # UI初期化（EEZ Studio生成）
├── screens.c / screens.h      # 画面定義（EEZ Studio生成）
├── eez-flow.cpp / eez-flow.h  # Flow言語エンジン（EEZ Studio生成）
├── actions.h                  # アクション定義（EEZ Studio生成）
├── styles.c / styles.h        # スタイル定義（EEZ Studio生成）
├── images.c / images.h        # 画像データ（EEZ Studio生成）
├── fonts.h                    # フォント定義（EEZ Studio生成）
├── structs.h                  # 構造体定義（EEZ Studio生成）
└── vars.h                     # 変数定義（EEZ Studio生成）
```

## 🎨 UIのカスタマイズ

### EEZ Studioでの編集

1. **EEZ Studioをインストール**
   - [公式サイト](https://www.envox.eu/eez-studio/)からダウンロード
   - Windows、Mac、Linux対応

2. **プロジェクトを開く**
   ```
   File → Open Project → LVGLv8withFlow.eez-project
   ```

3. **UIを編集**
   - **Pages**: 画面レイアウトを視覚的に設計
   - **Flow**: Flow言語で動作を定義
   - **Styles**: 色、フォント、サイズをカスタマイズ
   - **Variables**: グローバル変数を定義

4. **エクスポート**
   ```
   Build → Build & Export
   ```

5. **生成ファイルを反映**
   - 生成されたファイルをプロジェクトフォルダにコピー
   - Arduino IDEで再コンパイル

詳細は[CUSTOMIZATION_GUIDE.md](Docs/CUSTOMIZATION_GUIDE.md)を参照してください。

## 🔍 デフォルト機能

このテンプレートには以下の機能が実装されています：

### 1. 自動カウント機能

- ボタンで有効/無効を切り替え
- 0〜255まで自動カウント
- カウンター表示をリアルタイム更新

### 2. 画面の明るさ調整

- スライダーで明るさを調整（0〜255）
- リアルタイムで反映

## 📊 メモリ使用量

### 典型的なメモリ使用量

```
コンパイル後:
- プログラムストレージ: 約600KB / 16MB (4%)
- 動的メモリ: 約80KB / 512KB (16%)

実行時:
- 画面バッファ (SPIRAM): 1.8MB / 8MB (23%)
- 空きヒープ: 約400KB
- 空きPSRAM: 約6MB
```

## 🐛 トラブルシューティング

### コンパイルエラー: lv_conf.h not found

**原因**: lv_conf.hが見つからない

**解決策**: プロジェクトフォルダに`lv_conf.h`が存在することを確認

### 画面が表示されない

**チェックリスト**:
1. ✅ ボード設定が"M5Tab5"
2. ✅ PSRAM: Enabled
3. ✅ ui_init()が呼ばれている
4. ✅ lv_timer_handler()がループ内にある
5. ✅ シリアルモニタでエラーを確認

### メモリ不足エラー

**原因**: SPIRAMが有効化されていない

**解決策**:
```
ツール → PSRAM → Enabled
```

### 機能を追加したい

**ガイド**:
- WiFi機能の追加方法: [CUSTOMIZATION_GUIDE.md](Docs/CUSTOMIZATION_GUIDE.md)
- センサー読み取り: [CUSTOMIZATION_GUIDE.md](Docs/CUSTOMIZATION_GUIDE.md)
- データ通信実装: [CUSTOMIZATION_GUIDE.md](Docs/CUSTOMIZATION_GUIDE.md)

## 📖 詳細ドキュメント

- [プロジェクト概要](Docs/PROJECT_OVERVIEW.md) - プロジェクト全体の詳細説明
- [クイックスタートガイド](Docs/QUICK_START.md) - 初めての方向け
- [カスタマイズガイド](Docs/CUSTOMIZATION_GUIDE.md) - アプリ開発方法
- [技術詳細](Docs/TECHNICAL_GUIDE.md) - 内部実装の解説

## 🔗 参考リンク

- [EEZ Studio公式サイト](https://www.envox.eu/eez-studio/)
- [EEZ Studio ドキュメント](https://github.com/eez-open/studio/wiki)
- [EEZ Studio GitHub](https://github.com/eez-open/studio)
- [LVGL公式ドキュメント](https://docs.lvgl.io/8.3/)
- [LVGL GitHub](https://github.com/lvgl/lvgl)
- [M5Stack公式サイト](https://m5stack.com/)
- [M5Unified GitHub](https://github.com/m5stack/M5Unified)
- [M5Stack Tab5 公式ドキュメント](https://docs.m5stack.com/)

## 💡 次のステップ

このテンプレートをベースに、以下のようなアプリケーションを開発できます：

1. **IoTダッシュボード**
   - センサーデータの可視化
   - リアルタイムグラフ表示
   - MQTT通信

2. **Webサービス連携アプリ**
   - REST API通信
   - JSONデータ解析
   - ニュース表示

3. **制御パネル**
   - デバイス制御
   - 設定管理
   - ログ表示

4. **データロガー**
   - センサーデータ記録
   - SDカード保存
   - グラフ表示

## 📝 ライセンス

このプロジェクトはMITライセンスの下で公開されています。

```
MIT License

Copyright (c) 2026 macole

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## 🤝 コントリビューション

バグ報告や機能追加の提案は、IssueまたはPull Requestでお願いします。

---

**作成日**: 2026年1月18日  
**バージョン**: 1.0.0  
**対応環境**: M5Stack Tab5 (ESP32-P4), M5Unified 0.2.10+, LVGL 8.3.11  
**作者**: macole
