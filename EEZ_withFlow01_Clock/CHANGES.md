# 変更履歴 - EEZ_withFlow01_Clock

## 2026年1月25日 - EEZ Flow統合の修正

### 🐛 修正した問題

**症状**: リブートループ（画面がチカチカして動かない）

プログラムのアップロード直後にクラッシュし、リブートを繰り返す問題が発生していました。

**エラーメッセージ**:
```
assert failed: void eez::flow::stopScript() eez-flow.cpp:7497 (false)
Core  1 register dump:
...
```

### 🔍 根本原因

EEZ Studioが生成する`eez-flow.cpp`のデフォルト実装では、以下のフック関数が`assert(false)`を呼び出すようになっていました：

```cpp
// eez-flow.cpp の7496-7498行目
static void stopScript() {
    assert(false);  // ← これが原因
}
```

この関数は、Flowスクリプトが終了またはエラーが発生した際に呼ばれるため、実行時にアサーションエラーでクラッシュしていました。

### ✅ 実装した解決策

#### 1. EEZ Flowフックのオーバーライド

`setup()`関数内で、`ui_init()`を呼び出す**前**に、すべての必要なフックをオーバーライドしました：

```cpp
// stopScriptフック: デフォルトのassert(false)を回避
eez::flow::stopScriptHook = []() {
    Serial.println("⚠️  EEZ Flow: stopScript called");
    // 必要に応じてスクリプト再起動処理をここに追加
};

// replacePageフック: 画面切り替え処理
eez::flow::replacePageHook = [](int16_t pageId, uint32_t animType, 
                                 uint32_t speed, uint32_t delay) {
    lv_obj_t *target_screen = nullptr;
    switch(pageId) {
        case SCREEN_ID_MAIN:
            target_screen = objects.main;
            break;
        case SCREEN_ID_PAGE1:
            target_screen = objects.page1;
            break;
        default:
            Serial.printf("⚠️  Unknown screen ID: %d\n", pageId);
            return;
    }
    if (target_screen) {
        lv_scr_load_anim(target_screen, (lv_scr_load_anim_t)animType, 
                        speed, delay, false);
    }
};

// scpiComponentInitフック
eez::flow::scpiComponentInitHook = []() {
    Serial.println("🔌 EEZ Flow: scpiComponentInit called");
};
```

#### 2. 初期化順序の最適化

```cpp
void setup() {
    // 1. ハードウェア初期化
    M5.begin(cfg);
    delay(100);  // 安定化待ち
    
    // 2. シリアル通信初期化
    Serial.begin(SERIAL_BAUD_RATE);
    delay(100);
    
    // 3. LVGL初期化
    initLvglDisplay();
    initLvglTouch();
    
    // 4. EEZ Flowフック設定（重要！）
    setupEezFlowHooks();
    
    // 5. EEZ Studio UI初期化
    lv_obj_clean(lv_scr_act());  // 既存画面をクリア
    ui_init();                    // UI初期化
    
    // 6. その他の設定
    M5.Display.setBrightness(DEFAULT_BRIGHTNESS);
}
```

#### 3. loop()での遅延処理

EEZ Flowエンジンが完全に初期化されるまで、`ui_tick()`の呼び出しを遅延させました：

```cpp
void loop() {
    static unsigned long loopStartTime = millis();
    
    M5.update();
    lv_timer_handler();
    
    // 1秒待機してからEEZ Flow Tickを開始
    if (millis() - loopStartTime > 1000) {
        ui_tick();
    }
    
    delay(LVGL_TIMER_DELAY_MS);
}
```

#### 4. デバッグ情報の追加

トラブルシューティングを容易にするため、詳細なログを追加：

```cpp
// チェックポイント（各初期化段階）
Serial.println("📍 Checkpoint 1: Before LVGL display initialization");
// ...

// メモリ状況の表示
Serial.printf("📊 Memory Status:\n");
Serial.printf("   Free Heap: %d bytes\n", ESP.getFreeHeap());
Serial.printf("   Free PSRAM: %d bytes\n", ESP.getFreePsram());

// 定期的なループカウント（1000回ごと）
if (loopCount % 1000 == 0) {
    Serial.printf("📍 Loop count: %lu (uptime: %lu ms)\n", 
                  loopCount, millis());
}
```

### 📝 変更されたファイル

