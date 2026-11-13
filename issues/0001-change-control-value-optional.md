# ControlValue::get<T>() が v0.8.0 でも T を返すことを確認する

- Created: 2026-11-13
- Completed: 2026-11-13
- Branch: feature/change-control-value-optional
- Polished: {YYYY-MM-DD}

## 目的

libcamera v0.8.0+rpt20261104 への追従にあたり、本リポジトリのコントロール取得層が
v0.8.0 の `include/libcamera/controls.h` と整合しているかを確認する。

**libcamera v0.8.0 では `ControlValue::get<T>()` の戻り値は変化していない。**
v0.6.0+rpt20251202 / v0.7.2+rpt20261009 / v0.8.0+rpt20261104 の `controls.h` を確認したところ、
`ControlValue` の `get<T>()` はすべて `T get() const` を返しており、`std::optional<T>` を返す実装は存在しない。
v0.7.2 と v0.8.0 の `controls.h` は内容が完全に一致している。

したがって本 issue は v0.8.0 追従のための変更作業ではなく、現行実装が v0.8.0 のヘッダーで
そのままコンパイルできることを確認するものである。

## 現状

`c-api/controls.cpp` のスカラー getter は 8 個あり、いずれも事前チェックのうえで `val.get<T>()` の戻り値を
直接 `*out` に代入している。

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
  if (!list->list->contains(id)) {
    return false;
  }
  const auto& val = list->list->get(id);
  if (val.type() != ControlTypeBool) {
    return false;
  }
  *out = val.get<bool>();
  return true;
```

`ControlList::get(unsigned int id)` は `const ControlValue&` を返す。
存在しない ID を渡した場合の挙動は未定義であり、`contains(id)` で事前に確認する必要がある。
現行実装はすべての getter で `contains(id)` と `type()` を事前に確認している。

### `std::optional` を返す API との区別

libcamera には `std::optional` を返す `get` があるが、これは別の関数である。

- `ControlValue::get<T>()`: 戻り値は `T`。値そのものを取り出す。存在しない値を表す経路は無い
- `ControlList::get(const Control<T>&)`: 戻り値は `std::optional<T>`。コントロールが存在しない場合に `std::nullopt` を返す
- `ControlList::get(unsigned int id)`: 戻り値は `const ControlValue&`。存在しない ID を渡した場合の挙動は未定義

本リポジトリは `ControlList::get(unsigned int id)` と `ControlValue::get<T>()` の組み合わせを使っている。
`ControlList::get(const Control<T>&)` は使っていない。

### 配列 getter

配列 getter 6 個は `data()` と `numElements()` を使っている。
`ControlValue::data()` の戻り値は v0.7.2 で `libcamera::Span` から `std::span` に変わったが、
本リポジトリは `auto` で受けているため影響を受けない。
この変更は 2026.2.0 の v0.7.2 追従 (C++ 標準を C++20 に変更したコミット) で取り込み済みである。

## 設計方針

### 変更対象

本 issue ではコードを変更しない。
確認の結果をこの issue に記録し、closed にする。

`c-api/`、`src/`、`examples/` はいずれも変更しない。
`CHANGES.md` への追記は行わない。C ABI と Rust API のシグネチャと挙動が変わらないためである。

### 確認した内容

- `ControlValue::get<T>()` が v0.6.0 から v0.8.0 まで `T` を返し続けていること
- `v0.7.2` と `v0.8.0` の `controls.h` が完全に一致していること
- 現行の 8 つのスカラー getter が v0.8.0 のヘッダーでそのままコンパイルできること
- 配列 getter が使う `data()` と `numElements()` が `std::span` 化の影響を受けないこと

## 完了条件

- `ControlValue::get<T>()` が v0.6.0 から v0.8.0 まで `T` を返しており、`std::optional` 化されていないことを確認している
- `v0.7.2` と `v0.8.0` の `controls.h` が完全に一致していることを確認している
- `c-api/controls.cpp` の 8 つのスカラー getter が v0.8.0 のヘッダーでそのままコンパイルできることを確認している
- `ControlList::get(const Control<T>&)` を本リポジトリが使っていないことを確認している

## 解決方法

コード変更を行わずに closed にする。理由は次のとおり。

- 起票時の目的は「libcamera v0.8.0+rpt20261104 で `ControlValue::get<T>()` の戻り値が `T` から `std::optional<T>` に変更されたことへの追従」であったが、これは事実誤認であった
- v0.6.0+rpt20251202 / v0.7.2+rpt20261009 / v0.8.0+rpt20261104 の `include/libcamera/controls.h` を確認したところ、`ControlValue` の `get<T>()` はすべて `T get() const` を返しており、`std::optional<T>` を返す実装は存在しない
- v0.7.2 と v0.8.0 の `controls.h` は内容が完全に一致しており、v0.8.0 で `controls.h` に起因するコンパイル差分は存在しない
- `std::optional<T>` を返すのは `ControlList::get(const Control<T>&)` という別の関数であり、v0.6.0 から存在する。本リポジトリはこの関数を使っていない
- 本リポジトリは `ControlList::get(unsigned int id)` と `ControlValue::get<T>()` の組み合わせを使っており、`contains(id)` と `type()` の事前チェックによって失敗経路を既に網羅している
- 現行の `c-api/controls.cpp` は 2026.2.0 の v0.7.2 追従で同一のヘッダーに対してビルドと CI を通しており、v0.8.0 でもそのままコンパイルできる。したがって v0.8.0 追従として本 issue で対応すべき作業は存在しない
- 配列 getter が使う `data()` の `libcamera::Span` から `std::span` への変更は、2026.2.0 の v0.7.2 追従で既に取り込み済みである
