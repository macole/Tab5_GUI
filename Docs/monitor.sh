#!/bin/bash
# Arduino CLI シリアルモニタースクリプト

# 設定ファイルのパス
CONFIG_FILE="/Users/masato/Documents/Arduino/arduino-cli-config.yaml"

# Arduino CLIのパス
ARDUINO_CLI="/opt/homebrew/bin/arduino-cli"

# デフォルトポート
DEFAULT_PORT="/dev/cu.usbmodem21201"

# 引数チェック
PORT="${1:-$DEFAULT_PORT}"

echo "=== Arduino CLI シリアルモニター ==="
echo "ポート: $PORT"
echo "ボーレート: 115200"
echo ""
echo "終了するには Ctrl+C を押してください"
echo ""

# シリアルモニター実行
$ARDUINO_CLI --config-file "$CONFIG_FILE" monitor \
    --port "$PORT" \
    --config baudrate=115200

