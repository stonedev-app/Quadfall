以下の手順でリリース準備を行ってください。push は最後にコマンドを表示するだけにして、実行しないこと。

## 手順

### 1. 事前チェック

以下を確認し、問題があれば作業を中断してユーザーに報告する。

- 現在のブランチが `develop` であること
- `git status` で未コミットの変更がないこと
- `git log main..develop --oneline` の結果が1件以上あること（マージすべきコミットが存在すること）

### 2. バージョン番号の確認

`$ARGUMENTS` にバージョン番号（例: `v1.0.0`）が指定されていればそれを使う。
指定がなければユーザーに確認する。
`v` プレフィックスがなければ自動で付与する。

### 3. リリースノートの生成

`git log main..develop --oneline` のコミット一覧をもとに、日本語でリリースノートを作成する。

フォーマット：
```
リリース <version>

## 変更内容
- <変更点1>
- <変更点2>
...
```

生成したリリースノートをユーザーに提示し、確認を取る。修正が必要であれば対応する。

### 3.5. CHANGELOG.md を更新

`CHANGELOG.md` の先頭（既存の `## v...` の直前）に今回のリリースを追記してコミットする。

```markdown
## <version> - <today>

- <変更点1>
- <変更点2>
...
```

```bash
git add CHANGELOG.md
git commit -m "docs: CHANGELOG を <version> に更新"
```

### 4. main へマージ

```bash
git checkout main
git merge --no-ff develop -m "<リリースノート全文>"
```

### 5. タグ作成

```bash
git tag <version>
```

### 6. push コマンドを表示

以下のコマンドを表示する（実行はしない）。

```bash
git push origin main
git push origin <version>
```

「上記コマンドを実行すると GitHub Actions が起動してビルド＆リリースが作成されます。」と案内する。
push が完了したら知らせるよう伝える。

### 7. push 後の確認（push完了を受けてから実行）

以下を実行して結果を報告する。

```bash
gh run list
gh release view <version>
```

`fix #番号` / `close #番号` を含むコミットがある場合は、以下で自動クローズを確認する。

```bash
gh issue list --state closed
```

すべての確認が終わったら、develop ブランチに戻るよう案内する。

```bash
git checkout develop
```
