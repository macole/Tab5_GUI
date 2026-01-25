# フォルダ変更対応ガイド (ui/ → src/)

EEZ Studioから出力されたファイルを`src/`フォルダに配置する際の変更箇所をまとめたガイドです。

## 📋 変更が必要な理由

Arduinoのビルドシステムは以下のルールで動作します：

- ✅ **プロジェクトルート**の`.ino`、`.cpp`、`.c`ファイルは**自動コンパイル**
- ✅ **`src/`フォルダ**内のファイルは**自動コンパイル**（サブディレクトリも含む）
- ❌ **その他のフォルダ**（`ui/`、`lib/`など）は**自動コンパイルされない**

そのため、`ui/`フォルダだと`.c`ファイルがコンパイルされず、リンクエラーが発生します。

---

## 🔧 変更箇所の完全リスト

### 1. フォルダ構造の変更

#### ✅ 実施済み

```bash
# フォルダ名を変更
ui/ → src/
```

**変更後の構造:**
```
EEZ_Template2/
├── EEZ_Template2.ino
├── lv_conf.h
├── README.md
├── EEZ/
│   └── LV8wF_Template2.eez-project
└── src/                    # ← 変更
    ├── ui.h, ui.c
    ├── screens.h, screens.c
    ├── eez-flow.h, eez-flow.cpp
    ├── vars.h
    ├── actions.h
    ├── styles.h, styles.c
    ├── images.h, images.c
    ├── fonts.h
    └── structs.h
```

---

### 2. メインプログラムの変更

#### ファイル: `EEZ_Template2.ino`

✅ **実施済み**

```cpp
// 変更前
#include "ui/ui.h"
#include "ui/screens.h"

// 変更後
#include "src/ui.h"
#include "src/screens.h"
```

**変更箇所:**
- 行23: `#include "ui/ui.h"` → `#include "src/ui.h"`
- 行24: `#include "ui/screens.h"` → `#include "src/screens.h"`

---

### 3. EEZ Studio生成ファイルの修正

#### ファイル: `src/ui.c`

✅ **実施済み**

```c
// 変更前（EEZ Studioが生成）
#if defined(EEZ_FOR_LVGL)
#include <eez/core/vars.h>
#endif

// 変更後
// EEZ Studio生成ファイル - src/フォルダに配置済み
```

**変更箇所:**
- 行1-3: 不要なインクルードを削除してコメントに置き換え

**⚠️ 重要:** この変更は、EEZ Studioで再エクスポートするたびに必要です！

---

### 4. README.mdの更新

✅ **実施済み**

以下の箇所で`ui/`を`src/`に変更：

1. **プロジェクト構造図**
   ```
   ui/ → src/
   ```

2. **インクルードパスの例**
   ```cpp
   "ui/ui.h" → "src/ui.h"
   "ui/screens.h" → "src/screens.h"
   ```

3. **説明文**
   - "ui/フォルダ" → "src/フォルダ"
   - "EEZ Studio生成ファイル - ui/フォルダに配置済み" 
     → "EEZ Studio生成ファイル - src/フォルダに配置済み"

---

## 🔄 EEZ Studioで再エクスポートする際の手順

### 毎回必要な作業

1. **EEZ Studioでエクスポート**
   ```
   Build → Build & Export
   ```

2. **生成ファイルをsrc/にコピー**
   ```bash
   # すべてのEEZ生成ファイルを src/ フォルダにコピー（上書き）
   ```

3. **src/ui.cを修正**（必須！）
   ```bash
   # ファイル先頭の以下の行を削除：
   # #if defined(EEZ_FOR_LVGL)
   # #include <eez/core/vars.h>
   # #endif
   
   # 以下のコメントに置き換え：
   # // EEZ Studio生成ファイル - src/フォルダに配置済み
   ```

### 自動化スクリプト（オプション）

毎回手動で修正するのが面倒な場合、以下のスクリプトを使用できます：

#### macOS/Linux用: `fix_ui_c.sh`