#### `EEZ_withFlow01_Clock.ino`

**追加された機能**:
- EEZ Flowフックのオーバーライド実装
- 詳細なデバッグログ（チェックポイント、メモリ状況）
- 初期化の安定化（delay追加）
- ループカウント表示

**主な変更箇所**:
1. 必要なインクルード（行10）:
   ```cpp
   #include "eez-flow.h"
   ```

2. setup()関数（行179-210）:
   - EEZ Flowフックの設定を追加
   - デバッグログの追加
   - 初期化順序の明確化

3. loop()関数（行240-260）:
   - 遅延処理の追加
   - ループカウント表示の追加

### 🧪 検証方法

段階的なデバッグ手順を確立しました：

#### ステップ1: LVGL単体テスト

`ui_init()`をコメントアウトし、LVGLの基本的な描画をテスト：

```cpp
// テスト用の描画
lv_obj_t * label = lv_label_create(lv_scr_act());
lv_label_set_text(label, "LVGL Test OK!");
lv_obj_center(label);
```

**期待される結果**: 白い背景に黒いテキストが表示される

#### ステップ2: EEZ Flowフック設定

フックを設定してから`ui_init()`を有効化：

```cpp
setupEezFlowHooks();  // フック設定
ui_init();            // UI初期化
```

**期待される結果**: リブートせず、EEZ StudioのUIが表示される

#### ステップ3: 完全な動作確認

`ui_tick()`を有効化してFlowスクリプトを実行：

```cpp
ui_tick();  // EEZ Flow Tickハンドラー
```

**期待される結果**: UIが正常に動作し、Flowスクリプトが実行される

### 📚 ドキュメント更新

以下のドキュメントを更新・作成しました：

1. **README.md**
   - 「重要: EEZ Flowフックの設定」セクションを追加
   - 初期化順序の詳細説明を追加
   - サンプルコードを追加

2. **TROUBLESHOOTING.md**
   - 「6. EEZ Flowのアサーションエラー（リブートループ）」セクションを追加
   - 症状、原因、解決策を詳細に記載
   - デバッグ方法を説明

3. **CHANGES.md** (このファイル)
   - 問題の詳細と解決策を記録
   - 変更履歴を文書化

### 🎓 学んだ教訓

1. **EEZ Studioのフックは必ずオーバーライドが必要**
   - デフォルト実装は開発環境向け（assert(false)）
   - 本番環境では必ず独自実装を提供する

2. **初期化順序が重要**
   - ハードウェア → LVGL → フック設定 → UI初期化

3. **段階的デバッグが効果的**
   - まずLVGL単体で動作確認
   - 次にEEZ Flow統合
   - 最後にFlowスクリプト実行

4. **詳細なログが問題解決を加速**
   - チェックポイント
   - メモリ状況
   - ループカウント

### 🔗 関連リンク

