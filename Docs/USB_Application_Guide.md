# M5Stack Tab5のUSB活用事例 - 入力・出力デバイスの実装ガイド

M5Stack Tab5は、**USB Type-A (Host)** と **USB Type-C (OTG)** の2つのUSBポートを搭載しており、ESP32-P4の高速USB機能を活用した多彩なアプリケーション開発が可能です。

---

## 📌 Tab5のUSB機能概要

### ハードウェア仕様

| 項目 | 仕様 |
|------|------|
| **USB Type-A** | USB Host機能 |
| **USB Type-C** | USB 2.0 OTG (Host/Device切り替え可能) |
| **対応速度** | High Speed (480Mbps), Full Speed (12Mbps), Low Speed (1.5Mbps) |
| **USB PHY** | 内蔵USB PHY (外部PHY不要) |
| **ソフトウェアスタック** | TinyUSB (ESP-IDF統合版) |

### USBモード

1. **USB Host Mode**: キーボード、マウス、USBメモリ、カメラなどのデバイスを接続
2. **USB Device Mode**: Tab5自体がUSBデバイスとして動作(キーボード、マウス、ストレージ、CDC-ACMシリアル等)

---

## 💡 USB Host Mode - 入力デバイスの例

### 例1: USBキーボード入力の実装

USB Type-AポートにUSBキーボードを接続して、キー入力を画面に表示するアプリケーションです。

#### ESP-IDF実装例

```cpp
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "usb/usb_host.h"
#include "usb/hid_host.h"
#include "usb/hid_usage_keyboard.h"
#include <hal/hal.h>

// グローバル変数
static bool hid_device_connected = false;
static hid_host_device_handle_t hid_device = NULL;

// HIDキーボードレポートコールバック
void hid_host_keyboard_report_callback(const uint8_t *const data, const int length)
{
    hid_keyboard_input_report_boot_t *kb_report = 
        (hid_keyboard_input_report_boot_t *)data;
    
    // 修飾キーチェック
    if (kb_report->modifier.left_ctrl) {
        printf("Left Ctrl pressed\n");
    }
    if (kb_report->modifier.left_shift) {
        printf("Left Shift pressed\n");
    }
    
    // 押されたキーの処理
    for (int i = 0; i < 6; i++) {
        if (kb_report->key[i] != 0) {
            printf("Key code: 0x%02X\n", kb_report->key[i]);
            
            // キーコードを文字に変換
            if (kb_report->key[i] >= HID_KEY_A && 
                kb_report->key[i] <= HID_KEY_Z) {
                char c = 'a' + (kb_report->key[i] - HID_KEY_A);
                if (kb_report->modifier.left_shift || 
                    kb_report->modifier.right_shift) {
                    c = c - 32; // 大文字に変換
                }
                printf("Character: %c\n", c);
            }
        }
    }
}

// HIDホストイベントコールバック
void hid_host_event_callback(const hid_host_event_t *event, void *arg)
{
    switch (event->event) {
        case HID_HOST_INTERFACE_EVENT_INPUT_REPORT:
            hid_host_keyboard_report_callback(
                event->input_report.data, 
                event->input_report.length);
            break;
            
        case HID_HOST_INTERFACE_EVENT_DISCONNECTED:
            printf("HID Device disconnected\n");
            hid_device_connected = false;
            break;
            
        case HID_HOST_INTERFACE_EVENT_TRANSFER_ERROR:
            printf("HID Transfer error\n");
            break;
            
        default:
            break;
    }
}

// HIDデバイスコールバック
void hid_host_device_callback(hid_host_device_handle_t hid_device_handle,
                               const hid_host_driver_event_t event,
                               void *arg)
{
    switch (event) {
        case HID_HOST_DRIVER_EVENT_CONNECTED:
            printf("HID Device connected\n");
            
            // デバイス記述子取得
            const hid_host_device_config_t dev_config = {
                .callback = hid_host_event_callback,
                .callback_arg = NULL
            };
            
            // キーボードインターフェースをクレーム
            hid_host_claim_interface(&dev_config, &hid_device);
            hid_device_connected = true;
            break;
            
        default:
            break;
    }
}

class UsbKeyboardApp : public mooncake::AppBase {
private:
    lv_obj_t* _screen = nullptr;
    lv_obj_t* _text_area = nullptr;
    std::string _input_text;
    
public:
    void onCreate() override {
        setAppInfo().name = "USB Keyboard Demo";
    }
    
    void onOpen() override {
        LvglLockGuard lock;
        
        // UI作成
        _screen = lv_obj_create(lv_scr_act());
        lv_obj_set_size(_screen, GetHAL()->getDisplayWidth(), 
                       GetHAL()->getDisplayHeight());
        
        // タイトル
        lv_obj_t* title = lv_label_create(_screen);
        lv_label_set_text(title, "USB Keyboard Input");
        lv_obj_set_style_text_font(title, &lv_font_montserrat_32, 0);
        lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);
        
        // テキストエリア
        _text_area = lv_textarea_create(_screen);
        lv_obj_set_size(_text_area, 700, 400);
        lv_obj_align(_text_area, LV_ALIGN_CENTER, 0, 0);
        lv_textarea_set_placeholder_text(_text_area, 
            "Connect USB keyboard and start typing...");
        
        // USB Host初期化
        initUsbHost();
    }
    
    void initUsbHost() {
        // USB Host設定
        const usb_host_config_t host_config = {
            .skip_phy_setup = false,
            .intr_flags = ESP_INTR_FLAG_LEVEL1,
        };
        ESP_ERROR_CHECK(usb_host_install(&host_config));
        
        // HID Host初期化
        const hid_host_driver_config_t hid_config = {
            .create_background_task = true,
            .task_priority = 5,
            .stack_size = 4096,
            .callback = hid_host_device_callback,
            .callback_arg = nullptr
        };
        ESP_ERROR_CHECK(hid_host_install(&hid_config));
    }
    
    void onLoop() override {
        // キーボード入力をテキストエリアに反映
        // (実際の実装ではコールバックからキューでデータを受け取る)
    }
    
    void onClose() override {
        // USB Host クリーンアップ
        if (hid_device) {
            hid_host_release_interface(hid_device);
        }
        hid_host_uninstall();
        usb_host_uninstall();
        
        if (_screen) {
            LvglLockGuard lock;
            lv_obj_del(_screen);
            _screen = nullptr;
        }
    }
};
```

