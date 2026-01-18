/*******************************************************************************
 * EEZ Studio Native Actions and Variables Implementation
 * 
 * このファイルは、EEZ Studioで定義したNativeアクションと変数の実装です。
 * 
 ******************************************************************************/

#include <M5Unified.h>
#include <lvgl.h>
#include "actions.h"
#include "screens.h"

// ============================================================================
// Native変数のストレージ
// ============================================================================

// スライダー値を保持（0-255）
static int32_t g_slider_value = 128;

// グローバルメッセージを保持
static String g_global_message = "Ready";

// ============================================================================
// Native変数のGetter/Setter実装
// ============================================================================

/**
 * @brief slider_value変数のGetter
 * @return 現在のスライダー値
 */
extern "C" int32_t get_var_slider_value()
{
    return g_slider_value;
}

/**
 * @brief slider_value変数のSetter
 * @param value 新しいスライダー値
 */
extern "C" void set_var_slider_value(int32_t value)
{
    g_slider_value = value;
    Serial.printf("✏️  Slider value set to: %d\n", value);
    
    // スライダー値に応じて画面の明るさを調整
    M5.Display.setBrightness(value);
}

/**
 * @brief gloval_msg変数のGetter
 * @return 現在のグローバルメッセージ文字列
 */
extern "C" const char* get_var_gloval_msg()
{
    return g_global_message.c_str();
}

/**
 * @brief gloval_msg変数のSetter
 * @param value 新しいメッセージ文字列
 */
extern "C" void set_var_gloval_msg(const char* value)
{
    if (value != nullptr) {
        g_global_message = String(value);
        Serial.printf("📝 Global message set to: %s\n", value);
    }
}

// ============================================================================
// Nativeアクションの実装
// ============================================================================

/**
 * @brief ボタンクリックアクション
 * 
 * ボタンが押された時に呼ばれます。
 * グローバルメッセージを"Changed"に変更し、クリック回数も表示します。
 * 
 * @param e LVGLイベント
 */
extern "C" void action_btn_click(lv_event_t *e)
{
    static int click_count = 0;
    click_count++;

    char msg[64];
    snprintf(msg, sizeof(msg), "Changed (%d)", click_count);
    set_var_gloval_msg(msg);    

    if (objects.label_01) {
        lv_label_set_text(objects.label_01, msg);    // LVGLのラベルを更新（label_01を使用）
    }
}

/**
 * @brief スイッチ変更アクション
 * 
 * スイッチの状態が変更された時に呼ばれます。
 * スイッチのON/OFFに応じてメッセージを更新します。
 * 
 * @param e LVGLイベント
 */
extern "C" void action_sw_change(lv_event_t *e)
{
    lv_obj_t *switch_obj = lv_event_get_target(e);
    
    if (switch_obj) {
        bool is_checked = lv_obj_has_state(switch_obj, LV_STATE_CHECKED);        

        const char *msg = is_checked ? "Switch is ON" : "Switch is OFF";        // メッセージを更新
        set_var_gloval_msg(msg);
        
        if (objects.label_01) {
            lv_label_set_text(objects.label_01, msg);        // LVGLのラベルを更新（label_01を使用）
        }
    }
}
