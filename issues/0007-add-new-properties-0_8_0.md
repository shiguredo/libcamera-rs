# v0.7.2 追従時に取りこぼしたプロパティを追加し不要になった定数を削除する

- Created: 2026-11-13
- Completed: {YYYY-MM-DD}
- Branch: feature/add-new-properties-0_8_0
- Polished: 2026-11-13

## 目的

本リポジトリの `src/property_ids.rs` は、v0.6.0 時点のプロパティの並びに基づいて定数を定義している。
libcamera v0.7.2+rpt20261009 でプロパティの構成が変わり、本リポジトリの 2026.2.0 (v0.7.2 追従) では
このファイルが更新されなかった。

そのため現行の定数には次の問題がある。

- v0.7.2 で追加された `PixelArrayArea` / `PixelArrayReadableArea` / `PipelineHandler` が定義されておらず、
  利用者が本クレート経由で取得できない
- v0.7.2 で削除された `PixelArraySize` / `PixelArrayOpticalBlackRectangles` が定義されたまま残っており、
  同じ ID には別のプロパティが割り当てられている。そのため利用者がこれらの定数を渡すと、
  取得に失敗するか、別の意味の値を受け取る

**libcamera v0.8.0 のプロパティは v0.7.2 と同じであり、v0.8.0 で追加されたプロパティはない。**
v0.7.2+rpt20261009 と v0.8.0+rpt20261104 の `property_ids_core.yaml` はプロパティの集合・並び順・型がすべて一致しており、
差分は `Location` の列挙子名から共通プレフィクスが除かれた点だけである。
`property_ids_draft.yaml` は両バージョンで完全に一致している。

したがって本 issue は v0.8.0 追従の必須作業ではなく、2026.2.0 の v0.7.2 追従時の取りこぼしを補うものである。
`git log -- src/property_ids.rs` の結果は初回インポートの 1 件のみで、v0.7.2 追従のコミットではこのファイルは変更されていない。

## 現状

`src/property_ids.rs` の定数定義は 11 件 (core 10 / draft 1) である。

- `core`: `Location` (1) / `Rotation` (2) / `Model` (3) / `UnitCellSize` (4) / `PixelArraySize` (5) /
  `PixelArrayOpticalBlackRectangles` (6) / `PixelArrayActiveAreas` (7) / `ScalerCropMaximum` (8) /
  `SensorSensitivity` (9) / `SystemDevices` (10)
- `draft`: `ColorFilterArrangement` (10001)

現行の core の 10 件は v0.6.0 / v0.7.0 の `property_ids_core.yaml` の並びと一致する。
v0.7.2 以降の `property_ids_core.yaml` は次の 11 件であり、現行と一致しない。

| 順 | 名称 | 型 | 配列 |
|---|---|---|---|
| 1 | `Location` | int32_t | スカラー |
| 2 | `Rotation` | int32_t | スカラー |
| 3 | `Model` | string | スカラー |
| 4 | `UnitCellSize` | Size | スカラー |
| 5 | `PixelArrayArea` | Rectangle | スカラー |
| 6 | `PixelArrayReadableArea` | Rectangle | スカラー |
| 7 | `PixelArrayActiveAreas` | Rectangle | 可変長配列 |
| 8 | `ScalerCropMaximum` | Rectangle | スカラー |
| 9 | `SensorSensitivity` | float | スカラー |
| 10 | `SystemDevices` | int64_t | 可変長配列 |
| 11 | `PipelineHandler` | string | スカラー |

libcamera のプロパティ ID は `utils/codegen/gen-controls.py` の `extend_control` が
`ranges[vendor] + 並び順の index + 1` で採番する。`src/libcamera/control_ranges.yaml` の値は
libcamera が 0、draft が 10000 である。
この規則により、`PixelArrayArea` は 5、`PixelArrayReadableArea` は 6、`PipelineHandler` は 11 になる。

### 旧定数と新プロパティの対応

`PixelArraySize` と `PixelArrayReadableArea` は、説明文の内容が対応している。

- `PixelArraySize` (v0.6.0): "The camera sensor pixel array readable area vertical and horizontal sizes, in pixels."
- `PixelArrayReadableArea` (v0.7.2 以降): "The camera sensor pixel array readable area."

つまり `PixelArraySize` の後継は `PixelArrayReadableArea` である。ただし型が `Size` から `Rectangle` に変わっており、
`width` と `height` に加えて読み出し位置の `x` と `y` を持つようになった。

`PixelArrayOpticalBlackRectangles` には後継のプロパティが存在しない。
このプロパティは raw データバッファ上の光学ブラック画素の位置を返すものであり、
v0.7.2 以降の libcamera に同等の情報を返すプロパティは用意されていない。
`PixelArrayArea` は、読み出し可能領域ではなく画素配列全体の領域を表す新しい概念であり、
`PixelArraySize` の置き換えではない。

### 現行定数を使った場合の挙動

`c-api/controls.cpp` の `lc_control_list_get_*` は、ID がリストに存在しないか型が一致しない場合に `false` を返す。
`src/controls.rs` の `ControlList::get_*` はその場合に `Err(Error::ControlGetFailed)` を返す。

v0.7.2 以降の libcamera では ID 5 は `PixelArrayArea` (Rectangle) であり、ID 6 は `PixelArrayReadableArea` (Rectangle) である。

- `PIXEL_ARRAY_SIZE` (ID 5) を `get_size` に渡すと、型が一致しないため `Err(Error::ControlGetFailed)` になる
- `PIXEL_ARRAY_OPTICAL_BLACK_RECTANGLES` (ID 6) を `get_rectangle_array` に渡すと、型が一致するため
  `PixelArrayReadableArea` の値を受け取る。エラーにならず、別の意味の値を返す

