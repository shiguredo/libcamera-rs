# v0.8.0 で追加されたコントロールを追加する

- Created: 2026-11-13
- Completed: {YYYY-MM-DD}
- Branch: feature/add-new-controls-0_8_0
- Polished: {YYYY-MM-DD}

## 目的

libcamera v0.8.0+rpt20261104 の control_ids の YAML には、v0.7.2 時点で反映済みのものに加えて新しいコントロールが存在する。
本リポジトリは `docs/CONTROLS.md` の冒頭で「libcamera の全コントロールに対応しています」と宣言しているため、
v0.8.0 で追加されたコントロールを定義しないと、この宣言と実装が食い違う。
`Hue` は画像の色相を制御する基本操作であり、未定義のままでは利用者が本クレート経由で設定できない。
`ControlListSequence` は Raspberry Pi の同期制御で使われる項目で、rpi ベンダーコントロールを網羅するうえで必要になる。

## 現状

`src/control_ids.rs` の定数定義 (80 件) と、v0.8.0 の YAML を機械的に突き合わせた結果は次のとおり。

| 区分 | 現状 | v0.8.0 | 未定義 |
|---|---|---|---|
| core | 52 | 53 | `Hue` |
| draft | 14 | 14 | なし |
| rpi | 14 | 15 | `ControlListSequence` |

draft については v0.8.0 の 14 件すべてが定義済みであり、追加作業は不要である。
`ColourCorrectionMatrix` / `DebugMetadataEnable` / `FrameDurationLimits` / `LensShadingCorrectionEnable` などの
core の追加項目は、現状の `src/control_ids.rs` に既に定義されていることを ID と名称の突き合わせで確認済みである。

未定義の 2 件の仕様は次のとおり。

- `Hue` (core、float、inout)。v0.8.0 の `control_ids_core.yaml` で末尾 (53 番目) に追加されており、ID は 52 になる
- `ControlListSequence` (rpi、int64、out)。v0.8.0 の `control_ids_rpi.yaml` で `SyncFrames` の直後 (9 番目) に挿入されている

`ControlListSequence` の挿入位置に注意が必要である。v0.8.0 の rpi の YAML は `SyncFrames` → `ControlListSequence` →
`CnnOutputTensor` の順に並んでおり、現状の `src/control_ids.rs` は `CnnOutputTensor` を ID 20009 として定義している。
libcamera の control ID は YAML の並び順に基づいて採番されるため、**v0.8.0 では `CnnOutputTensor` 以降の rpi コントロールの ID が
1 つずつ繰り下がる**可能性が高い。この場合 `src/control_ids.rs` の rpi の ID は 20009〜20014 の 6 件を更新する必要がある。
ID の実際の値は v0.8.0 の生成ヘッダーまたは `ControlId` の値から確認して確定させること。

## 設計方針

- v0.8.0 の `control_ids_core.yaml` と `control_ids_rpi.yaml` を正本とし、`Hue` と `ControlListSequence` の
  ID・型・方向を転記する。推測で埋めない
- 既存の定数の書式 (SCREAMING_SNAKE_CASE の定数名、`ControlId::new` の 4 引数) に合わせる
- 既存の 80 件の定数は変更しない。v0.8.0 で YAML の `name` から共通プレフィクスが除去されたが `value` は変わっていないため、
  既存の列挙値モジュールの定数名と値はそのまま維持できる
- `docs/CONTROLS.md` の更新は別の issue で扱う。この issue では Rust の定数定義のみを扱う

## 完了条件

- `src/control_ids.rs` に `Hue` と `ControlListSequence` が定義され、ID・型・方向が v0.8.0 の YAML と一致している
- 定義数が core 53 / draft 14 / rpi 15 になっている
- `cargo clippy --workspace --all-targets -- -D warnings` が通る

## 解決方法

- v0.8.0 の YAML から `Hue` と `ControlListSequence` の ID・型・方向を確認する
- `src/control_ids.rs` の `core` モジュールに `HUE`、`rpi` モジュールに `CONTROL_LIST_SEQUENCE` を追加する
- 追加位置は既存の定義順 (ID 昇順) に合わせる
