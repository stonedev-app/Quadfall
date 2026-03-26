#include "game.h"
#include <string.h>

// ---------------------------------------------------------------------------
// ピース形状データ (PROGMEM)
// 各エントリは4×4グリッドの16ビットマスク
// ビット15が(col=0,row=0)、ビット0が(col=3,row=3)
// ---------------------------------------------------------------------------
const uint16_t PIECES[7][4] PROGMEM = {
    // I
    { 0x0F00, 0x2222, 0x00F0, 0x4444 },
    // O
    { 0x6600, 0x6600, 0x6600, 0x6600 },
    // T
    { 0x0E40, 0x4C40, 0x4E00, 0x4640 },
    // S
    { 0x06C0, 0x4620, 0x06C0, 0x4620 },
    // Z
    { 0x0C60, 0x2640, 0x0C60, 0x2640 },
    // J
    { 0x08E0, 0x6440, 0x0E20, 0x44C0 },
    // L
    { 0x02E0, 0x4460, 0x0E80, 0xC440 },
};

// 落下間隔 (level 1〜10)
const uint8_t FALL_INTERVAL[MAX_LEVEL] PROGMEM = {
    48, 43, 38, 33, 28, 23, 18, 13, 8, 6
};

// ---------------------------------------------------------------------------
// グローバル変数
// ---------------------------------------------------------------------------
uint16_t  field[FIELD_H];
Piece     cur;
Piece     next;
uint32_t  score;
uint8_t   level;
uint16_t  linesCleared;
GameState gameState;
uint8_t   fallTimer;

// ---------------------------------------------------------------------------
// ヘルパー: ピースマスクの指定セルが立っているか
// ---------------------------------------------------------------------------
static inline bool maskBit(uint16_t mask, uint8_t col, uint8_t row) {
    return (mask >> (15 - (row * 4 + col))) & 1;
}

// ---------------------------------------------------------------------------
// 衝突判定
// ---------------------------------------------------------------------------
bool pieceCanPlace(const Piece &p) {
    uint16_t mask = pgm_read_word(&PIECES[p.type][p.rot]);
    for (uint8_t row = 0; row < 4; row++) {
        for (uint8_t col = 0; col < 4; col++) {
            if (!maskBit(mask, col, row)) continue;
            int8_t fx = p.x + col;
            int8_t fy = p.y + row;
            if (fx < 0 || fx >= FIELD_W) return false;
            if (fy >= FIELD_H) return false;
            if (fy < 0) continue; // フィールド上部は許可
            if (field[fy] & (1 << (FIELD_W - 1 - fx))) return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// ゴーストY座標
// ---------------------------------------------------------------------------
int8_t ghostY(const Piece &p) {
    Piece g = p;
    while (true) {
        g.y++;
        if (!pieceCanPlace(g)) { g.y--; break; }
    }
    return g.y;
}

// ---------------------------------------------------------------------------
// 移動
// ---------------------------------------------------------------------------
bool pieceMove(int8_t dx, int8_t dy) {
    Piece tmp = cur;
    tmp.x += dx;
    tmp.y += dy;
    if (!pieceCanPlace(tmp)) return false;
    cur = tmp;
    return true;
}

// ---------------------------------------------------------------------------
// 回転 (+1=右, -1=左)
// ---------------------------------------------------------------------------
bool pieceRotate(int8_t dir) {
    Piece tmp = cur;
    tmp.rot = (tmp.rot + 4 + dir) % 4;

    // SRS簡易キック: (0,0), (-1,0), (+1,0), (0,-1)
    const int8_t kicks[4][2] = {{0,0},{-1,0},{1,0},{0,-1}};
    for (uint8_t i = 0; i < 4; i++) {
        Piece k = tmp;
        k.x += kicks[i][0];
        k.y += kicks[i][1];
        if (pieceCanPlace(k)) { cur = k; return true; }
    }
    return false;
}

// ---------------------------------------------------------------------------
// ライン消去
// ---------------------------------------------------------------------------
static const uint16_t SCORE_TABLE[5] PROGMEM = {0, 100, 300, 500, 800};

void clearLines() {
    uint8_t cleared = 0;
    for (int8_t row = FIELD_H - 1; row >= 0; row--) {
        if (field[row] == (uint16_t)((1 << FIELD_W) - 1)) {
            // このラインを消去し、上のラインを下にシフト
            for (int8_t r = row; r > 0; r--) {
                field[r] = field[r - 1];
            }
            field[0] = 0;
            cleared++;
            row++; // 同じ行を再チェック
        }
    }
    if (cleared > 0) {
        score += (uint32_t)pgm_read_word(&SCORE_TABLE[cleared]) * level;
        linesCleared += cleared;
        if (linesCleared >= (uint16_t)level * 10 && level < MAX_LEVEL) {
            level++;
        }
    }
}

// ---------------------------------------------------------------------------
// ロック
// ---------------------------------------------------------------------------
void pieceLock() {
    uint16_t mask = pgm_read_word(&PIECES[cur.type][cur.rot]);
    for (uint8_t row = 0; row < 4; row++) {
        for (uint8_t col = 0; col < 4; col++) {
            if (!maskBit(mask, col, row)) continue;
            int8_t fy = cur.y + row;
            int8_t fx = cur.x + col;
            if (fy >= 0 && fy < FIELD_H && fx >= 0 && fx < FIELD_W) {
                field[fy] |= (1 << (FIELD_W - 1 - fx));
            }
        }
    }
    clearLines();
    spawnNext();
}

// ---------------------------------------------------------------------------
// 次ピースをスポーン
// ---------------------------------------------------------------------------
// 簡易乱数（線形合同法）
static uint16_t rng = 1;
static uint8_t nextType() {
    rng = rng * 25173 + 13849;
    return rng % 7;
}

void spawnNext() {
    cur = next;
    cur.x = (FIELD_W - 4) / 2;
    cur.y = -1;
    next.type = nextType();
    next.rot  = 0;
    next.x    = 0;
    next.y    = 0;
    fallTimer = 0;
    if (!pieceCanPlace(cur)) {
        gameState = STATE_GAMEOVER;
    }
}

// ---------------------------------------------------------------------------
// ゲーム初期化
// ---------------------------------------------------------------------------
void gameInit() {
    gameState = STATE_TITLE;
}

void gameStart() {
    memset(field, 0, sizeof(field));
    score        = 0;
    level        = 1;
    linesCleared = 0;
    fallTimer    = 0;
    rng          = (uint16_t)millis() ^ 0xA5A5;
    next.type    = nextType();
    next.rot     = 0;
    next.x       = 0;
    next.y       = 0;
    spawnNext();
    gameState    = STATE_PLAYING;
}

// ---------------------------------------------------------------------------
// ゲーム更新（落下のみ、入力はinput.cppから呼ばれる）
// ---------------------------------------------------------------------------
void gameUpdate() {
    if (gameState != STATE_PLAYING) return;
    fallTimer++;
    uint8_t interval = pgm_read_byte(&FALL_INTERVAL[level - 1]);
    if (fallTimer >= interval) {
        fallTimer = 0;
        if (!pieceMove(0, 1)) {
            pieceLock();
        }
    }
}
