# ControlId に vendor / 配列長 / 列挙子のメタデータを追加する

- Created: 2026-11-13
- Completed: {YYYY-MM-DD}
- Branch: feature/change-control-id-vendor-size
- Polished: 2026-11-13

## 目的

本リポジトリの `src/controls.rs` の `ControlId` は、id / name / control_type / direction の 4 つしか持たない。
一方 libcamera の `ControlId` は v0.4.0 以降、vendor・配列情報・列挙子のマップを保持している。
そのため利用者は、本クレートの `ControlId` から次の情報を得られない。

- そのコントロールが libcamera / draft / rpi のどれ由来か
- 配列コントロールかどうか、固定長ならその要素数
- 列挙子と値の対応を機械的に引けるか

本 issue は、この 3 つを Rust の公開 API として追加する機能追加である。

**libcamera v0.8.0 への追従を目的とするものではない。**
libcamera v0.7.2+rpt20261009 と v0.8.0+rpt20261104 の `include/libcamera/controls.h` は内容が完全に一致しており、
`ControlId` の構造は v0.8.0 で変化していない。
したがって本 issue は v0.8.0 追従の必須作業ではなく、独立した機能追加として扱う。

## 現状

`src/controls.rs` の `ControlId` は次の 4 フィールドのみを持つ。

```rust
pub struct ControlId {
    id: u32,
    name: &'static str,
    control_type: ControlType,
    direction: Direction,
}
```

`ControlId::new` も `(id, name, control_type, direction)` の 4 引数で、`src/control_ids.rs` と `src/property_ids.rs` の全定数がこの形で書かれている。

現状の定数の数は次のとおり (実ファイルの定義数)。

- `src/control_ids.rs`: 80 個 (core 52 / draft 14 / rpi 14)
- `src/property_ids.rs`: 11 個 (core 10 / draft 1)

`ControlId` は C ABI に対しては `id.id()` の `u32` しか渡していない。
`c-api/controls.h` の `lc_control_list_*` はすべて `unsigned int id` を受け取るだけで、
vendor・配列長・列挙子をやり取りする C ABI は存在しない。
したがって本 issue で追加する情報は、C++ 側の変更を一切伴わない。

列挙子の値そのものは、既に `src/control_ids.rs` の列挙値モジュール (`core::ae_state::IDLE` など) で定数として公開されている。
本 issue で追加するのは、値から名前を引くためのデータであり、定数の代替ではない。

## 設計方針

### 変更対象

`src/controls.rs`、`src/control_ids.rs`、`src/property_ids.rs`、`CHANGES.md` を変更する。
`c-api/` は変更しない。
`docs/CONTROLS.md` と `skills/shiguredo-libcamera/SKILL.md` の更新はドキュメント更新の issue が扱う。

### 追加するフィールド

- `vendor: &'static str` (値は `"libcamera"` / `"draft"` / `"rpi"`)
- `array_length: Option<usize>` を導入し、次の 3 種を区別する
  - `None`: スカラー
  - `Some(n)`: 固定長配列 (要素数 n)
  - `Some(0)`: 可変長配列
- `enumerators: &'static [(i32, &'static str)]` (列挙子を持たない場合は空スライス)

`array_length` という名前は、既存の `ControlList::size()` (リストの要素数) と `geometry::Size` (幅と高さ) と区別するために採る。
libcamera の `isArray()` は配列長が 0 より大きい場合のみ真となり可変長配列を配列と判定しないため、
その意味をそのまま写さない。本 issue では可変長配列を `Some(0)` で明示的に表す。

`ColourCorrectionMatrix` のように多次元の配列 (`[3,3]`) は、要素数の積を `array_length` に入れる。次元ごとの形状は保持しない。

### シグネチャ

`ControlId::new` の引数を上のフィールド構成に合わせて拡張する。
4 引数の短縮形は残さない。理由は、vendor を省略した定数が定義されると情報が欠落した状態が再発するためである。

`ControlId::new` は `const fn` のまま維持する。定数はすべて `const` で定義されているため、const 構築できる必要がある。

### 生成ではなく手作業で更新する

`src/control_ids.rs` と `src/property_ids.rs` は手作業で更新する。
libcamera の YAML から Rust 定数を生成するパイプラインは本 issue では作らない。理由は次のとおり。

- 生成には libcamera のソースツリーの YAML 5 ファイルと、ID 採番に使う `src/libcamera/control_ranges.yaml` が必要であり、
  本リポジトリはこれらを保持していない
- YAML の取得元・取得方法・パーサの依存 (`PyYAML` など)・バージョン固定を決める必要があり、
  ビルド保守基盤の追加という別の判断を伴う
- 生成対象に列挙値モジュールを含めるかどうかで、既存の公開定数名と衝突する可能性がある

ID は現行の値をそのまま維持し、変更しない。
本 issue は既存の定数の ID を変えない。

### ID 採番について

libcamera は YAML の並び順に `ranges[vendor] + 順序 + 1` で ID を採番している。
本リポジトリの現行 ID が v0.7.2 の YAML の並びと一致しているかは、本 issue では確認しない。
ID の不一致 (rpi の `CnnOutputTensor` 以降など) は、コントロール追加を扱う別 issue の範囲とする。

## 完了条件

- `ControlId` が `vendor` / `array_length` / `enumerators` を保持し、それぞれ `vendor()` / `array_length()` / `enumerators()` で取得できる
- `ControlId::new` が新しいシグネチャになり、4 引数の呼び出しがコンパイルエラーになる
- `src/control_ids.rs` の 80 個と `src/property_ids.rs` の 11 個の定数が新しいシグネチャで定義されている
- 既存の定数の ID・名前・型・方向が v0.7.2 時点から変化していない
- 既存の列挙値モジュールの定数値が変化していない
- `cargo clippy --workspace --target aarch64-unknown-linux-gnu -- -D warnings` が通る
- `CHANGES.md` の `develop` セクションに `[CHANGE]` が記載され、`shiguredo-changelog` の規約 (担当者行を含む) に従っている

## 解決方法

- `src/controls.rs` の `ControlId` に 3 つのフィールドを追加し、`ControlId::new` のシグネチャを拡張する
- `vendor()` / `array_length()` / `enumerators()` の getter を追加する。`ControlId::new` は `const fn` を維持する
- `src/control_ids.rs` と `src/property_ids.rs` の全定数を新しいシグネチャで書き換える。ID・名前・型・方向は変えない
- 配列長は v0.8.0 の YAML の `size` を参照し、`[n]` は `Some(0)`、`[3,3]` は `Some(9)` とする
- `enumerators` は、既存の列挙値モジュールの定数名を SCREAMING_SNAKE_CASE から元の CamelCase に戻した名前で登録する
  (例: `PN9` は `"Pn9"`、`CUSTOM1` は `"Custom1"`)。名前の出所は libcamera の YAML の `name` と一致させる
- `CHANGES.md` の `develop` セクションに `[CHANGE]` を追加する。担当者行を含め、`shiguredo-changelog` の書式に従う
