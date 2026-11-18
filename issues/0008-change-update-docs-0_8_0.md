# README とコントロール一覧とスキルを libcamera v0.8.0 に更新する

- Created: 2026-11-13
- Completed: {YYYY-MM-DD}
- Branch: feature/update-docs-0_8_0
- Polished: 2026-11-18

## 目的

本リポジトリは `README.md` の「対応 libcamera バージョン」で動的リンク先のバージョンを明記しており、
`docs/CONTROLS.md` で全コントロールの一覧と件数を公開している。
libcamera v0.8.0+rpt20261104 へ追従した後もこれらが v0.7.2 のまま残ると、利用者は対応バージョンを取り違え、
一覧の件数と実装が食い違う。

また `skills/shiguredo-libcamera/SKILL.md` は 2026.1.0 時点の内容で更新が止まっており、
対応 libcamera を v0.7.0+rpt20260205 と記載したままである。
さらに、`ControlId` のシグネチャ変更と、コントロール定数とプロパティ定数の追加削除が他の issue で行われるため、
スキルの記述もそれに追随させる必要がある。

本 issue は、これらのドキュメントを実装の現在の状態に合わせるものである。

## 依存関係

本 issue はドキュメントの転記を扱うため、次の issue の完了後に着手する。

- `ControlId` への vendor / 配列長 / 列挙子の追加と `ControlId::new` のシグネチャ変更
- コントロール定数の追加と rpi の ID 是正
- プロパティ定数の追加と削除

完了時点の `src/control_ids.rs`、`src/property_ids.rs`、`src/controls.rs`、`src/error.rs` を正本とし、
その内容をドキュメントに転記する。

**libcamera v0.8.0 自体には新しいコントロールもプロパティも追加されていない。**
件数が変わるのは、コントロールは v0.7.2 追従時に取りこぼした分の補完によるもの、
プロパティは同じく v0.7.2 追従時に取りこぼした分の追加と、不要になった定数の削除によるものである。

### ビルドに使う libcamera のバージョン

`sysroot/raspberry-pi-os_armv8.json` の `packages` は `libcamera-dev` をバージョン指定なしで並べており、
`cargo shiguredo-sysroot` が Raspberry Pi アーカイブから解決したものをそのまま使う。
つまり本 issue がドキュメントに書くバージョンは、ビルド時のヘッダーのバージョンをリポジトリ側で固定するものではない。

本 issue では `sysroot/raspberry-pi-os_armv8.json` を変更しない。
ビルドされるヘッダーのバージョンと、実機で動かす共有ライブラリのバージョンの確認は、
実機検証の手順を整備する issue が扱う。

### 本 issue の範囲外とする変更

`ControlList::set_*` を `Result<()>` に変更し、書き込みを想定していないリストへの設定を失敗させる変更は、
後方互換のない変更であるため、v0.8.0 追従とは別の issue として保留している。
したがって本 issue では、`skills/shiguredo-libcamera/SKILL.md` の次の記述を現行のまま維持する。

- `ControlList` の設定 (`set_*`) の節にある戻り値の記述
- 既知の制限事項にある `set_*` メソッドが `Result` を返さないという記述
- `Request` と `CompletedRequest` の節にある非所有 (`owned = false`) の記述
- エラー型の表

これらは `set_*` の `Result<()>` 化、`owned` の `writable` への改名、`Error::ControlSetFailed` の追加を行った後に更新する。

## 現状

v0.8.0 追従の影響を受けるドキュメントと、現在の記述は次のとおり。

- `README.md`: 「対応 libcamera バージョン」に `libcamera v0.7.2+rpt20261009` と記載されている
- `docs/CONTROLS.md`: 冒頭に `libcamera v0.7.2+rpt20261009 の全コントロール (80 件) に対応しています。` と記載され、
  `## Core (52 件)` / `## Draft (14 件)` / `## Raspberry Pi (14 件)` の 3 章で構成されている。
  プロパティの記載は無い
- `skills/shiguredo-libcamera/SKILL.md`: バージョンが 2026.1.0、対応 libcamera が `v0.7.0+rpt20260205` と記載されている
- `CHANGES.md`: `develop` セクションにはコントロール定数とプロパティ定数の追加削除、`ControlId` のメタデータ追加、
  不具合修正の記載が既にある。対応 libcamera の更新の記載だけが無い

### スキルで陳腐化する記述

`skills/shiguredo-libcamera/SKILL.md` には、実装と合わなくなっている記述が次のとおり存在する。

| 現在の記述 | 更新する理由 |
|---|---|
| `- バージョン: 2026.1.0` | `Cargo.toml` のバージョンは 2026.2.0 |
| `- 対応 libcamera: **v0.7.0+rpt20260205**` | 対応バージョンが v0.8.0+rpt20261104 になる |
| `v0.7.0+rpt20260205 系を想定している` (既知の制限事項) | 同上 |
| `ControlId::new(id, name, control_type, direction)` | `ControlId::new` のシグネチャが変わる |
| `core` の `ID 1..52` | `Hue` の追加により `ID 1..53` になる |
| `properties::core` の一覧に `PIXEL_ARRAY_SIZE` | `PIXEL_ARRAY_SIZE` が削除され、別の定数に置き換わる |
| `props_list.get_size(&props::PIXEL_ARRAY_SIZE)` (コード例) | 同上。`get_size` ではなく `get_rectangle` を使う形になる |
| `Dev Container セットアップガイドを参照` (動作環境) | 該当する文書がリポジトリに存在しない |

## 設計方針

### 変更対象

