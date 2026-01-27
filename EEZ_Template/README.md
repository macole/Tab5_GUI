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
- ✅ **スムーズなタッチ操作** - 連続操作に対応
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

- 画面にGUIが表示されることを確認
- タッチ操作が正常に動作することを確認
- EEZ Studioで作成したUI要素が表示されることを確認

## 📁 プロジェクト構造

```
EEZ_Template/
├── EEZ_Template.ino                    # メインプログラム
├── LV8wF_Template.eez-project         # EEZ Studioプロジェクトファイル
├── LV8wF_Template.eez-project-ui-state # EEZ Studio UI状態ファイル
├── lv_conf.h                           # LVGL設定ファイル
├── LICENSE                              # ライセンスファイル
├── README.md                           # このファイル
└── src/
    └── ui/                             # EEZ Studio生成ファイル
        ├── ui.c / ui.h                 # UI初期化
        ├── screens.c / screens.h       # 画面定義
        ├── eez-flow.cpp / eez-flow.h   # Flow言語エンジン
        ├── actions.h                   # アクション定義
        ├── styles.c / styles.h         # スタイル定義
        ├── images.c / images.h         # 画像データ
        ├── fonts.h                     # フォント定義
        ├── structs.h                   # 構造体定義
        └── vars.h                      # 変数定義
```

## 🎨 UIのカスタマイズ

### EEZ Studioでの編集

1. **EEZ Studioをインストール**
   - [公式サイト](https://www.envox.eu/eez-studio/)からダウンロード
   - Windows、Mac、Linux対応

2. **プロジェクトを開く**
   ```
   File → Open Project → LV8wF_Template.eez-project
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

詳細は[../Docs/CUSTOMIZATION_GUIDE.md](../Docs/CUSTOMIZATION_GUIDE.md)を参照してください。

## 🔍 デフォルト機能

このテンプレートは基本的な構造のみを提供しています。EEZ StudioでUIを設計し、Flow言語で動作を定義することで、独自のアプリケーションを作成できます。

### 実装済みの基本機能

- ✅ **LVGL初期化** - ディスプレイとタッチスクリーンの初期化
- ✅ **EEZ Flow統合** - Flow言語エンジンの統合
- ✅ **タッチ入力** - スムーズなタッチ操作対応
- ✅ **アプリケーション更新ループ** - カスタム処理を追加可能

### カスタマイズ例

このテンプレートをベースに、以下のような機能を追加できます：

- **Native変数** - C++コードから直接制御（[LV8wF_Native](../LV8wF_Native/README.md)参照）
- **WiFi/NTP** - ネットワーク機能（[LV8wF_Clock](../LV8wF_Clock/README.md)参照）
- **センサー読み取り** - 各種センサーデータの取得
- **データ通信** - MQTT、HTTP、WebSocketなど

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

詳細なトラブルシューティングガイドは[../Docs/TROUBLESHOOTING.md](../Docs/TROUBLESHOOTING.md)を参照してください。

### よくある問題

- **コンパイルエラー: lv_conf.h not found**: プロジェクトフォルダに`lv_conf.h`が存在することを確認
- **画面が表示されない**: [../Docs/TROUBLESHOOTING.md#画面が表示されない](../Docs/TROUBLESHOOTING.md#画面が表示されない)
- **メモリ不足エラー**: PSRAMを有効化（ツール → PSRAM → Enabled）
- **タッチパネルの反応が悪い**: [../Docs/TROUBLESHOOTING.md#7-タッチパネルの反応が悪い](../Docs/TROUBLESHOOTING.md#7-タッチパネルの反応が悪いボタンを何度も押さないと反応しない)
- **EEZ Flowのエラー**: [../Docs/TROUBLESHOOTING.md#6-eez-flowのアサーションエラー](../Docs/TROUBLESHOOTING.md#6-eez-flowのアサーションエラー)

### 機能を追加したい

**ガイド**:
- WiFi機能の追加方法: [../Docs/CUSTOMIZATION_GUIDE.md](../Docs/CUSTOMIZATION_GUIDE.md)
- センサー読み取り: [../Docs/CUSTOMIZATION_GUIDE.md](../Docs/CUSTOMIZATION_GUIDE.md)
- データ通信実装: [../Docs/CUSTOMIZATION_GUIDE.md](../Docs/CUSTOMIZATION_GUIDE.md)
- Native変数の実装: [../LV8wF_Native/README.md](../LV8wF_Native/README.md)

## 📖 詳細ドキュメント

- [プロジェクト概要](../Docs/PROJECT_OVERVIEW.md) - プロジェクト全体の詳細説明
- [クイックスタートガイド](../Docs/QUICK_START.md) - 初めての方向け
- [カスタマイズガイド](../Docs/CUSTOMIZATION_GUIDE.md) - アプリ開発方法
- [技術詳細](../Docs/TECHNICAL_GUIDE.md) - 内部実装の解説
- [トラブルシューティング](../Docs/TROUBLESHOOTING.md) - よくある問題と解決方法

## 🔗 参考リンク

### 関連プロジェクト

- **[LV8wF_Clock](../LV8wF_Clock/README.md)** - WiFi/NTP時刻同期機能を実装した時計アプリケーション
- **[LV8wF_Native](../LV8wF_Native/README.md)** - Native変数の実装例を含むサンプルアプリケーション

### 外部リソース

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

1. **時計アプリケーション**
   - WiFi接続とNTP時刻同期
   - リアルタイム時計表示
   - 参考: [LV8wF_Clock](../LV8wF_Clock/README.md)

2. **Native変数を使ったアプリ**
   - C++コードから直接制御
   - 自動更新機能
   - 参考: [LV8wF_Native](../LV8wF_Native/README.md)

3. **IoTダッシュボード**
   - センサーデータの可視化
   - リアルタイムグラフ表示
   - MQTT通信

4. **Webサービス連携アプリ**
   - REST API通信
   - JSONデータ解析
   - ニュース表示

5. **制御パネル**
   - デバイス制御
   - 設定管理
   - ログ表示

6. **データロガー**
   - センサーデータ記録
   - SDカード保存
   - グラフ表示

## 📝 更新履歴

### 2026-01-27
- ✅ プロジェクト構造の更新（`src/ui/`形式に統一）
- ✅ ドキュメントの整理と更新
- ✅ トラブルシューティングリンクの修正
- ✅ 他のプロジェクトとの整合性向上

### 2026-01-18
- ✅ 初版リリース
- ✅ 基本的なテンプレート構造の実装
- ✅ EEZ Studio Flow統合
- ✅ タッチ入力の最適化

## 📝 ライセンス

このプロジェクトはMITライセンスの下で公開されています。

---

**作成日**: 2026年1月18日  
**最終更新**: 2026年1月27日  
**バージョン**: 1.0.0  
**対応環境**: M5Stack Tab5 (ESP32-P4), M5Unified 0.2.10+, LVGL 8.3.11  
**ベースプロジェクト**: なし（テンプレートプロジェクト）
