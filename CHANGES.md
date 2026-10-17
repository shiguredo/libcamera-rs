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