## 設計方針

### 変更対象

`src/property_ids.rs` と `CHANGES.md` を変更する。
`c-api/`、`src/controls.rs`、`examples/` は変更しない。
`docs/CONTROLS.md` と `skills/shiguredo-libcamera/SKILL.md` の更新はドキュメント更新の issue が扱う。

### 定数の追加と削除

- `core` モジュールに `PIXEL_ARRAY_AREA` を ID 5、`PIXEL_ARRAY_READABLE_AREA` を ID 6、
  `PIPELINE_HANDLER` を ID 11 で追加する
- `PIXEL_ARRAY_SIZE` と `PIXEL_ARRAY_OPTICAL_BLACK_RECTANGLES` を削除する
  - 後継があるのは `PIXEL_ARRAY_READABLE_AREA` だけであり、型が `Size` から `Rectangle` に変わるため、
    `#[deprecated]` の note で単純に誘導できない
  - `PIXEL_ARRAY_OPTICAL_BLACK_RECTANGLES` には後継が無く、`#[deprecated]` の note に書く代替を示せない
  - どちらも残すと、ID が別のプロパティに再利用されているため、エラーにならず誤った値を受け取る経路が残る
- `draft` の `ColorFilterArrangement` と `color_filter_arrangement` モジュールは両バージョンに存在するため変更しない
- `core::location` の列挙値定数 (`FRONT` / `BACK` / `EXTERNAL`) は v0.7.2 以降の列挙子名と値が一致しているため変更しない
- 既存の定数のうち、削除する 2 件以外の ID・名前・型は変更しない

### `ControlId::new` のシグネチャ

`ControlId::new` の引数は、`ControlId` に vendor / 配列長 / 列挙子を追加する issue が拡張する。
本 issue はその拡張後のシグネチャに従う。
拡張が未適用の状態で実装する場合は現行の 4 引数で定義し、拡張の適用時にこの 3 件も新しいシグネチャへ追随させる。

追加する 3 件の vendor は `"libcamera"`、列挙子は空、配列長は `PixelArrayActiveAreas` と同様にすべてスカラーである。

### 変更履歴

`CHANGES.md` の `develop` セクションに次の 2 件を追記し、`shiguredo-changelog` の規約に従って担当者行を付ける。

- `[ADD]`: `core::PIXEL_ARRAY_AREA` / `core::PIXEL_ARRAY_READABLE_AREA` / `core::PIPELINE_HANDLER` を追加する
- `[CHANGE]`: `core::PIXEL_ARRAY_SIZE` と `core::PIXEL_ARRAY_OPTICAL_BLACK_RECTANGLES` を削除する。
  `PIXEL_ARRAY_SIZE` の後継は `PIXEL_ARRAY_READABLE_AREA` であり、`get_size` から `get_rectangle` に変える必要がある

削除は後方互換がない変更であるため `[CHANGE]` とする。
v0.8.0 追従全体の変更履歴をドキュメント更新の issue が一括で扱う場合は重複を避けるが、
本 issue の変更は個別の定数に閉じているため本 issue で記載する。

## 完了条件

- `src/property_ids.rs` の `core` モジュールに `PIXEL_ARRAY_AREA` (ID 5) / `PIXEL_ARRAY_READABLE_AREA` (ID 6) /
  `PIPELINE_HANDLER` (ID 11) が定義されている
- `PIXEL_ARRAY_AREA` と `PIXEL_ARRAY_READABLE_AREA` の型が `ControlType::Rectangle`、`PIPELINE_HANDLER` の型が `ControlType::String` である
- 3 件の方向が `Direction::Out` である
- `PIXEL_ARRAY_SIZE` と `PIXEL_ARRAY_OPTICAL_BLACK_RECTANGLES` が削除されている
- `core` のプロパティ定数が ID 1 から 11 まで欠番なく 11 件になり、名称と型が v0.7.2 以降の `property_ids_core.yaml` と一致している
- `draft` の `COLOR_FILTER_ARRANGEMENT` と `color_filter_arrangement` モジュールが変更されていない
- `cargo clippy --workspace --all-targets --target aarch64-unknown-linux-gnu -- -D warnings` が通る
- `cargo fmt --all -- --check` が通る
- `CHANGES.md` の `develop` セクションに `[ADD]` と `[CHANGE]` が担当者行付きで記載されている

## 解決方法

- `src/property_ids.rs` の `core` モジュールで `PIXEL_ARRAY_SIZE` と `PIXEL_ARRAY_OPTICAL_BLACK_RECTANGLES` を削除する
- 同じ位置に `PIXEL_ARRAY_AREA` と `PIXEL_ARRAY_READABLE_AREA` を追加し、`SystemDevices` の後に `PIPELINE_HANDLER` を追加する
- 追加する 3 件の型は v0.8.0 の YAML の `type` をそのまま移す
- プロパティの YAML には `direction` フィールドが無く、libcamera の生成規則によりプロパティの方向は常に `out` になる。
  3 件の方向は `Direction::Out` とする
- `skills/shiguredo-libcamera/SKILL.md` は `PIXEL_ARRAY_SIZE` と `get_size(&props::PIXEL_ARRAY_SIZE)` を記載しており、
  本 issue でこの定数を削除すると記載が壊れる。この修正はドキュメント更新の issue が扱う
- `CHANGES.md` の `develop` セクションに `[ADD]` と `[CHANGE]` を追加する
