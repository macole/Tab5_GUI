/*
 * EEZ_withFlow04_Clock - 設定ファイル
 * M5Stack Tab5用のLVGL時計アプリケーション設定
 */

#pragma once

// ============================================================================
// ディスプレイ設定
// ============================================================================
// Tab5の物理ディスプレイサイズ（LVGLでソフトウェア回転を使用）
// 物理ディスプレイ: 720x1280（縦）
// LVGLでLV_DISP_ROT_90を使用して横向き（1280x720）に回転

// 注意: pins_config.hでEXAMPLE_LCD_H_RES/EXAMPLE_LCD_V_RESとして定義されています
// このファイルの定義は参考用です（実際にはpins_config.hの定義が使用されます）
#define LCD_HORIZONTAL_RES  720   // 物理横解像度（ピクセル）
#define LCD_VERTICAL_RES    1280  // 物理縦解像度（ピクセル）

// LVGL描画バッファのサイズ（全画面分のバッファを確保）
#define LVGL_LCD_BUF_SIZE   (LCD_HORIZONTAL_RES * LCD_VERTICAL_RES)

// 送信バッファのサイズ（必要に応じて調整）
#define SEND_BUF_SIZE       (LCD_HORIZONTAL_RES * 10)
 
 