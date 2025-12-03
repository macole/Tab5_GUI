# M5Stack Tab5でGPIOを使用するアプリケーション例

M5Stack Tab5には複数のGPIO拡張インターフェースが用意されており、外部デバイスとの接続が可能です。以下、具体的な実装例を解説します。

---

## 📌 Tab5のGPIO拡張インターフェース

Tab5には以下のGPIO拡張ポートがあります:

### 1. **HY2.0-4P ポート (PORT.A)**
| ピン | 色 | 機能 |
|------|-----|------|
| 1 | 黒(Black) | GND |
| 2 | 赤(Red) | 5V |
| 3 | 黄(Yellow) | GPIO 53 |
| 4 | 白(White) | GPIO 54 |

### 2. **M5-Bus 拡張コネクタ**
背面に30ピンのM5-Busコネクタを搭載。以下の主要GPIOが使用可能:
- **GPIO**: G2, G3, G4, G16, G35, G45, G47, G48, G51, G52
- **SPI**: MOSI(G18), MISO(G19), SCK(G5)
- **UART**: TXD0(G37), RXD0(G38)
- **I2C**: SDA(G31), SCL(G32) ※内部デバイスと共用
- **電源**: 3.3V, 5V, HVIN, BAT

### 3. **GPIO_EXT 拡張バス**
10ピンの拡張ヘッダー(2.54mmピッチ)

---

## 💡 GPIO使用アプリケーション例1: LEDコントロール

HY2.0-4PポートのGPIO 53/54を使用してLEDを制御する例です。

### Arduino実装例

```cpp
#include <M5Unified.h>

// HY2.0-4P GPIO定義
#define LED_PIN_1 53  // 黄色ピン
#define LED_PIN_2 54  // 白ピン

void setup() {
  M5.begin();
  
  // GPIOピンを出力モードに設定
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  
  // 初期状態はOFF
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, LOW);
  
  M5.Display.setRotation(3);
  M5.Display.setTextSize(2);
  M5.Display.clear();
  M5.Display.setCursor(10, 10);
  M5.Display.println("GPIO LED Control");
  M5.Display.println("Touch to toggle LED");
}

void loop() {
  M5.update();
  
  // タッチで交互にLEDを点滅
  if (M5.Touch.getCount()) {
    static bool led_state = false;
    led_state = !led_state;
    
    digitalWrite(LED_PIN_1, led_state);
    digitalWrite(LED_PIN_2, !led_state);
    
    M5.Display.fillRect(10, 100, 300, 50, BLACK);
    M5.Display.setCursor(10, 100);
    M5.Display.printf("LED1: %s, LED2: %s", 
                      led_state ? "ON" : "OFF",
                      !led_state ? "ON" : "OFF");
    
    delay(300);  // デバウンス
  }
  
  delay(50);
}
```

### ESP-IDF実装例(HAL使用)

```cpp
#include <driver/gpio.h>
#include <hal/hal.h>

class GpioLedApp : public mooncake::AppBase {
private:
    static const gpio_num_t LED_PIN_1 = GPIO_NUM_53;
    static const gpio_num_t LED_PIN_2 = GPIO_NUM_54;
    bool _led_state = false;
    lv_obj_t* _screen = nullptr;
    lv_obj_t* _status_label = nullptr;
    
public:
    void onCreate() override {
        setAppInfo().name = "GPIO LED Demo";
        
        // GPIO初期化
        gpio_config_t io_conf = {};
        io_conf.intr_type = GPIO_INTR_DISABLE;
        io_conf.mode = GPIO_MODE_OUTPUT;
        io_conf.pin_bit_mask = (1ULL << LED_PIN_1) | (1ULL << LED_PIN_2);
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        gpio_config(&io_conf);
        
        // 初期状態
        gpio_set_level(LED_PIN_1, 0);
        gpio_set_level(LED_PIN_2, 0);
    }
    
    void onOpen() override {
        LvglLockGuard lock;
        
        // UI作成
        _screen = lv_obj_create(lv_scr_act());
        lv_obj_set_size(_screen, GetHAL()->getDisplayWidth(), 
                       GetHAL()->getDisplayHeight());
        
        // タイトルラベル
        lv_obj_t* title = lv_label_create(_screen);
        lv_label_set_text(title, "GPIO LED Control");
        lv_obj_set_style_text_font(title, &lv_font_montserrat_32, 0);
        lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);
        
        // トグルボタン
        lv_obj_t* btn = lv_btn_create(_screen);
        lv_obj_set_size(btn, 200, 80);
        lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_event_cb(btn, button_event_cb, LV_EVENT_CLICKED, this);
        
        lv_obj_t* btn_label = lv_label_create(btn);
        lv_label_set_text(btn_label, "Toggle LED");
        lv_obj_center(btn_label);
        
        // ステータスラベル
        _status_label = lv_label_create(_screen);
        lv_label_set_text(_status_label, "LED: OFF");
        lv_obj_set_style_text_font(_status_label, &lv_font_montserrat_24, 0);
        lv_obj_align(_status_label, LV_ALIGN_BOTTOM_MID, 0, -50);
    }
    
    static void button_event_cb(lv_event_t* e) {
        GpioLedApp* app = (GpioLedApp*)lv_event_get_user_data(e);
        app->toggleLED();
    }
    
    void toggleLED() {
        LvglLockGuard lock;
        _led_state = !_led_state;
        
        // LEDを交互に点灯
        gpio_set_level(LED_PIN_1, _led_state);
        gpio_set_level(LED_PIN_2, !_led_state);
        
        // ステータス更新
        char status[64];
        snprintf(status, sizeof(status), "LED1: %s, LED2: %s",
                _led_state ? "ON" : "OFF",
                !_led_state ? "ON" : "OFF");
        lv_label_set_text(_status_label, status);
    }
    
    void onClose() override {
        // GPIO OFF
        gpio_set_level(LED_PIN_1, 0);
        gpio_set_level(LED_PIN_2, 0);
        
        if (_screen) {
            LvglLockGuard lock;
            lv_obj_del(_screen);
            _screen = nullptr;
        }
    }
};
```