---

### 例2: USBマウス入力の実装

USB Type-AポートにUSBマウスを接続して、カーソル移動とクリックイベントを処理します。

#### Arduino実装例(簡易版)

```cpp
#include <M5Unified.h>
#include "USB.h"
#include "USBHID.h"

USBHID HID;

// マウス座標
int mouse_x = 640;
int mouse_y = 360;

// マウスレポートコールバック
void mouse_callback(void* arg, uint8_t* report, uint16_t len) {
    if (len >= 4) {
        // バイト0: ボタン状態
        uint8_t buttons = report[0];
        // バイト1: X移動量
        int8_t dx = (int8_t)report[1];
        // バイト2: Y移動量
        int8_t dy = (int8_t)report[2];
        // バイト3: ホイール
        int8_t wheel = (int8_t)report[3];
        
        // カーソル位置更新
        mouse_x += dx * 2;  // 感度調整
        mouse_y += dy * 2;
        
        // 画面範囲内に制限
        mouse_x = constrain(mouse_x, 0, 1280);
        mouse_y = constrain(mouse_y, 0, 720);
        
        // ボタン判定
        if (buttons & 0x01) {
            Serial.println("Left button clicked");
        }
        if (buttons & 0x02) {
            Serial.println("Right button clicked");
        }
        if (buttons & 0x04) {
            Serial.println("Middle button clicked");
        }
    }
}

void setup() {
    M5.begin();
    M5.Display.setRotation(3);
    M5.Display.clear();
    
    Serial.begin(115200);
    
    // USB Host初期化
    HID.begin();
    HID.onEvent(mouse_callback);
    
    M5.Display.setTextSize(2);
    M5.Display.println("USB Mouse Demo");
    M5.Display.println("Connect USB mouse");
}

void loop() {
    M5.update();
    
    // カーソル描画
    M5.Display.fillCircle(mouse_x, mouse_y, 5, RED);
    
    delay(10);
    
    // 軌跡を少しフェードアウト
    M5.Display.fillCircle(mouse_x, mouse_y, 5, 
                          M5.Display.color565(240, 240, 240));
}
```

---

## 💡 USB Device Mode - 出力デバイスの例

Tab5自体をUSBデバイスとして動作させる例です。

### 例3: USBキーボードエミュレーション(HID Device)

Tab5をUSBキーボードとして認識させ、タッチスクリーンの入力をキーボード信号としてPCに送信します。

#### ESP-IDF実装例

