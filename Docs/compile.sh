#!/bin/bash
# Arduino CLI コンパイルスクリプト

# 設定ファイルのパス
CONFIG_FILE="/Users/masato/Documents/Arduino/arduino-cli-config.yaml"

# Arduino CLIのパス
ARDUINO_CLI="/opt/homebrew/bin/arduino-cli"

# 引数チェック
if [ $# -eq 0 ]; then
    echo "使用方法: $0 <スケッチファイル>"
    echo "例: $0 BarGraph/BarGraph.ino"
    exit 1
fi

SKETCH_FILE="$1"

# ファイルの存在確認
if [ ! -f "$SKETCH_FILE" ]; then
    echo "エラー: ファイル '$SKETCH_FILE' が見つかりません"
    exit 1
fi

echo "=== Arduino CLI コンパイル ==="
echo "スケッチ: $SKETCH_FILE"
echo "ボード: ESP32-P4"
echo ""

# コンパイル実行
$ARDUINO_CLI --config-file "$CONFIG_FILE" compile \
    --fqbn esp32:esp32:esp32p4 \
    "$SKETCH_FILE"

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ コンパイル成功！"
else
    echo ""
    echo "❌ コンパイル失敗"
    exit 1
fi

