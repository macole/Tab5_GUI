# 変更履歴 - ui/フォルダからsrc/フォルダへの移行

## 📅 変更日: 2026年1月19日

## 🎯 変更の目的

Arduinoのビルドシステムが`src/`フォルダのみを自動認識してコンパイルするため、`ui/`フォルダから`src/`フォルダへ移行しました。

## 📝 変更内容の要約

### 1. フォルダ構造
```
変更前:
EEZ_Template2/
├── EEZ_Template2.ino
├── lv_conf.h
└── ui/                     ← このフォルダ

変更後:
EEZ_Template2/
├── EEZ_Template2.ino
├── lv_conf.h
└── src/                    ← リネーム
```

### 2. 変更ファイル一覧

| ファイル | 変更内容 | 変更箇所 |
|---------|---------|---------|
| **EEZ_Template2.ino** | インクルードパス変更 | 行23-24 |
| **src/ui.c** | 不要なインクルード削除 | 行1-3 |
| **README.md** | ドキュメント更新 | 複数箇所 |

### 3. 詳細な変更内容

#### EEZ_Template2.ino
```diff
- #include "ui/ui.h"
- #include "ui/screens.h"
+ #include "src/ui.h"
+ #include "src/screens.h"
```

#### src/ui.c
```diff
- #if defined(EEZ_FOR_LVGL)
- #include <eez/core/vars.h>
- #endif
-
+ // EEZ Studio生成ファイル - src/フォルダに配置済み
+
```

## ✅ 動作確認

- [x] コンパイル成功
- [x] リンクエラー解消
- [x] ドキュメント更新完了

## 🔗 関連ドキュメント

- **詳細ガイド**: [FOLDER_MIGRATION_GUIDE.md](./FOLDER_MIGRATION_GUIDE.md)
- **プロジェクトREADME**: [README.md](./README.md)

## ⚠️ 今後の注意点

EEZ Studioで再エクスポートするたびに、`src/ui.c`の先頭行を修正する必要があります。

詳細は[FOLDER_MIGRATION_GUIDE.md](./FOLDER_MIGRATION_GUIDE.md)の「EEZ Studioで再エクスポートする際の手順」を参照してください。
