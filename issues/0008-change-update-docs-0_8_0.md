# README とコントロール一覧とスキルを libcamera v0.8.0 に更新する

- Created: 2026-11-13
- Completed: {YYYY-MM-DD}
- Branch: feature/update-docs-0_8_0
- Polished: 2026-11-13

## 目的

本リポジトリは `README.md` の「対応 libcamera バージョン」で動的リンク先のバージョンを明記しており、
`docs/CONTROLS.md` で全コントロールの一覧と件数を公開している。
libcamera v0.8.0+rpt20261104 へ追従した後もこれらが v0.7.2 のまま残ると、利用者は対応バージョンを取り違え、
一覧の件数と実装が食い違う。

また `skills/shiguredo-libcamera/SKILL.md` は 2026.1.0 時点の内容で更新が止まっており、
対応 libcamera を v0.7.0+rpt20260205 と記載したままである。
さらに、`ControlId` のシグネチャ変更、コントロール定数とプロパティ定数の追加削除、
コントロール値の設定 API の変更が他の issue で行われるため、スキルの記述もそれに追随させる必要がある。

本 issue は、これらのドキュメントを実装の現在の状態に合わせるものである。

## 依存関係

本 issue はドキュメントの転記を扱うため、次の issue の完了後に着手する。

- コントロール値の設定 API の変更 (`ControlList::set_*` の `Result` 化、`Error::ControlSetFailed` の追加、`owned` から `writable` への改名)
- `ControlId` への vendor / 配列長 / 列挙子の追加と `ControlId::new` のシグネチャ変更
- コントロール定数の追加と rpi の ID 是正
- プロパティ定数の追加と削除

完了時点の `src/control_ids.rs`、`src/property_ids.rs`、`src/controls.rs`、`src/error.rs` を正本とし、
その内容をドキュメントに転記する。

**libcamera v0.8.0 自体には新しいコントロールもプロパティも追加されていない。**
コントロールの件数が変わるのは、v0.7.2 追従時に取りこぼした分の補完によるものである。

## 現状

v0.8.0 追従の影響を受けるドキュメントと、現在の記述は次のとおり。

- `README.md`: 「対応 libcamera バージョン」に `libcamera v0.7.2+rpt20261009` と記載されている
- `docs/CONTROLS.md`: 冒頭に `libcamera v0.7.2+rpt20261009 の全コントロール (80 件) に対応しています。` と記載され、
  `## Core (52 件)` / `## Draft (14 件)` / `## Raspberry Pi (14 件)` の 3 章で構成されている。
  プロパティの記載は無い
- `skills/shiguredo-libcamera/SKILL.md`: バージョンが 2026.1.0、対応 libcamera が `v0.7.0+rpt20260205` と記載されている
- `CHANGES.md`: `develop` セクションが空であり、v0.8.0 追従の記載が無い

### スキルで陳腐化する記述

`skills/shiguredo-libcamera/SKILL.md` には、他の issue の変更によって陳腐化する記述が次のとおり存在する。

| 現在の記述 | 陳腐化する理由 |
|---|---|
| `- バージョン: 2026.1.0` | `Cargo.toml` のバージョンは 2026.2.0 |
| `- 対応 libcamera: **v0.7.0+rpt20260205**` | 対応バージョンが v0.8.0+rpt20261104 になる |
| `v0.7.0+rpt20260205 系を想定している` (既知の制限事項) | 同上 |
| `controls()` / `metadata()` で返る `ControlList` は非所有 (`owned = false`) | `owned` が `writable` に改名される |
| `戻り値なし (libcamera 側でバリデーション)。` (設定の節) | `set_*` が `Result<()>` を返すようになる |
| `set_*` メソッドは Result を返さない (既知の制限事項) | 同上 |
| エラー型表に `ControlSetFailed` が無い | `Error::ControlSetFailed` が追加される |
| `ControlId::new(id, name, control_type, direction)` | `ControlId::new` のシグネチャが変わる |
| `core` の `ID 1..52` | `Hue` の追加により 53 件になる |
| `properties::core` に `PIXEL_ARRAY_SIZE` | `PIXEL_ARRAY_SIZE` が削除され、別の定数に置き換わる |
| `props_list.get_size(&props::PIXEL_ARRAY_SIZE)` (コード例) | 同上。`get_size` ではなく `get_rectangle` を使う形になる |
| `controls.set_f32(&core::BRIGHTNESS, 0.2)` (コード例) | `set_*` が `Result<()>` を返すようになる |

## 設計方針

### 変更対象

`README.md`、`docs/CONTROLS.md`、`skills/shiguredo-libcamera/SKILL.md`、`CHANGES.md` を変更する。
コードは変更しない。
`docs/CONTAINER.md` は実機検証の issue が扱うため、本 issue では変更しない。

### 対応 libcamera バージョン

