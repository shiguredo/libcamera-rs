# v0.8.0 で追加されたプロパティを追加する

- Created: 2026-11-13
- Completed: {YYYY-MM-DD}
- Branch: feature/add-new-properties-0_8_0
- Polished: {YYYY-MM-DD}

## 目的

libcamera v0.8.0+rpt20261104 の `property_ids_core.yaml` では、v0.7.2 時点のプロパティ集合から名称が整理された。
本リポジトリの `src/property_ids.rs` は `Camera::properties()` で返る値を型付きで参照するための定数を公開しているため、
v0.8.0 で追加された `PixelArrayArea` / `PixelArrayReadableArea` / `PipelineHandler` を定義しないと、
利用者がそれらの値を本クレート経由で取得できない。
また現行の定数には v0.8.0 に存在しない名称が含まれており、そのままでは存在しないプロパティを参照して取得に失敗する。

## 現状

`src/property_ids.rs` の定数定義 (11 件) は次のとおり。

- `core`: `Location` (1) / `Rotation` (2) / `Model` (3) / `UnitCellSize` (4) / `PixelArraySize` (5) /
  `PixelArrayOpticalBlackRectangles` (6) / `PixelArrayActiveAreas` (7) / `ScalerCropMaximum` (8) /
  `SensorSensitivity` (9) / `SystemDevices` (10)
- `draft`: `ColorFilterArrangement` (10001)

v0.8.0 の `property_ids_core.yaml` は並び順に次の 11 件を持つ。

| 順 | 名称 | 型 |
|---|---|---|
| 1 | `Location` | int32_t |
| 2 | `Rotation` | int32_t |
| 3 | `Model` | string |
| 4 | `UnitCellSize` | Size |
| 5 | `PixelArrayArea` | Rectangle |
| 6 | `PixelArrayReadableArea` | Rectangle |
| 7 | `PixelArrayActiveAreas` | Rectangle |
| 8 | `ScalerCropMaximum` | Rectangle |
| 9 | `SensorSensitivity` | float |
| 10 | `SystemDevices` | int64_t |
| 11 | `PipelineHandler` | string |

差分は次の 2 点である。

- 未定義: `PixelArrayArea` (ID 5) / `PixelArrayReadableArea` (ID 6) / `PipelineHandler` (ID 11)
- v0.8.0 に存在しない: `PixelArraySize` (現行 ID 5) / `PixelArrayOpticalBlackRectangles` (現行 ID 6)

現行の ID 5 と 6 は、v0.8.0 では `PixelArrayArea` と `PixelArrayReadableArea` に置き換わっている。
v0.8.0 の説明文は `PixelArrayArea` が "The camera sensor pixel array area, in pixels."、
`PixelArrayReadableArea` が "The camera sensor pixel array readable area." であり、
現行の `PixelArraySize` と `PixelArrayOpticalBlackRectangles` に対応する。
ただし `PixelArraySize` (Size 型) と `PixelArrayArea` (Rectangle 型) は型が異なる。
現行の `PIXEL_ARRAY_SIZE` を使って取得しているコードは、v0.8.0 では別の型のプロパティになるため、
利用側の扱いが変わる点に注意が必要である。

## 設計方針

- v0.8.0 の `property_ids_core.yaml` を正本とし、ID・型を転記する。推測で埋めない
- `PixelArraySize` と `PixelArrayOpticalBlackRectangles` の扱いを決める。v0.8.0 に存在しないプロパティを
  そのまま残すと、利用者が取得に失敗するため、削除するか `#[deprecated]` で非推奨として残すかを選ぶ。
  型が異なる置き換えであることを踏まえ、単純なエイリアスにしないこと
- `ColorFilterArrangement` は v0.8.0 の `property_ids_draft.yaml` に存在し、現行の
  `COLOR_FILTER_ARRANGEMENT` と `color_filter_arrangement` モジュールも存在するため、追加作業は不要である
- `docs/CONTROLS.md` への追記は別の issue で扱う。この issue では Rust の定数定義のみを扱う

## 完了条件

- v0.8.0 の `property_ids_core.yaml` に存在する全 11 件が `src/property_ids.rs` に定義され、ID と型が一致している
- v0.8.0 に存在しない `PixelArraySize` / `PixelArrayOpticalBlackRectangles` の扱いが決まり、コードと `CHANGES.md` に反映されている
- `cargo clippy --workspace --all-targets -- -D warnings` が通る

## 解決方法

- v0.8.0 の `property_ids_core.yaml` の説明文を読み、`PixelArrayArea` / `PixelArrayReadableArea` と
  現行の `PixelArraySize` / `PixelArrayOpticalBlackRectangles` の対応関係を確定させる
- `src/property_ids.rs` の `core` モジュールに `PIXEL_ARRAY_AREA` / `PIXEL_ARRAY_READABLE_AREA` / `PIPELINE_HANDLER` を追加する
- 廃止されたプロパティを残す場合は `#[deprecated]` 属性と代替となる定数名を doc コメントに書く
- `CHANGES.md` の区分は、削除か非推奨かで `[CHANGE]` と `[REMOVE]` を選ぶ
