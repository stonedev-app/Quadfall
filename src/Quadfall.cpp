/**
 * @file    Quadfall.cpp
 * @brief   エントリーポイントファイル
 *
 * テトリス風落ち物パズルゲーム
 */

// ---------------------------------------------------------------------------
// Arduboy標準ライブラリ
// ---------------------------------------------------------------------------
#include <Arduino.h>
#include <Arduboy2.h>

// ---------------------------------------------------------------------------
// Quadfall固有ライブラリ
// ---------------------------------------------------------------------------
#include "game.h"       // ゲームロジック
#include "input.h"      // 入力処理
#include "renderer.h"   // 描画処理
#include "sound.h"      // サウンド処理

/** @brief Arduboy2本体オブジェクト（全モジュールで共有） */
Arduboy2 arduboy;

/**
 * @brief 初期化処理
 *
 * フレームレート設定、各種初期化処理を行う
 */
void setup()
{
    arduboy.begin();            // ハードウェア初期化・ブートロゴ表示
    arduboy.setFrameRate(60);   // フレームレートを60FPSに設定
    soundInit(arduboy);         // サウンド初期化
    gameInit();                 // ゲームロジック初期化
}

/**
 * @brief メインループ
 *
 * フレーム毎の処理を行う
 */
void loop()
{
    // 次のフレームまでは処理をスキップ
    if (!arduboy.nextFrame())
        return;

    arduboy.pollButtons();      // ボタン状態更新
    inputUpdate(arduboy);       // ボタン入力に起因する処理
    gameUpdate();               // 時間経過に起因する処理
    renderFrame(arduboy);       // 画面描画処理
}
