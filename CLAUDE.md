# CLAUDE.md

このファイルは、Claude Code (claude.ai/code) がこのリポジトリで作業する際のガイダンスを提供します。

## ビルド

ビルドシステムは **PlatformIO**（Arduino IDE ではない）。

```bash
pio run                      # ビルド
pio run --target upload      # ビルド＆Arduboy へ書き込み
pio run --target clean       # ビルド成果物を削除
```

依存ライブラリは `platformio.ini` の `lib_deps` で管理され、`.pio/libdeps/` に自動取得される。

## AVR 固有の制約

Flash / RAM の厳しいメモリ制限があるため：

- **定数データは PROGMEM に置く** → `pgm_read_word()` / `pgm_read_byte()` で読み出す
- **文字列リテラルは `F()` マクロ** → `F("SCORE")` のようにしてフラッシュに格納する

具体的な数値は `BOARD.md`、メモリ見積もりは `DESIGN.md` のメモリ見積もりセクションを参照。

## ドキュメント

- **`BOARD.md`** — ハードウェア仕様
- **`DESIGN.md`** — プロジェクトの仕様・設計詳細
