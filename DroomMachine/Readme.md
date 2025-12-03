# DroomMachine - ドラムマシン（音楽シーケンサー）

M5Stack Tab5とM5UnitSynthを使用した16ステップドラムマシン（音楽シーケンサー）プログラムです。

## 📋 概要

このプログラムは、16ステップのドラムシーケンサーを実装しています。5種類のドラム音（ベースドラム、スネアドラム、サイドスティック、ライドシンバル、ローボンゴ）を使用して、タッチ操作でノートを編集し、音楽を演奏できます。

## 🔧 必要なハードウェア

- M5Stack Tab5
- M5UnitSynth（音声合成ユニット）

## 📚 必要なライブラリ

- **M5Unified**: M5Stack統合ライブラリ
  ```bash
  arduino-cli lib install "M5Unified@0.2.10"
  ```

- **M5GFX**: M5Stack用グラフィックライブラリ
  ```bash
  arduino-cli lib install "M5GFX@0.2.15"
  ```

- **M5UnitSynth**: M5UnitSynth音声合成ライブラリ
  ```bash
  arduino-cli lib install "M5UnitSynth"
  ```

## 🚀 セットアップ

1. **ハードウェア接続**
   - M5UnitSynthをM5Stack Tab5に接続

2. **ボード設定**
   - Board: ESP32P4 Dev Module
   - USB CDC on boot: Enabled
   - Flash Size: 16MB (128Mb)
   - Partition Scheme: Custom
   - PSRAM: Enabled

3. **プログラムのアップロード**
   - Arduino IDEまたはArduino CLIでプログラムをアップロード

## 💻 使用方法

1. **ノートの編集**
   - 画面のグリッドをタッチしてノートをON/OFF
   - 5行が5種類のドラム音に対応
   - 16列が16ステップに対応

2. **再生・停止**
   - 再生ボタンをタッチして演奏開始
   - 再度タッチして停止

3. **BPM調整**
   - BPM（Beats Per Minute）を調整してテンポを変更

## 📊 機能

- **16ステップシーケンサー**: 16ステップのパターンを編集可能
- **5種類のドラム音**:
  - Bass Drum (35)
  - Snare (38)
  - Side Stick (37)
  - Ride Cymbal (59)
  - Low Bongo (61)
- **タッチ操作**: 画面をタッチしてノートを編集
- **再生・停止機能**: パターンの再生と停止
- **BPM調整**: テンポの調整
- **ビジュアルフィードバック**: 再生中のステップをハイライト表示

## 🔍 プログラムの動作

1. **初期化**
   - M5Stackの初期化
   - M5UnitSynthの初期化
   - 画面の設定
   - グリッドの描画

2. **メインループ**
   - タッチ入力の検出
   - ノートの編集処理
   - 再生中のステップ更新
   - ドラム音の再生
   - 画面の更新

## 📝 コードの主要なポイント

### ドラム音の定義
```cpp
const int drumNotes[] = {35, 38, 37, 59, 61};
String instruments[5] = {
  "Bass Drum",
  "Snare",
  "Side Stick",
  "Ride Cymbal",
  "Low Bongo"
};
```

### ノートの管理
```cpp
bool notePlayed[5][16] = {0};  // 5種類 × 16ステップ
```

### 再生制御
```cpp
bool playing = 0;  // 再生中かどうか
long bmp = 90;     // BPM値
```

## 🎵 MIDIノート番号

- **35**: Bass Drum
- **37**: Side Stick
- **38**: Snare
- **59**: Ride Cymbal
- **61**: Low Bongo

## ⚙️ カスタマイズ

### BPMの変更
```cpp
long bmp = 90;  // デフォルトBPMを変更
```

### ドラム音の追加
`drumNotes`配列と`instruments`配列に新しいドラム音を追加できます。

## 📖 参考資料

- [M5UnitSynth 公式ドキュメント](https://docs.m5stack.com/)
- [M5Unified ライブラリ](https://github.com/m5stack/M5Unified)
- [MIDIノート番号一覧](https://www.midi.org/specifications-old/item/table-1-summary-of-midi-message)

## 📄 ライセンス

このプログラムは教育・学習目的で提供されています。

---

**作成日**: 2025年11月  
**対象デバイス**: M5Stack Tab5 + M5UnitSynth  
**動作確認**: ✅ 正常動作確認済み