```bash
#!/bin/bash
# src/ui.cの先頭3行を修正するスクリプト

cd "$(dirname "$0")"

if [ -f "src/ui.c" ]; then
    # 元のファイルをバックアップ
    cp src/ui.c src/ui.c.bak
    
    # 最初の3行を削除して新しいコメントを追加
    tail -n +4 src/ui.c > src/ui.c.tmp
    echo "// EEZ Studio生成ファイル - src/フォルダに配置済み" > src/ui.c.new
    echo "" >> src/ui.c.new
    cat src/ui.c.tmp >> src/ui.c.new
    mv src/ui.c.new src/ui.c
    rm src/ui.c.tmp
    
    echo "✅ src/ui.c を修正しました"
else
    echo "❌ src/ui.c が見つかりません"
fi
```

**使い方:**
```bash
chmod +x fix_ui_c.sh
./fix_ui_c.sh
```

---

## 📝 他のプロジェクトに適用する場合

### 新規プロジェクトの場合

1. **フォルダ構成**
   ```
   YourProject/
   ├── YourProject.ino
   ├── lv_conf.h
   └── src/              # ← EEZ生成ファイルをここに配置
       └── (すべてのEEZ生成ファイル)
   ```

2. **メインプログラム**
   ```cpp
   #include "src/ui.h"
   #include "src/screens.h"
   ```

3. **src/ui.cを修正**（上記参照）

### 既存プロジェクトの変更

1. **既存のui/フォルダをリネーム**
   ```bash
   mv ui src
   ```

2. **インクルードパスを変更**
   ```cpp
   // すべての .ino ファイルで
   "ui/ui.h" → "src/ui.h"
   "ui/screens.h" → "src/screens.h"
   ```

3. **src/ui.cを修正**（上記参照）

---

## ✅ 変更確認チェックリスト

コンパイル前に以下を確認してください：

- [ ] `src/`フォルダが存在する（`ui/`フォルダではない）
- [ ] `EEZ_Template2.ino`に`#include "src/ui.h"`が記述されている
- [ ] `src/ui.c`の先頭行が修正されている（`<eez/core/vars.h>`のインクルードがない）
- [ ] すべてのEEZ生成ファイルが`src/`フォルダに配置されている
- [ ] `lv_conf.h`がプロジェクトルートに存在する

---

## 🐛 トラブルシューティング

### エラー: "undefined reference to ui_init"

**原因**: `src/`フォルダではなく別の名前のフォルダを使用している

**解決策**:
```bash
# フォルダ名を src/ にリネーム
mv your_folder_name src
```

### エラー: "src/ui.h: No such file or directory"

**原因**: EEZ生成ファイルが`src/`フォルダに配置されていない

**解決策**:
```bash
# すべてのEEZ生成ファイルを src/ にコピー
cp path/to/eez/export/*.* src/
```

### エラー: "eez/core/vars.h: No such file or directory"

**原因**: `src/ui.c`の修正が必要

**解決策**: 上記「3. EEZ Studio生成ファイルの修正」を参照

---

## 📊 変更箇所の概要表

| ファイル/フォルダ | 変更前 | 変更後 | 状態 |
|------------------|--------|--------|------|
| フォルダ名 | `ui/` | `src/` | ✅ 完了 |
| `EEZ_Template2.ino` (行23) | `"ui/ui.h"` | `"src/ui.h"` | ✅ 完了 |
| `EEZ_Template2.ino` (行24) | `"ui/screens.h"` | `"src/screens.h"` | ✅ 完了 |
| `src/ui.c` (行1-3) | `#include <eez/core/vars.h>` | コメントに置き換え | ✅ 完了 |
| `README.md` | 複数箇所で`ui/`参照 | `src/`に変更 | ✅ 完了 |

---

## 🎯 まとめ

### 最重要ポイント

1. **フォルダ名は必ず`src/`にする**
   - Arduinoは`src/`フォルダのみ自動認識
   - 他の名前（`ui/`、`lib/`など）はコンパイルされない

2. **src/ui.cの修正を忘れない**
   - EEZ Studioが生成する`<eez/core/vars.h>`のインクルードは削除
   - 再エクスポートするたびに必要

3. **インクルードパスは`src/`から**
   - メインプログラムで`#include "src/ui.h"`
   - `src/`フォルダ内のファイル同士は相対パス

これらを守れば、EEZ Studioで生成したUIがArduinoで正常にコンパイルできます！

---

**作成日**: 2026年1月19日  
**対象プロジェクト**: EEZ_Template2  
**最終確認**: すべての変更完了、コンパイル可能