記載するバージョンは `libcamera v0.8.0+rpt20261104` とし、Raspberry Pi のリリースタグを正本とする。

### `docs/CONTROLS.md`

- 冒頭のバージョンと件数を、完了時点の `src/control_ids.rs` と `src/property_ids.rs` に合わせる
- 冒頭の件数は、コントロールとプロパティを区別して書く
- 完了時点でコントロールは core 53 / draft 14 / rpi 15 の合計 82 件になる
- 新規に記載するのは `Hue` (core、ID 53) と `ControlListSequence` (rpi、ID 20009) の 2 件である
- rpi の CNN 系 6 件は ID が変わるため、既存の記載の ID を書き換える。件数は増えない
- 末尾にプロパティの章を新設し、`src/property_ids.rs` の定数を記載する。
  `ColorFilterArrangement` を含め、これまで記載されていなかった分をすべて記載する
- 各項目の書式は既存の記載に合わせる。見出しに名前と ID、続けて定数名・型・方向・説明を書き、
  列挙子を持つものは表で記載する
- 説明文は既存の記載と同様に日本語で書く

### `skills/shiguredo-libcamera/SKILL.md`

次の節を更新する。

- バージョン情報: バージョンを `Cargo.toml` の値に合わせ、対応 libcamera を v0.8.0+rpt20261104 にする
- 既知の制限事項: libcamera のバージョンに関する記述と、`set_*` が `Result` を返さないという記述を更新する
- `ControlList`: `set_*` が `Result<()>` を返すこと、`owned` を `writable` に読み替えることを反映する
- エラー型: `ControlSetFailed` を追加する
- `ControlId`: `ControlId::new` のシグネチャと、追加される getter を反映する
- コントロール定数: `core` の ID の範囲と、rpi の CNN 系の ID を更新する
- プロパティ定数: 削除される定数と追加される定数に合わせて一覧を差し替える
- コード例: プロパティの読み取り例を `get_rectangle` を使う形にし、コントロールの設定例を `Result` を処理する形にする
- 動作環境: `Dev Container セットアップガイドを参照` という記述は、リポジトリに該当する文書が無いため、
  `README.md` の Dev Containers の節を参照する形に正す。`docs/CONTAINER.md` は実機検証の issue が扱うため変更しない

### `CHANGES.md`

`shiguredo-changelog` スキルの規約に従い、`develop` セクションに
`[UPDATE] 対応 libcamera を v0.8.0+rpt20261104 に更新する` の 1 件だけを担当者行付きで追記する。

定数の追加削除、`set_*` の `Result` 化、`ControlId` の変更は、それぞれの issue が自身で記載するため、
本 issue では重複して記載しない。
`README.md`、`docs/CONTROLS.md`、`skills/shiguredo-libcamera/SKILL.md` の変更は、
`shiguredo-changelog` の規約に従い変更履歴には記載しない。

## 完了条件

- `README.md` の対応 libcamera バージョンが `v0.8.0+rpt20261104` になっている
- `docs/CONTROLS.md` の冒頭のバージョンと件数が、`src/control_ids.rs` と `src/property_ids.rs` の定数数と一致している
- `docs/CONTROLS.md` に `Hue` と `ControlListSequence` が記載されている
- `docs/CONTROLS.md` の rpi の CNN 系の ID が `src/control_ids.rs` と一致している
- `docs/CONTROLS.md` にプロパティの章があり、`src/property_ids.rs` の定数がすべて記載されている
- `skills/shiguredo-libcamera/SKILL.md` のバージョンが `Cargo.toml` の値と一致し、対応 libcamera が `v0.8.0+rpt20261104` になっている
- `skills/shiguredo-libcamera/SKILL.md` に、次の文字列が残っていない
  - `2026.1.0`
  - `v0.7.0+rpt20260205`
  - `owned`
  - `PIXEL_ARRAY_SIZE`
  - `ID 1..52`
- `skills/shiguredo-libcamera/SKILL.md` に `ControlSetFailed` が記載されている
- `skills/shiguredo-libcamera/SKILL.md` のコード例が、完了時点の公開 API で動作する形になっている
- `CHANGES.md` の `develop` セクションに `[UPDATE]` が担当者行付きで 1 件だけ記載されている
- `prek run --all-files` が通る (行末の空白、ファイル末尾の改行、改行コードの検査を含む)

## 解決方法

- `README.md` の「対応 libcamera バージョン」の引用ブロックを更新する
- `docs/CONTROLS.md` の冒頭と各章の件数を更新し、`Hue` と `ControlListSequence` を追記し、
  rpi の CNN 系の ID を書き換え、末尾にプロパティの章を新設する
- `skills/shiguredo-libcamera/SKILL.md` の「設計方針」に挙げた各節を更新する
- `CHANGES.md` の `develop` セクションに `[UPDATE]` を 1 件追記する
- 全角と半角の間に半角スペースを入れる規約に従う
