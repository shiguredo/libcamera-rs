# v0.7.2 追従時に取りこぼしたコントロールを追加し rpi の ID を是正する

- Created: 2026-11-13
- Completed: {YYYY-MM-DD}
- Branch: feature/add-new-controls-0_8_0
- Polished: 2026-11-13

## 目的

本リポジトリは `docs/CONTROLS.md` の冒頭で「libcamera の全コントロールに対応しています」と宣言しているが、
`src/control_ids.rs` には libcamera のコントロール 2 件が定義されていない。
加えて、rpi のコントロール 6 件が libcamera の ID と一致していない。

`Hue` は画像の色相を制御する基本操作であり、未定義のため本クレート経由では設定できない。
`ControlListSequence` は、直前に適用されたリクエストのシーケンス番号をメタデータとして返すコントロールである。
コントロールは数フレーム遅れて適用されるため、利用者が「この撮影条件がどのリクエストのものか」を同定するには
このシーケンス番号が必要になる。rpi の同期制御 (サーバーとクライアント) でフレームの適用タイミングを
突き合わせる用途でも使われる。

さらに rpi の `CNN_OUTPUT_TENSOR` 以降の 6 件は、libcamera が使う ID から 1 つずつずれている。
ID がずれたままでは、利用者が本クレートで設定した値が libcamera 側の別のコントロールに渡る。

**libcamera v0.8.0 では新しいコントロールは追加されていない。**
v0.7.2+rpt20261009 と v0.8.0+rpt20261104 の `control_ids_core.yaml` / `control_ids_draft.yaml` / `control_ids_rpi.yaml` は、
コントロールの集合・並び順・型・方向・配列長がすべて一致している。
差分は列挙子の `name` から共通プレフィクスが除かれた点と、それに伴う説明文の参照表記の変化だけである。
本 issue が扱う 2 件は、libcamera v0.7.0+rpt20260205 から v0.7.2+rpt20261009 にかけて追加されたものであり、
本リポジトリの 2026.2.0 (v0.7.2 追従) で `src/control_ids.rs` が更新されなかったため取りこぼしたものである。
`git log -- src/control_ids.rs` の結果は初回インポートの 1 件のみで、v0.7.2 追従のコミットではこのファイルは変更されていない。

## 現状

`src/control_ids.rs` の定数定義は 80 件 (core 52 / draft 14 / rpi 14) であり、libcamera の YAML の件数と一致しない。

| 区分 | 現状 | v0.7.2 | v0.8.0 | 未定義 |
|---|---|---|---|---|
| core | 52 | 53 | 53 | `Hue` |
| draft | 14 | 14 | 14 | なし |
| rpi | 14 | 15 | 15 | `ControlListSequence` |

v0.7.2 と v0.8.0 の件数は同じであり、本リポジトリが未定義なのは v0.7.2 時点からの取りこぼしによるものである。
draft の 14 件はすべて定義済みである。
`ColourCorrectionMatrix` / `DebugMetadataEnable` / `FrameDurationLimits` / `LensShadingCorrectionEnable` も定義済みである。

libcamera のコントロール ID は `utils/codegen/gen-controls.py` の `extend_control` が
`ranges[vendor] + 並び順の index + 1` で採番する。`src/libcamera/control_ranges.yaml` の値は
libcamera が 0、draft が 10000、rpi が 20000 である。

### 追加する 2 件

- `Hue`: core、`float`、`inout`。v0.8.0 の `control_ids_core.yaml` の 53 番目 (index 52) にあるため、ID は `0 + 52 + 1 = 53` になる
- `ControlListSequence`: rpi、`int64_t`、`out`。v0.8.0 の `control_ids_rpi.yaml` の 9 番目 (index 8) にあるため、ID は `20000 + 8 + 1 = 20009` になる

### rpi の ID のずれ

現行の `src/control_ids.rs` の rpi の ID は、`ControlListSequence` の定義が無いまま詰めて採番されているため、
`CnnOutputTensor` 以降が 1 つずつずれている。現行値と正しい値の対応は次のとおり。

