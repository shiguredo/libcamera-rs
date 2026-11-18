# 変更履歴

- CHANGE
  - 後方互換のない変更
- ADD
  - 後方互換がある追加
- UPDATE
  - 後方互換がある変更
- FIX
  - バグ修正

## develop

- [CHANGE] ControlId に vendor / array_length / enumerators を追加する
  - @Hexa
- [CHANGE] rpi の CNN_OUTPUT_TENSOR 以降 6 件の ID を libcamera の採番に合わせる
  - @Hexa
- [CHANGE] core::PIXEL_ARRAY_SIZE と core::PIXEL_ARRAY_OPTICAL_BLACK_RECTANGLES を削除する
  - PIXEL_ARRAY_SIZE の後継は core::PIXEL_ARRAY_READABLE_AREA であり、get_size から get_rectangle に変える必要がある
  - 後継の矩形は幅と高さに加えて PixelArrayArea を基準とした x と y を持つため、Size の width と height をそのまま置き換えることはできない
  - PIXEL_ARRAY_OPTICAL_BLACK_RECTANGLES には後継が無い。光学ブラック画素の位置を返すプロパティは libcamera に用意されていない
  - get_* は ControlId の型ではなく数値の ID だけを C ABI に渡すため、ID 5 と 6 を数値で直接組んだ ControlId はコンパイルでは検出できない
  - 削除後は ID 5 と 6 が別のプロパティを指すため、削除前に定数を使って取得していた箇所は呼び出し先を確認すること
  - @Hexa
- [ADD] core::PIXEL_ARRAY_AREA と core::PIXEL_ARRAY_READABLE_AREA と core::PIPELINE_HANDLER を追加する
  - @Hexa
- [ADD] core::HUE と rpi::CONTROL_LIST_SEQUENCE を追加する
  - @Hexa
- [UPDATE] 対応 libcamera を v0.8.0+rpt20261104 に更新する
  - @Hexa
- [FIX] lc_camera_manager_get_camera_by_id に NULL を渡すとクラッシュする問題を修正する
  - @Hexa

## 2026.2.0

**リリース日**: 2026-10-19

- [CHANGE] 対応 libcamera を v0.7.2+rpt20261009 に更新する
  - libcamera 0.7 系の公開ヘッダーが要求する C++20 で C++ ラッパーをビルドするように変更する
  - libcamera::Span の廃止に伴い std::span を利用するように変更する
  - @melpon
- [CHANGE] 最小サポート Rust バージョン (MSRV) を 1.93 に引き上げる
  - @voluntas
- [FIX] capture サンプルが取得したバッファ数より多いフレーム数を待ち続けてハングする問題を修正する
  - @melpon

## 2026.1.0

**リリース日**: 2026-06-23
