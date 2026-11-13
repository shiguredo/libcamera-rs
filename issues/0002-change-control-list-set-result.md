# ControlList::set の暗黙失敗に対応し ControlList::set_* を Result 返却にする

- Created: 2026-11-13
- Completed: {YYYY-MM-DD}
- Branch: feature/change-control-list-set-result
- Polished: {YYYY-MM-DD}

## 目的

libcamera v0.8.0+rpt20261104 では `ControlList::set(unsigned int, const ControlValue&)` の挙動が変わり、
対象のコントロール ID がリストに存在しない場合は**何もせずに戻る** (エラーもシグナルも返さない)。
本リポジトリの `ControlList::set_*` は戻り値が `()` のため、設定が黙って失敗したことを呼び出し側が一切検知できない。
カメラ制御値の設定漏れは画質や露出の誤動作に直結するため、明示的に失敗を返す必要がある。

## 現状

`c-api/controls.cpp` の setter は `ControlValue` を作ってから `list->list->set(id, cv)` を呼ぶだけで、結果を返さない。

```cpp
void lc_control_list_set_bool(lc_control_list_t* list,
                              unsigned int id,
                              bool value) {
  ControlValue cv;
  cv.set<bool>(value);
  list->list->set(id, cv);
}
```

同じ形の関数が次の 12 個ある。

- スカラー: `lc_control_list_set_bool` / `_byte` / `_int32` / `_int64` / `_float` / `_rectangle`
- 配列: `lc_control_list_set_byte_array` / `_int32_array` / `_int64_array` / `_float_array` / `_rectangle_array` / `_point_array`

Rust 側の `src/controls.rs` は公開 API がすべて `()` を返している。

- `ControlList::set_bool`
- `ControlList::set_i32`
- `ControlList::set_i64`
- `ControlList::set_f32`
- `ControlList::set_rectangle`
- `ControlList::set_f32_array`
- `ControlList::set_i32_array`
- `ControlList::set_i64_array`
- `ControlList::set_u8`
- `ControlList::set_u8_array`
- `ControlList::set_rectangle_array`
- `ControlList::set_point_array`

README の使用例 (`controls.set_f32(&core::BRIGHTNESS, 0.2);`) も戻り値を無視する形で書かれている。

## 設計方針

- C ABI の setter を `void` から `bool` に変更し、設定できた場合に `true`、対象が存在しない場合に `false` を返す
- 失敗を検出するため、v0.8.0 の `ControlList::contains(unsigned int)` で事前に存在確認する。存在しなければ `false` を返して終わる
- Rust 側の `ControlList::set_*` を `()` ではなく `Result<()>` に変更する。失敗時は `Error::ControlSetFailed` を返す
- `src/error.rs` に `Error::ControlSetFailed` を追加する。表示文言は英語で `"failed to set control value"` とする
- これは後方互換のない変更なので `CHANGES.md` に `[CHANGE]` として記載する
- README の使用例は `?` 演算子または `unwrap()` を使う形に更新する
- 空の配列 (`values.len() == 0`) をどう扱うかは v0.8.0 の仕様を確認したうえで決める。確認できるまで推測で決めない

## 完了条件

- C ABI の全 setter が `bool` を返し、存在しない ID に対して `false` を返す
- Rust の `ControlList::set_*` が `Result<()>` を返し、失敗時に `Error::ControlSetFailed` となる
- `cargo test --workspace`、`cargo clippy --workspace --all-targets -- -D warnings` が通る
- 実機 Raspberry Pi 上で `controls` サンプルが従来どおり動作する
- `CHANGES.md` と README の使用例が更新されている

## 解決方法

- `c-api/controls.h` の setter 宣言を `bool` 戻り値に変更する
- `c-api/controls.cpp` の各 setter の先頭で `if (!list->list->contains(id)) { return false; }` を判定し、`list->list->set(id, cv);` の後に `return true;` を置く
- `src/controls.rs` の `set_*` を `Result<()>` に変更し、C ABI の戻り値が `false` の場合に `Err(Error::ControlSetFailed)` を返す
- `src/error.rs` に `ControlSetFailed`、`impl fmt::Display` の対応するアーム、および `Error` の網羅性を追加する