```cpp
#include "tinyusb.h"
#include "class/hid/hid_device.h"
#include <hal/hal.h>

// HID レポート記述子(キーボード)
static const uint8_t hid_keyboard_report_descriptor[] = {
    TUD_HID_REPORT_DESC_KEYBOARD()
};

// TinyUSB HID コールバック
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                                hid_report_type_t report_type,
                                uint8_t* buffer, uint16_t reqlen)
{
    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                            hid_report_type_t report_type,
                            uint8_t const* buffer, uint16_t bufsize)
{
}

class UsbKeyboardEmulatorApp : public mooncake::AppBase {
private:
    lv_obj_t* _screen = nullptr;
    lv_obj_t* _keyboard = nullptr;
    bool _usb_initialized = false;
    
public:
    void onCreate() override {
        setAppInfo().name = "USB Keyboard Emulator";
        initUsbDevice();
    }
    
    void initUsbDevice() {
        // TinyUSB設定
        const tinyusb_config_t tusb_cfg = {
            .device_descriptor = NULL,  // デフォルト使用
            .string_descriptor = NULL,  // デフォルト使用
            .external_phy = false,
            .configuration_descriptor = NULL,
        };
        
        ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
        
        // HID デバイス初期化
        const tinyusb_config_hid_t hid_cfg = {
            .subclass = HID_SUBCLASS_NONE,
            .protocol = HID_ITF_PROTOCOL_KEYBOARD,
            .report_desc = hid_keyboard_report_descriptor,
            .report_desc_len = sizeof(hid_keyboard_report_descriptor),
        };
        
        ESP_ERROR_CHECK(tusb_hid_init(&hid_cfg));
        _usb_initialized = true;
    }
    
    void onOpen() override {
        LvglLockGuard lock;
        
        // UI作成
        _screen = lv_obj_create(lv_scr_act());
        lv_obj_set_size(_screen, GetHAL()->getDisplayWidth(), 
                       GetHAL()->getDisplayHeight());
        
        // タイトル
        lv_obj_t* title = lv_label_create(_screen);
        lv_label_set_text(title, "USB Keyboard Emulator");
        lv_obj_set_style_text_font(title, &lv_font_montserrat_28, 0);
        lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
        
        // ステータス
        lv_obj_t* status = lv_label_create(_screen);
        lv_label_set_text(status, tud_mounted() ? 
                         "USB Connected" : "Connect to PC via USB-C");
        lv_obj_align(status, LV_ALIGN_TOP_MID, 0, 50);
        
        // 仮想キーボードボタン配列
        createVirtualKeyboard();
    }
    
    void createVirtualKeyboard() {
        // 簡易的な仮想キーボードUI
        const char* keys[] = {
            "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
            "A", "S", "D", "F", "G", "H", "J", "K", "L",
            "Z", "X", "C", "V", "B", "N", "M"
        };
        
        int x_offset = 50;
        int y_offset = 150;
        int btn_width = 80;
        int btn_height = 60;
        int spacing = 10;
        
        for (int row = 0; row < 3; row++) {
            int keys_in_row = (row == 0) ? 10 : (row == 1) ? 9 : 7;
            
            for (int col = 0; col < keys_in_row; col++) {
                int key_index = (row == 0) ? col : 
                                (row == 1) ? 10 + col : 19 + col;
                
                lv_obj_t* btn = lv_btn_create(_screen);
                lv_obj_set_size(btn, btn_width, btn_height);
                lv_obj_set_pos(btn, 
                    x_offset + col * (btn_width + spacing),
                    y_offset + row * (btn_height + spacing));
                
                lv_obj_add_event_cb(btn, key_event_cb, 
                                   LV_EVENT_CLICKED, 
                                   (void*)(intptr_t)key_index);
                
                lv_obj_t* label = lv_label_create(btn);
                lv_label_set_text(label, keys[key_index]);
                lv_obj_center(label);
            }
        }
    }
    
    static void key_event_cb(lv_event_t* e) {
        int key_index = (int)(intptr_t)lv_event_get_user_data(e);
        
        // USBデバイスが接続されているか確認
        if (!tud_mounted()) {
            return;
        }
        
        // HID キーボードレポート送信
        uint8_t keycode = HID_KEY_A + key_index;  // 簡易変換
        
        // キー押下レポート
        uint8_t report[8] = {0};
        report[2] = keycode;  // キーコード
        tud_hid_keyboard_report(1, 0, report);
        
        // 少し待機
        vTaskDelay(pdMS_TO_TICKS(50));
        
        // キー解放レポート
        uint8_t release_report[8] = {0};
        tud_hid_keyboard_report(1, 0, release_report);
    }
    
    void onClose() override {
        if (_screen) {
            LvglLockGuard lock;
            lv_obj_del(_screen);
            _screen = nullptr;
        }
    }
};
```