---

## 💡 GPIO使用アプリケーション例2: ボタン入力読み取り

外部ボタンをHY2.0-4PポートのGPIO 53に接続して読み取る例です。

### Arduino実装例

```cpp
#include <M5Unified.h>

#define BUTTON_PIN 53  // HY2.0-4P 黄色ピン

int button_count = 0;

void setup() {
  M5.begin();
  
  // GPIOピンを入力モード(プルアップ)に設定
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  M5.Display.setRotation(3);
  M5.Display.setTextSize(3);
  M5.Display.clear();
  M5.Display.setCursor(10, 10);
  M5.Display.println("GPIO Button Demo");
}

void loop() {
  M5.update();
  
  // ボタンが押された(LOW)を検出
  static bool last_state = HIGH;
  bool current_state = digitalRead(BUTTON_PIN);
  
  if (last_state == HIGH && current_state == LOW) {
    button_count++;
    
    M5.Display.fillRect(0, 100, M5.Display.width(), 100, BLACK);
    M5.Display.setCursor(10, 120);
    M5.Display.printf("Button pressed: %d", button_count);
    
    // ビープ音(オプション)
    M5.Speaker.tone(2000, 100);
  }
  
  last_state = current_state;
  delay(50);  // デバウンス
}
```

---

## 💡 GPIO使用アプリケーション例3: センサー読み取り(アナログ入力)

ESP32-P4にはADCが内蔵されているため、アナログセンサーの読み取りも可能です。

### 温度センサー読み取り例(M5-Bus使用)

```cpp
#include <M5Unified.h>
#include <driver/adc.h>

// M5-BusのGPIO 45を使用
#define SENSOR_PIN 45

void setup() {
  M5.begin();
  
  // ADC設定(ESP-IDF API使用)
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
  
  M5.Display.setRotation(3);
  M5.Display.setTextSize(2);
  M5.Display.clear();
  M5.Display.setCursor(10, 10);
  M5.Display.println("Analog Sensor Reading");
}

void loop() {
  M5.update();
  
  // ADC読み取り(0-4095)
  int adc_value = adc1_get_raw(ADC1_CHANNEL_0);
  
  // 電圧に変換(0-3.3V)
  float voltage = (adc_value / 4095.0) * 3.3;
  
  // 画面表示
  M5.Display.fillRect(10, 100, 400, 150, BLACK);
  M5.Display.setCursor(10, 100);
  M5.Display.printf("ADC Value: %d\n", adc_value);
  M5.Display.printf("Voltage: %.2f V\n", voltage);
  
  delay(500);
}
```

---

## 💡 GPIO使用アプリケーション例4: PWM出力(モーター制御)

M5-BusのGPIOを使用してPWM信号を出力し、モーターやサーボを制御します。

### サーボモーター制御例