| 正しい ID | 正しいコントロール名 | 現行の ID | 現行のコントロール名 |
|---|---|---|---|
| 20009 | `ControlListSequence` | 20009 | `CnnOutputTensor` |
| 20010 | `CnnOutputTensor` | 20010 | `CnnOutputTensorInfo` |
| 20011 | `CnnOutputTensorInfo` | 20011 | `CnnEnableInputTensor` |
| 20012 | `CnnEnableInputTensor` | 20012 | `CnnInputTensor` |
| 20013 | `CnnInputTensor` | 20013 | `CnnInputTensorInfo` |
| 20014 | `CnnInputTensorInfo` | 20014 | `CnnKpiInfo` |
| 20015 | `CnnKpiInfo` | (未定義) | (未定義) |

現行の 6 件はすべて 1 つ手前のコントロール名を指している。

## 設計方針

### 変更対象

`src/control_ids.rs` と `CHANGES.md` を変更する。
`c-api/`、`src/controls.rs`、`examples/` は変更しない。
`docs/CONTROLS.md` と `skills/shiguredo-libcamera/SKILL.md` の更新はドキュメント更新の issue が扱う。

### 定数の追加と ID の是正

- `core` モジュールに `HUE` を ID 53 で追加する
- `rpi` モジュールに `CONTROL_LIST_SEQUENCE` を ID 20009 で追加する
- `rpi` モジュールの `CNN_OUTPUT_TENSOR` / `CNN_OUTPUT_TENSOR_INFO` / `CNN_ENABLE_INPUT_TENSOR` /
  `CNN_INPUT_TENSOR` / `CNN_INPUT_TENSOR_INFO` / `CNN_KPI_INFO` の ID を 20010 から 20015 に変更する
- `draft` モジュールは変更しない
- 既存の列挙値モジュールとその定数値は変更しない
- 定数名は SCREAMING_SNAKE_CASE とする

### `ControlId::new` のシグネチャ

`ControlId::new` の引数は、`ControlId` に vendor / 配列長 / 列挙子を追加する issue が拡張する。
本 issue はその拡張後のシグネチャに従う。
拡張が未適用の状態で実装する場合は現行の 4 引数で定義し、拡張の適用時にこの 2 件も新しいシグネチャへ追随させる。

### 変更履歴

`CHANGES.md` の `develop` セクションに次の 2 件を追記し、`shiguredo-changelog` の規約に従って担当者行を付ける。

- `[ADD]`: `core::HUE` と `rpi::CONTROL_LIST_SEQUENCE` を追加する
- `[CHANGE]`: `rpi` の `CNN_OUTPUT_TENSOR` 以降 6 件の ID を libcamera の採番に合わせて変更する

2 件目の ID 変更は、利用者が指定した ID が別のコントロールに渡っていたことを意味するため後方互換がない。
`[CHANGE]` として記載する。

## 完了条件

- `src/control_ids.rs` の `core` モジュールに `HUE` が ID 53 で定義されている
- `src/control_ids.rs` の `rpi` モジュールに `CONTROL_LIST_SEQUENCE` が ID 20009 で定義されている
- `rpi` モジュールの `CNN_OUTPUT_TENSOR` から `CNN_KPI_INFO` までの 6 件が ID 20010 から 20015 になっている
- 定義数が core 53 / draft 14 / rpi 15 の合計 82 件になっている
- `HUE` の型が `ControlType::Float`、方向が `Direction::InOut` である
- `CONTROL_LIST_SEQUENCE` の型が `ControlType::Int64`、方向が `Direction::Out` である
- 既存の列挙値モジュールの定数名と値が変化していない
- `cargo clippy --workspace --all-targets --target aarch64-unknown-linux-gnu -- -D warnings` が通る
- `CHANGES.md` の `develop` セクションに `[ADD]` と `[CHANGE]` が担当者行付きで記載されている

## 解決方法

- `src/control_ids.rs` の `core` モジュールの末尾に `HUE` を追加する
- `src/control_ids.rs` の `rpi` モジュールで、`SYNC_FRAMES` の直後に `CONTROL_LIST_SEQUENCE` を追加し、
  `CNN_OUTPUT_TENSOR` 以降の 6 件の ID を 1 つずつ増やす
- 追加する 2 件の型と方向は v0.8.0 の YAML の `type` と `direction` をそのまま移す
- `CHANGES.md` の `develop` セクションに `[ADD]` と `[CHANGE]` を追加する