- [EEZ Studio公式ドキュメント](https://www.envox.eu/studio/)
- [LVGL 8.3ドキュメント](https://docs.lvgl.io/8.3/)
- [M5Stack Tab5公式情報](https://docs.m5stack.com/en/core/Tab5)

---

## 2026年1月25日 - タッチパネルの反応性改善

### 🐛 修正した問題

**症状**: タッチパネルの反応が悪い（ボタンを3-4回押さないと反応しない）

タッチパネルでボタンを押しても、何度も繰り返さないと反応しない問題が発生していました。

### 🔍 根本原因

1. **タッチ座標の回転変換がされていない**
   - ディスプレイが90度回転しているが、タッチ座標がそのまま使用されていた
   - そのため、押した位置と反応する位置がずれていた

2. **タッチ検出のタイミング問題**
   - `wasPressed()`のみを使用していたため、タッチが検出されにくかった
   - `isPressed()`も併用することで、タッチ検出の確実性が向上

3. **読み取り間隔が長い**
   - デフォルトの30ms間隔では、タッチの反応が遅い場合があった

### ✅ 実装した解決策

#### 1. タッチ座標の回転変換

`lv_indev_read()`関数で、タッチ座標を90度回転変換するように修正：

```cpp
static void lv_indev_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    auto touch_detail = M5.Touch.getDetail();
    
    // isPressed()とwasPressed()の両方をチェック
    if (touch_detail.isPressed() || touch_detail.wasPressed()) {
        data->state = LV_INDEV_STATE_PR;
        
        int16_t raw_x = touch_detail.x;
        int16_t raw_y = touch_detail.y;
        
        // 時計回り90度回転: (x, y) → (y, 720 - x)
        int16_t rotated_x = raw_y;
        int16_t rotated_y = 720 - raw_x;
        
        // 座標範囲チェック
        if (rotated_x < 0) rotated_x = 0;
        if (rotated_x >= EXAMPLE_LCD_H_RES) rotated_x = EXAMPLE_LCD_H_RES - 1;
        if (rotated_y < 0) rotated_y = 0;
        if (rotated_y >= EXAMPLE_LCD_V_RES) rotated_y = EXAMPLE_LCD_V_RES - 1;
        
        data->point.x = rotated_x;
        data->point.y = rotated_y;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}
```

#### 2. タッチ検出の改善

`wasPressed()`だけでなく、`isPressed()`も併用することで、タッチ検出の確実性を向上：

```cpp
// 変更前
if (touch_detail.wasPressed()) {
    // ...
}

// 変更後
if (touch_detail.isPressed() || touch_detail.wasPressed()) {
    // ...
}
```

#### 3. 読み取り間隔の短縮

LVGL 8.3.11では、読み取り間隔は`lv_conf.h`で設定します。デフォルトの30msから10msに変更することで、タッチ入力の応答性が向上します：

**lv_conf.h の設定**:
```c
/*Input device read period in milliseconds*/
/* タッチ入力の応答性を向上させるには、この値を小さくしてください（例: 10ms）*/
#define LV_INDEV_DEF_READ_PERIOD 10     /*[ms]*/  // デフォルト: 30ms
```

**注意**: LVGL 8.3.11では、`lv_indev_set_read_timer()`という関数は存在しません。`lv_conf.h`で設定する必要があります。

#### 4. デバッグログの追加

タッチ座標のデバッグログを追加（200msごとに出力）：

```cpp
static unsigned long last_touch_log = 0;
if (millis() - last_touch_log > 200) {
    Serial.printf("🖐️  Touch: raw(%d, %d) → rotated(%d, %d)\n", 
                 raw_x, raw_y, rotated_x, rotated_y);
    last_touch_log = millis();
}
```

### 📝 変更されたファイル

#### `EEZ_withFlow01_Clock.ino`

**変更箇所**:
1. `lv_indev_read()`関数（行85-125）:
   - タッチ座標の回転変換を追加
   - `isPressed()`と`wasPressed()`の両方をチェック
   - デバッグログの追加
   - 座標範囲チェックの追加

2. `initLvglTouch()`関数（行139-160）:
   - 読み取り間隔を10msに設定
   - エラーチェックの追加

### 🧪 検証方法

1. **タッチ座標の確認**:
   - 画面の四隅をタッチして、座標が正しい範囲内か確認
   - シリアルモニターでタッチ座標のログを確認

2. **反応性の確認**:
   - ボタンを1回押して、即座に反応するか確認
   - 連続してタッチしても、すべてのタッチが検出されるか確認

3. **座標の精度確認**:
   - ボタンの位置とタッチ座標が一致しているか確認
   - 画面の端をタッチしても、正しく検出されるか確認

### 🎓 学んだ教訓

1. **ディスプレイの回転とタッチ座標の対応**
   - ディスプレイが回転している場合、タッチ座標も回転変換が必要
   - 回転方向（時計回り/反時計回り）を正しく把握する

2. **タッチ検出の確実性**
   - `wasPressed()`だけでは検出が不安定な場合がある
   - `isPressed()`も併用することで、タッチ検出の確実性が向上

3. **読み取り間隔の最適化**
   - デフォルトの30msでは遅い場合がある
   - 10ms程度に短縮することで、応答性が向上

4. **デバッグログの重要性**
   - タッチ座標のログを出力することで、問題の特定が容易になる
   - 連続出力を防ぐため、一定間隔でのみログ出力

### 🔗 関連ドキュメント

- [TROUBLESHOOTING.md](TROUBLESHOOTING.md#7-タッチパネルの反応が悪いボタンを何度も押さないと反応しない) - タッチパネルの問題と解決策

---

**作成日**: 2026年1月25日  
**最終更新**: 2026年1月25日