---

### 例4: USBシリアルデバイス(CDC-ACM)

Tab5をUSBシリアルポートとして認識させ、PCとシリアル通信を行います。

#### ESP-IDF実装例

```cpp
#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include <hal/hal.h>

// CDCラインコーディング変更コールバック
void tinyusb_cdc_line_coding_changed_cb(int itf, 
                                         cdc_line_coding_t const* line_coding)
{
    printf("Line coding changed: %d baud, %d bits, %d parity, %d stop\n",
           line_coding->bit_rate,
           line_coding->data_bits,
           line_coding->parity,
           line_coding->stop_bits);
}

// CDC受信コールバック
void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t* event)
{
    uint8_t buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE];
    size_t rx_size = 0;
    
    // データ読み取り
    esp_err_t ret = tinyusb_cdcacm_read(itf, buf, 
                                        CONFIG_TINYUSB_CDC_RX_BUFSIZE, 
                                        &rx_size);
    if (ret == ESP_OK && rx_size > 0) {
        printf("Received %d bytes: ", rx_size);
        for (int i = 0; i < rx_size; i++) {
            printf("%c", buf[i]);
        }
        printf("\n");
        
        // エコーバック
        tinyusb_cdcacm_write_queue(itf, buf, rx_size);
        tinyusb_cdcacm_write_flush(itf, 0);
    }
}

class UsbSerialApp : public mooncake::AppBase {
private:
    lv_obj_t* _screen = nullptr;
    lv_obj_t* _console = nullptr;
    
public:
    void onCreate() override {
        setAppInfo().name = "USB Serial Device";
        
        // TinyUSB初期化
        const tinyusb_config_t tusb_cfg = {
            .device_descriptor = NULL,
            .string_descriptor = NULL,
            .external_phy = false,
            .configuration_descriptor = NULL,
        };
        ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
        
        // CDC-ACM初期化
        tinyusb_config_cdcacm_t acm_cfg = {
            .usb_dev = TINYUSB_USBDEV_0,
            .cdc_port = TINYUSB_CDC_ACM_0,
            .rx_unread_buf_sz = 64,
            .callback_rx = &tinyusb_cdc_rx_callback,
            .callback_line_coding_changed = 
                &tinyusb_cdc_line_coding_changed_cb,
        };
        ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));
    }
    
    void onOpen() override {
        LvglLockGuard lock;
        
        // UI作成
        _screen = lv_obj_create(lv_scr_act());
        lv_obj_set_size(_screen, GetHAL()->getDisplayWidth(), 
                       GetHAL()->getDisplayHeight());
        
        // タイトル
        lv_obj_t* title = lv_label_create(_screen);
        lv_label_set_text(title, "USB Serial Console");
        lv_obj_set_style_text_font(title, &lv_font_montserrat_28, 0);
        lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
        
        // コンソールテキストエリア
        _console = lv_textarea_create(_screen);
        lv_obj_set_size(_console, 1000, 500);
        lv_obj_align(_console, LV_ALIGN_CENTER, 0, 30);
        lv_textarea_set_text(_console, "USB Serial ready...\n");
        
        // ステータス
        lv_obj_t* status = lv_label_create(_screen);
        lv_label_set_text(status, "Connect via USB-C to PC");
        lv_obj_align(status, LV_ALIGN_BOTTOM_MID, 0, -10);
    }
    
    void sendData(const char* data) {
        if (tud_cdc_connected()) {
            tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, 
                                       (uint8_t*)data, 
                                       strlen(data));
            tinyusb_cdcacm_write_flush(TINYUSB_CDC_ACM_0, 0);
        }
    }
    
    void onClose() override {
        if (_screen) {
            LvglLockGuard lock;
            lv_obj_del(_screen);
            _screen = nullptr;
        }
    }
};
```

---

## 💡 USB Mass Storage (MSC) - ストレージデバイス

### 例5: USBメモリアクセス(USB Host MSC)

USB Type-AポートにUSBメモリを接続し、ファイル読み書きを行います。