```cpp
#include <M5Unified.h>
#include <driver/ledc.h>

// M5-BusのGPIO 48を使用
#define SERVO_PIN 48

// PWM設定
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT
#define LEDC_FREQUENCY          (50)  // 50Hz for servo

void setup() {
  M5.begin();
  
  // LEDCタイマー設定
  ledc_timer_config_t ledc_timer = {
    .speed_mode       = LEDC_MODE,
    .duty_resolution  = LEDC_DUTY_RES,
    .timer_num        = LEDC_TIMER,
    .freq_hz          = LEDC_FREQUENCY,
    .clk_cfg          = LEDC_AUTO_CLK
  };
  ledc_timer_config(&ledc_timer);
  
  // LEDCチャンネル設定
  ledc_channel_config_t ledc_channel = {
    .gpio_num       = SERVO_PIN,
    .speed_mode     = LEDC_MODE,
    .channel        = LEDC_CHANNEL,
    .intr_type      = LEDC_INTR_DISABLE,
    .timer_sel      = LEDC_TIMER,
    .duty           = 0,
    .hpoint         = 0
  };
  ledc_channel_config(&ledc_channel);
  
  M5.Display.setRotation(3);
  M5.Display.setTextSize(2);
  M5.Display.clear();
  M5.Display.println("Servo Control Demo");
}

void setServoAngle(int angle) {
  // 0-180度を0.5-2.5msパルス幅に変換
  // 50Hz = 20ms周期、13bit = 8192段階
  int duty = (angle * 8192 / 180) * 2 / 20 + 205;
  ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
  ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

void loop() {
  M5.update();
  
  // 0度→90度→180度→90度を繰り返し
  for (int angle = 0; angle <= 180; angle += 5) {
    setServoAngle(angle);
    
    M5.Display.fillRect(10, 100, 300, 50, BLACK);
    M5.Display.setCursor(10, 100);
    M5.Display.printf("Angle: %d degrees", angle);
    
    delay(50);
  }
  
  for (int angle = 180; angle >= 0; angle -= 5) {
    setServoAngle(angle);
    
    M5.Display.fillRect(10, 100, 300, 50, BLACK);
    M5.Display.setCursor(10, 100);
    M5.Display.printf("Angle: %d degrees", angle);
    
    delay(50);
  }
}
```

---

## 🔧 GPIO使用時の注意点

### 1. **電圧レベル**
- ESP32-P4のGPIOは**3.3V**ロジック
- 5V信号を入力する場合はレベルシフターが必要

### 2. **電流制限**
- 各GPIOピンの最大電流: **40mA**
- LEDを直接駆動する場合は電流制限抵抗(330Ω〜1kΩ)を使用

### 3. **IO Expander制御**
- 一部の電源制御はIO Expander(PI4IOE5V6408)経由で制御
- EXT5V_ENで外部5V電源を制御可能

```cpp
// 外部5V電源を有効化
GetHAL()->setExt5vEnable(true);

// M5-Bus, HY2.0-4P, GPIO_EXTに5V電源供給
```

### 4. **使用可能なGPIO確認**
以下のGPIOは内部デバイスで使用済み:
- **G31, G32**: I2C(IMU, RTC, INA226など)
- **G22**: LCD バックライト
- **G5, G18, G19**: SPI(SD カード可)
- **G37, G38**: UART0

---

## 📚 実装のベストプラクティス

### 1. **HAL抽象化の活用**
```cpp
// HAL経由でアクセス
int width = GetHAL()->getDisplayWidth();
GetHAL()->setDisplayBrightness(80);
```

### 2. **LVGL操作のロック**
```cpp
void update() {
    LvglLockGuard lock;  // 自動ロック/アンロック
    lv_label_set_text(label, "Updated");
}
```

### 3. **割り込み処理**
```cpp
// GPIO割り込み設定
gpio_install_isr_service(0);
gpio_isr_handler_add(GPIO_NUM_53, gpio_isr_handler, (void*)GPIO_NUM_53);
```

---

## 🎯 応用例

1. **環境監視システム**: 温湿度センサー、CO2センサーをHY2.0-4Pに接続
2. **産業制御**: RS-485経由でPLC、モーター制御
3. **ホームオートメーション**: リレーモジュールで家電制御
4. **ロボット制御**: M5-Bus経由で複数のサーボモーター制御
5. **データロガー**: GPIOでセンサー読み取り→SDカードに保存

---

これらの例を参考に、Tab5の豊富なGPIO拡張機能を活用した独自のアプリケーション開発が可能です。HubにあるLVGL実装例も併せて参照することで、より洗練されたUIを持つGPIOアプリケーションを構築できます。
