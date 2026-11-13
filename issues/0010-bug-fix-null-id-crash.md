# lc_camera_manager_get_camera_by_id に nullptr を渡すとクラッシュする

- Created: 2026-11-13
- Completed: {YYYY-MM-DD}
- Branch: feature/fix-null-id-crash
- Polished: {YYYY-MM-DD}

## 目的

`c-api/camera_manager.h` の `lc_camera_manager_get_camera_by_id` は `const char* id` を受け取る C ABI であるが、
`id` に `nullptr` を渡すと `std::string_view` の構築で `std::char_traits::length` がヌルポインタを辿り、
セグメンテーションフォルトでプロセスが落ちる。

C ABI を直接使う C / C++ 利用者は、ID が見つからなかった場合などに `nullptr` を渡しうる。
`nullptr` を渡したときにクラッシュせず「見つからなかった」として `nullptr` を返すのが、
他の `lc_camera_manager_*` 関数と揃った振る舞いである。

## 現状

`c-api/camera_manager.cpp` の `lc_camera_manager_get_camera_by_id` は `id` を検証せずに `std::string_view` へ渡している。

```cpp
lc_camera_t* lc_camera_manager_get_camera_by_id(lc_camera_manager_t* mgr,
                                                const char* id) {
  auto camera = mgr->cm->get(id);
  if (!camera) {
    return nullptr;
  }
  auto cam = new lc_camera();
  cam->camera = camera;
  return cam;
}
```

`CameraManager::get()` は `std::string_view` を受け取る。
`std::string_view` の `const CharT*` を受け取るコンストラクタは、後置条件として
`[s, s + traits::length(s))` を満たす範囲を要求し、`nullptr` の検査を行わない。
そのため `nullptr` を渡すと `std::char_traits::length(nullptr)` が呼ばれて未定義動作になる。

次のコードを `c++ -std=c++20 -O0` でビルドして実行すると、警告なしでコンパイルされたうえで
終了コード 139 (SIGSEGV) で落ちることを確認した。

```cpp
#include <string_view>
#include <cstdio>

int main() {
  const char* p = nullptr;
  std::string_view sv(p);
  std::printf("len=%zu\n", sv.size());
  return 0;
}
```

本リポジトリの Rust API からは到達しない。
`src/camera_manager.rs` の `CameraManager::get_camera_by_id` は `std::ffi::CString::new(id)` で `CString` を作り、
その `as_ptr()` を渡すため、`nullptr` にはならない。

影響を受けるのは `lc_camera_manager_get_camera_by_id` を直接呼ぶ C / C++ の利用者だけである。

同じ `c-api/` の他の関数も `mgr` / `cam` などのポインタ引数を検証していないが、
本 issue は `id` の `nullptr` によって**クラッシュが実際に発生する**点に限定して扱う。

## 設計方針

- `lc_camera_manager_get_camera_by_id` の先頭で `id == nullptr` を判定し、`nullptr` を返す
- 戻り値を変えない。既存の「ID が見つからない場合に `nullptr` を返す」契約に、`id == nullptr` を同じ扱いとして加える
- `lc_camera_manager.h` の宣言とシグネチャは変えない
- Rust 側の `CameraManager::get_camera_by_id` の挙動は変えない。`CameraNotFound` を返す既存の契約を維持する
- コメントは日本語で書く。`nullptr` を渡すと `std::string_view` の構築で未定義動作になることを、
  修正理由としてコードコメントに残す

## 完了条件

- `lc_camera_manager_get_camera_by_id` が `id == nullptr` の場合に `nullptr` を返す
- `c-api/camera_manager.h` の宣言とシグネチャが変わっていない
- `cargo clippy --workspace --all-targets --target aarch64-unknown-linux-gnu -- -D warnings` が通る
- `cargo fmt --all -- --check` が通る
- 実機 Raspberry Pi 上で `list_cameras` サンプルが従来どおり動作する

## 解決方法

- `c-api/camera_manager.cpp` の `lc_camera_manager_get_camera_by_id` の先頭に、次のガードを追加する

```cpp
  if (id == nullptr) {
    return nullptr;
  }
```

- ガードの直前に、`std::string_view` が `nullptr` を受け付けず未定義動作になることを説明するコメントを日本語で書く
- `CHANGES.md` の `develop` セクションに `[FIX]` として追記する。C ABI を直接使う利用者から見て挙動が変わるため