```cpp
#include "usb/usb_host.h"
#include "msc_host.h"
#include "msc_host_vfs.h"
#include "esp_vfs_fat.h"

class UsbStorageApp : public mooncake::AppBase {
private:
    msc_host_device_handle_t _msc_device = NULL;
    msc_host_vfs_handle_t _vfs_handle = NULL;
    
public:
    void onCreate() override {
        setAppInfo().name = "USB Storage Manager";
        
        // USB Host初期化
        const usb_host_config_t host_config = {
            .skip_phy_setup = false,
            .intr_flags = ESP_INTR_FLAG_LEVEL1,
        };
        ESP_ERROR_CHECK(usb_host_install(&host_config));
        
        // MSC Host初期化
        const msc_host_driver_config_t msc_config = {
            .create_background_task = true,
            .task_priority = 5,
            .stack_size = 4096,
            .callback = msc_event_callback,
        };
        ESP_ERROR_CHECK(msc_host_install(&msc_config));
    }
    
    static void msc_event_callback(const msc_host_event_t *event, void *arg)
    {
        if (event->event == MSC_DEVICE_CONNECTED) {
            printf("MSC Device connected\n");
        } else if (event->event == MSC_DEVICE_DISCONNECTED) {
            printf("MSC Device disconnected\n");
        }
    }
    
    void mountUsbDrive() {
        // USBメモリをマウント
        const esp_vfs_fat_mount_config_t mount_config = {
            .format_if_mount_failed = false,
            .max_files = 5,
        };
        
        ESP_ERROR_CHECK(msc_host_vfs_register(_msc_device, 
                                              "/usb", 
                                              &mount_config, 
                                              &_vfs_handle));
        
        printf("USB Drive mounted at /usb\n");
    }
    
    void readFile(const char* filename) {
        char path[64];
        snprintf(path, sizeof(path), "/usb/%s", filename);
        
        FILE* f = fopen(path, "r");
        if (f == NULL) {
            printf("Failed to open file\n");
            return;
        }
        
        char line[128];
        while (fgets(line, sizeof(line), f)) {
            printf("%s", line);
        }
        
        fclose(f);
    }
    
    void writeFile(const char* filename, const char* data) {
        char path[64];
        snprintf(path, sizeof(path), "/usb/%s", filename);
        
        FILE* f = fopen(path, "w");
        if (f == NULL) {
            printf("Failed to create file\n");
            return;
        }
        
        fprintf(f, "%s", data);
        fclose(f);
        
        printf("File written successfully\n");
    }
    
    void onClose() override {
        // アンマウント
        if (_vfs_handle) {
            msc_host_vfs_unregister(_vfs_handle);
        }
        
        // MSC Host 終了
        msc_host_uninstall();
        usb_host_uninstall();
    }
};
```

---

## 🔧 USB使用時の注意点

### 1. **電源管理**
- USB Type-A (Host) の5V電源は IO Expander 経由で制御
- 外部デバイスへの電源供給を有効化:

```cpp
GetHAL()->setUsb5vEnable(true);  // USB-A 5V出力有効
```

### 2. **USB接続検出**
```cpp
// USB-C検出
bool usb_c_connected = GetHAL()->usbCDetect();

// USB-A検出
bool usb_a_connected = GetHAL()->usbADetect();
```

### 3. **同時使用の制限**
- USB Type-A (Host) と USB Type-C (OTG) は同時に異なるモードで使用可能
- ただし、USB Type-Cは Host/Device のどちらか一方のみ

### 4. **サポートデバイスクラス**
- **HID**: キーボード、マウス、ゲームパッド
- **MSC**: USBメモリ、外付けHDD
- **CDC-ACM**: USBシリアル、モデム
- **UVC**: USBカメラ (※ビデオストリーミング対応)

---

## 📚 応用例

1. **産業用HMI**: USB経由でバーコードスキャナー、RFIDリーダー接続
2. **データロガー**: USBメモリに測定データを自動保存
3. **リモートデスクトップ**: Tab5をキーボード・マウスとしてPC制御
4. **IoTゲートウェイ**: USBシリアル経由で外部モジュール(LTE、LoRa等)接続
5. **デジタルサイネージ**: USBメモリから画像・動画コンテンツを再生
6. **テストツール**: USBデバイスエミュレーターとして機器テスト

---

この実装ガイドを参考に、Tab5の強力なUSB機能を活用した多様なアプリケーション開発が可能です。TinyUSBスタックの柔軟性により、カスタムUSBデバイスクラスの実装にも対応できます。
