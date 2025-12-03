# M5Tab5 Drawing Camera App

Interactive drawing and camera application for [M5Tab5](https://docs.m5stack.com/en/products/sku/k145) featuring real-time drawing with camera integration.

## Features

- **Drawing Canvas**: Touch-based drawing with customizable brush colors and sizes
- **Camera Integration**: Live camera preview and photo capture functionality  
- **Dual-Platform**: Runs on both desktop (SDL2) and ESP32-P4 hardware
- **Hardware Testing**: Comprehensive hardware validation through app launcher

## Build

### Fetch Dependencies

```bash
python ./fetch_repos.py
```

## Desktop Build

#### Tool Chains

```bash
sudo apt install build-essential cmake libsdl2-dev
```

#### Build

```bash
mkdir build && cd build
```

```bash
cmake .. && make -j8
```

#### Run

```bash
./desktop/app_desktop_build
```

## IDF Build

#### Tool Chains

[ESP-IDF v5.4.1](https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32s3/index.html) または [ESP-IDF v5.5.1](https://docs.espressif.com/projects/esp-idf/en/latest/esp32p4/index.html)

**注意**: ESP-IDF v5.5.1でも動作確認済みです。

#### 前提条件

1. ESP-IDFがインストールされていること
2. ESP-IDF環境がセットアップされていること（`export.sh`が実行可能）

#### セットアップ手順

1. **依存関係の取得**（初回のみ、または依存関係が更新された場合）

```bash
cd /Users/masato/Documents/Arduino/tmp/DrawingCameraApp
python3 fetch_repos.py
```

2. **ESP-IDF環境のセットアップ**

```bash
# ESP-IDF環境をアクティブ化（例: v5.5.1の場合）
source ~/esp/v5.5.1/esp-idf/export.sh

# または、v5.4.1の場合
# source ~/esp/esp-idf/export.sh
```

3. **プロジェクトディレクトリに移動**

```bash
cd platforms/tab5
```

#### Build

```bash
idf.py build
```

ビルドが成功すると、以下のメッセージが表示されます：
```
Project build complete. To flash, run:
 idf.py flash
```

#### Flash（転送）

1. **M5Tab5をUSBケーブルでPCに接続**

2. **シリアルポートの確認**（オプション）

```bash
idf.py flash -p /dev/cu.usbmodemXXXXX
```

ポートを指定しない場合、自動的に検出されます。

3. **転送実行**

```bash
idf.py flash
```

転送が成功すると、以下のようなメッセージが表示されます：
```
Wrote 2814448 bytes at 0x00010000 in 11.5 seconds
Hash of data verified.
Leaving...
Hard resetting via RTS pin...
Done
```

#### Monitor（シリアルモニター）

転送後、シリアルモニターで動作を確認できます：

```bash
idf.py monitor
```

終了するには `Ctrl+]` を押します。

#### トラブルシューティング

- **ESP-IDFが見つからない場合**:
  - `export.sh`のパスを確認してください
  - ESP-IDFが正しくインストールされているか確認してください

- **ビルドエラーが発生した場合**:
  - 依存関係が正しく取得されているか確認: `python3 fetch_repos.py`
  - クリーンビルドを試す: `idf.py fullclean && idf.py build`

- **転送エラーが発生した場合**:
  - USBケーブルが正しく接続されているか確認
  - シリアルポートが正しいか確認: `idf.py flash -p PORT`
  - デバイスが正しく認識されているか確認: `ls /dev/cu.*`

## Acknowledgments

This project is based on the official [M5Stack M5Tab5-UserDemo](https://github.com/m5stack/M5Tab5-UserDemo) repository and has been extended with drawing and camera integration features.

### Original Source
- **M5Stack M5Tab5-UserDemo**: https://github.com/m5stack/M5Tab5-UserDemo - Official user demo for M5Tab5 hardware evaluation

### Libraries and Dependencies
This project references the following open-source libraries and resources:
https://protopedia.net/prototype/6776
- https://github.com/lvgl/lvgl
- https://www.heroui.com
- https://github.com/Forairaaaaa/smooth_ui_toolkit
- https://github.com/Forairaaaaa/mooncake
- https://github.com/Forairaaaaa/mooncake_log
- https://github.com/alexreinert/piVCCU/blob/master/kernel/rtc-rx8130.c
- https://components.espressif.com/components/espressif/esp_cam_sensor
- https://components.espressif.com/components/espressif/esp_ipa
- https://components.espressif.com/components/espressif/esp_sccb_intf
- https://components.espressif.com/components/espressif/esp_video
- https://components.espressif.com/components/espressif/esp_lvgl_port
- https://github.com/jarzebski/Arduino-INA226
- https://github.com/boschsensortec/BMI270_SensorAPI
