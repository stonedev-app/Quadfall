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

**ビルド検証は `.cpp` / `.h` を変更したときのみ実施する。ドキュメントのみの変更ではビルド不要。**

## AVR 固有の制約

Flash / RAM の厳しいメモリ制限があるため：

- **定数データは PROGMEM に置く** → `pgm_read_word()` / `pgm_read_byte()` で読み出す
- **文字列リテラルは `F()` マクロ** → `F("SCORE")` のようにしてフラッシュに格納する

具体的な数値は `BOARD.md`、メモリ見積もりは `DESIGN.md` のメモリ見積もりセクションを参照。

## コードレビュー

`.cpp` または `.h` ファイルを変更したとき、およびレビュー指摘を修正したときは、
必ず arduboy-reviewer エージェントでレビューを実施すること。
「重大」な指摘がゼロになるまで **修正 → 再レビュー** を繰り返すこと。

## ドキュメント

- **`BOARD.md`** — ハードウェア仕様
- **`DESIGN.md`** — プロジェクトの仕様・設計詳細

## スクリーンショット

[Ardens Player](https://tiberiusbrown.github.io/Ardens/) を使う。

1. `pio run` でビルドして `.pio/build/arduboy/firmware.hex` を生成
2. Ardens Player を開き、`firmware.hex` をドラッグ＆ドロップ
3. 目的の画面を表示した状態で `Cmd + Shift + 5` → ウィンドウのスクリーンショットを撮影
4. `docs/` に保存して README.md から参照

以下のような変更があった場合は、スクリーンショットの撮り直しと README.md の更新をユーザーに提案すること：

- 画面レイアウトが変わった（UI の位置・表示内容の変更）
- 新しい画面が追加された（新ステートの追加など）
- 操作方法が変わった（コントロール表の更新が必要な場合）

## ブランチ戦略とリリース

- 開発は常に `develop` ブランチで行う。`main` へは直接コミットしない
- `main` へのマージは `/release` コマンドで行う
- 会話の中で実装が一段落したとき、かつ `git log main..develop --oneline` が5件以上あれば、リリースを提案すること

## スラッシュコマンド

| コマンド | 内容 |
|----------|------|
| `/release` | develop→main マージ、CHANGELOG 更新、リリースノート生成、タグ作成 |