`README.md`、`docs/CONTROLS.md`、`skills/shiguredo-libcamera/SKILL.md`、`CHANGES.md` を変更する。
コードは変更しない。
`docs/CONTAINER.md` は実機検証の issue が扱うため、本 issue では変更しない。
なお `docs/CONTAINER.md` の実機手順は libcamera 0.8 系を前提にするため、
実機検証の issue が先にマージされている場合は、完了時点でドキュメント間のバージョン表記が矛盾しないことを確認する。

### 対応 libcamera バージョン

記載するバージョンは `libcamera v0.8.0+rpt20261104` とし、Raspberry Pi のリリースタグを正本とする。

### `docs/CONTROLS.md`

- 冒頭のバージョンと件数を、完了時点の `src/control_ids.rs` と `src/property_ids.rs` に合わせる
- 冒頭の件数は、コントロールとプロパティを区別して書く
- 冒頭の文は「libcamera v0.8.0+rpt20261104 の全コントロール (82 件) に対応しています。」のように書き、
  プロパティの件数は別の文か同じ文の中でコントロールと区別して示す
- 完了時点でコントロールは core 53 / draft 14 / rpi 15 の合計 82 件になる
- 新規に記載するのは `Hue` (core、ID 53) と `ControlListSequence` (rpi、ID 20009) の 2 件である
- rpi の CNN 系 6 件は ID が変わるため、既存の記載の ID を書き換える。件数は増えない
- 末尾にプロパティの章を新設し、`src/property_ids.rs` の定数を記載する。
  `ColorFilterArrangement` を含め、これまで記載されていなかった分をすべて記載する
- 各項目の書式は既存の記載に合わせる。`### 名前 (ID: n)` の見出しに続けて、定数名・型・方向・説明を書き、
  列挙子を持つものは `| 値 | 定数 | 説明 |` の表で記載する
- 型の表記は既存に倣う。スカラーは型名のみ、可変長配列は `(配列)`、固定長配列は `(配列, サイズ N)`、
  多次元配列は `(配列, サイズ 3x3)` のように次元を `x` で区切って書く
- プロパティの方向はすべて `out` である
- プロパティの章の見出しと件数は、コントロールの章と同じく件数を含める。
  完了時点で core 11 件 / draft 1 件の合計 12 件になる
- 説明文は既存の記載と同様に日本語で書く

### `skills/shiguredo-libcamera/SKILL.md`

次の節を更新する。

- バージョン情報: バージョンを `Cargo.toml` の値に合わせ、対応 libcamera を v0.8.0+rpt20261104 にする
- 既知の制限事項: libcamera のバージョンに関する記述を更新する
- `ControlId`: `ControlId::new` のシグネチャと、追加される `vendor()` / `array_length()` / `enumerators()` を反映する
- コントロール定数: `core` の ID の範囲と、rpi の CNN 系の ID を更新する
- プロパティ定数: 削除される定数と追加される定数に合わせて一覧を差し替える
- コード例: プロパティの読み取り例を `get_rectangle` を使う形にする
- 動作環境: `Dev Container セットアップガイドを参照` という記述は、リポジトリに該当する文書が無いため、
  `README.md` の Dev Containers の節を参照する形に正す。`docs/CONTAINER.md` は実機検証の issue が扱うため変更しない

### `CHANGES.md`

`shiguredo-changelog` スキルの規約に従い、`develop` セクションに
`[UPDATE] 対応 libcamera を v0.8.0+rpt20261104 に更新する` を担当者行付きで追記する。

`develop` セクションには `[CHANGE]` と `[ADD]` のエントリが既に並んでいる。
規約の種別順 (CHANGE → ADD → UPDATE → FIX) に従い、`[UPDATE]` は `[ADD]` の後、`[FIX]` の前に置く。
本 issue が追記するエントリはこの 1 件だけである。

定数の追加削除と `ControlId` の変更は、それぞれの issue が自身で記載するため、
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
- `skills/shiguredo-libcamera/SKILL.md` の次の文字列が、`skills/shiguredo-libcamera/SKILL.md` の中で残っていない
  (確認は対象ファイルを限定して行う。`CHANGES.md` には `2026.1.0` や `PIXEL_ARRAY_SIZE` が別の文脈で存在する)
  - `2026.1.0`
  - `v0.7.0+rpt20260205`
  - `PIXEL_ARRAY_SIZE`
  - `ID 1..52`
- `skills/shiguredo-libcamera/SKILL.md` の `core` の名前空間の行が `ID 1..53` になり、主要な定数の列挙に `HUE` が含まれている
- `skills/shiguredo-libcamera/SKILL.md` のコード例が、完了時点の公開 API で動作する形になっている
  (確認は `skills/shiguredo-libcamera/SKILL.md` の getter の表と突き合わせる。
  `examples/` にはプロパティを読むサンプルが無いため、既存サンプルとの突き合わせはできない。
  コードブロックのコンパイル確認は本 issue の対象外とする)
- `CHANGES.md` の `develop` セクションで、`[UPDATE]` が `[ADD]` の後かつ `[FIX]` の前に担当者行付きで記載されている
- `prek run trailing-whitespace end-of-file-fixer mixed-line-ending --all-files` が通る
  (本 issue はドキュメントのみを変更するため、`cargo-fmt` / `cargo-clippy` / `cargo-test` は対象外とする)

## 解決方法

- `README.md` の「対応 libcamera バージョン」の引用ブロックを更新する
- `docs/CONTROLS.md` の冒頭と各章の件数を更新し、`Hue` と `ControlListSequence` を追記し、
  rpi の CNN 系の ID を書き換え、末尾にプロパティの章を新設する
- `skills/shiguredo-libcamera/SKILL.md` の「設計方針」に挙げた各節を更新する
- `CHANGES.md` の `develop` セクションに `[UPDATE]` を 1 件追記する
- 全角と半角の間に半角スペースを入れる規約に従う
