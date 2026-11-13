# ControlValue::get<T>() の std::optional 化に c-api のコントロール getter を追従させる

- Created: 2026-11-13
- Completed: {YYYY-MM-DD}
- Branch: feature/change-control-value-optional
- Polished: {YYYY-MM-DD}

## 目的

libcamera v0.8.0+rpt20261104 では `ControlValue::get<T>()` の戻り値が `T` から `std::optional<T>` に変更された。
本リポジトリの `c-api/controls.cpp` は戻り値をそのまま受けているため、v0.8.0 ではコンパイルが通らない。
v0.8.0 への追従にあたり、コントロール取得層を新 API に合わせる必要がある。

## 現状

`c-api/controls.cpp` のスカラー getter が `val.get<T>()` の戻り値を直接 `*out` に代入している。

- `lc_control_list_get_bool`
- `lc_control_list_get_byte`
- `lc_control_list_get_int32`
- `lc_control_list_get_int64`
- `lc_control_list_get_float`
- `lc_control_list_get_string`
- `lc_control_list_get_rectangle`
- `lc_control_list_get_size`

例えば `lc_control_list_get_bool` は次の形になっている。

```cpp
  const auto& val = list->list->get(id);
  if (val.type() != ControlTypeBool) {
    return false;
  }
  *out = val.get<bool>();
  return true;
```

v0.8.0 の `include/libcamera/controls.h` では `get<T>()` が `std::optional<T>` を返すため、この代入は型不整合になる。
`lc_control_list_get_string` は `std::string_view` を受け取っており、ここも `std::optional<std::string_view>` になる。

v0.8.0 の `get<T>()` は、コントロール ID がリストに存在しない場合に `std::nullopt` を返す。
既存実装は `contains(id)` と `type()` の事前チェックを行っているため、失敗経路は既に網羅されている。

## 設計方針

- getter の「値が存在しない / 型が違う」という失敗経路を、C ABI の `bool` 戻り値にそのまま写像する
- v0.8.0 の `get<T>()` が返す `std::optional` は `has_value()` で判定し、`std::nullopt` の場合は `false` を返す
- 既存の `contains(id)` と `type()` の事前チェックは残す。理由は、型不一致と値の欠落を区別しない C ABI の契約を変えないため
- 配列 getter (`lc_control_list_get_*_array`) が使っている `data()` / `numElements()` は v0.8.0 でも同じシグネチャで残っているため、この issue では変更しない
- Rust 側の公開 API (`src/controls.rs` の `ControlList::get_*`) のシグネチャと挙動は変えない

## 完了条件

- v0.8.0 のヘッダーに対して `c-api/controls.cpp` のスカラー getter がコンパイルできる
- コントロールが存在しない場合、型が一致しない場合、`std::nullopt` の場合のいずれも `false` を返す
- `cargo clippy --workspace --all-targets --target aarch64-unknown-linux-gnu -- -D warnings` が通る
- 実機 Raspberry Pi 上で `controls` サンプルが従来どおり動作する

## 解決方法

- `c-api/controls.cpp` の 8 つのスカラー getter で、`val.get<T>()` の結果を `std::optional<T>` として受け取り、`if (!v) return false;` の後に `*out = *v;` とする
- `lc_control_list_get_string` は `std::optional<std::string_view>` を受け取り、`v->data()` と `v->size()` を設定する
- コメントは日本語、ログメッセージは英語で書く
