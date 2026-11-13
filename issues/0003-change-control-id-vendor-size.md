# ControlId::new に vendor と size を追加し定数定義を更新する

- Created: 2026-11-13
- Completed: {YYYY-MM-DD}
- Branch: feature/change-control-id-vendor-size
- Polished: {YYYY-MM-DD}

## 目的

libcamera v0.8.0+rpt20261104 では `ControlId` のコンストラクタが
`ControlId(unsigned int id, const std::string &name, const std::string &vendor, ControlType type, DirectionFlags direction, std::size_t size = 0, const std::map<std::string, int32_t> &enumStrMap = {})`
に変わり、vendor が必須引数として追加され、`size` (配列長) と列挙子のマップも持つようになった。
本リポジトリの `src/controls.rs` の `ControlId` は id / name / type / direction の 4 つしか持たないため、
v0.8.0 の情報 (vendor、配列かどうか、列挙子) を表現できず、`control_ids.rs` と `property_ids.rs` の定数定義もそれに合わせて作り直す必要がある。

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

現状の `ControlId` 定数の数は次のとおり。

- `src/control_ids.rs`: 80 個 (core 52 / draft 14 / rpi 14)
- `src/property_ids.rs`: 11 個 (core 10 / draft 1)

v0.8.0 の YAML では core が 53 件、draft が 14 件、rpi が 15 件、プロパティ core が 11 件、プロパティ draft が 1 件であり、
合計 94 個と 12 個になる。増加分の追加はこの issue ではなく、コントロールの追加とプロパティの追加を扱う別の issue で行う。

現状の `ControlId` は libcamera の `ControlId` とは独立した単なる数値と名前の組であり、次の情報が失われている。

- `vendor` (libcamera / draft / rpi)
- 配列コントロールかどうかとその要素数 (`size`)
- 列挙子名と値の対応 (`enumerators()`)

そのため、利用者は「このコントロールは配列か」「列挙子は何か」を Rust API から知ることができない。

## 設計方針

- `ControlId` を libcamera v0.8.0 の `ControlId` に対応するフィールド構成に拡張する
  - `vendor: &'static str`
  - `size: usize` (0 は非配列を表す)
  - 列挙子は `&'static [(i32, &'static str)]` で保持し、`enumerators()` で参照を返す
- `ControlId::new` の引数を追加する。引数追加は後方互換のない変更なので `CHANGES.md` に `[CHANGE]` として記載する
- 4 引数の短縮形は残さない。理由は、vendor を省略した定数が定義されると v0.8.0 の情報と食い違う状態が再発するため
- `src/control_ids.rs` と `src/property_ids.rs` の定数は v0.8.0 の YAML (`control_ids_core.yaml` / `control_ids_draft.yaml` / `control_ids_rpi.yaml` / `property_ids_core.yaml` / `property_ids_draft.yaml`) から機械的に生成する方針とし、手作業での書き換えは行わない
- 生成した定数のうち、列挙子を持つものは既存の `pub mod {control}_{enum}` モジュールと値を突き合わせ、**値が変わらないこと**を確認する。v0.7.2 から v0.8.0 で YAML の `name` は共通プレフィクスが除去されたが、`value` は変わっていないため、既存の enum 定数モジュールは値も名前もそのまま維持できる見込みである。ただし `CUSTOM1` と `PN9` のように Rust 側の変換規則と一致しない名前があるため、生成規則を確定させてから照合する

## 完了条件

- `ControlId` が vendor / size / 列挙子を保持し、対応する getter を持つ
- `src/control_ids.rs` と `src/property_ids.rs` の全定数が新しいシグネチャで定義され、v0.8.0 の YAML と ID・型・方向・vendor が一致する
- 既存の列挙値モジュールの定数値が v0.7.2 時点から変化していないことを確認している
- `cargo clippy --workspace --all-targets -- -D warnings` が通る
- `CHANGES.md` に `[CHANGE]` が記載されている

## 解決方法

- `src/controls.rs` の `ControlId` にフィールドを追加し、`ControlId::new` のシグネチャを拡張する
- `vendor()` / `size()` / `is_array()` / `enumerators()` の getter を追加する
- v0.8.0 の YAML を読み、`src/control_ids.rs` と `src/property_ids.rs` を生成するスクリプトを `Makefile` から呼べる形で用意する (スクリプトの置き場所と形式は実装時に決める)
- 生成結果を既存の列挙値モジュールと突き合わせ、値の差分が無いことを確認する
