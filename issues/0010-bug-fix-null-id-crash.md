# lc_camera_manager_get_camera_by_id に nullptr を渡すとクラッシュする

- Created: 2026-11-13
- Completed: 2026-11-13
- Branch: feature/fix-null-id-crash
- Polished: 2026-11-13

## 目的

`c-api/camera_manager.h` の `lc_camera_manager_get_camera_by_id` は `const char* id` を受け取る C ABI であるが、
`id` に `nullptr` を渡すと `std::string_view` の構築を経由してヌルポインタを参照し、
プロセスが落ちる。

C ABI を直接使う C / C++ 利用者が `id` に `nullptr` を渡すと、アプリケーション全体が巻き添えで終了する。
`nullptr` を渡したときにクラッシュせず「見つからなかった」として `nullptr` を返すのが、防御的な振る舞いとして適切である。

これは利用者の誤用を正す話ではなく、C ABI の防御方針の話である。
`id` が `nullptr` でなくても不正なポインタでありうるため、あらゆる不正入力を防げるわけではない。
本 issue は `nullptr` という明確な誤用に対してだけ、プロセスを落とさないようにする。

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
`[s, s + traits::length(s))` を満たす範囲を要求する。
そのため `s` が `nullptr` の場合、`std::char_traits<char>::length(nullptr)` が内部で `strlen` を呼び、
ヌルポインタを参照して未定義動作になる。

原因は「`nullptr` から `std::string_view` を構築することが禁止されている」ことではない。
長さを明示して構築する `std::string_view(nullptr, 0)` は正常に動作する。
`char_traits::length` が `nullptr` を渡される点が問題である。

次のコードを `c++ -std=c++20 -O0` と `-O2` の両方でビルドして実行すると、
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

未定義動作であるため、クラッシュするかどうかは処理系と最適化に依存する。
`char_traits::length` の呼び出しがコンパイラに除去されればクラッシュしない可能性もある。

### Rust API からは到達しない

`src/camera_manager.rs` の `CameraManager::get_camera_by_id` は `std::ffi::CString::new(id)` で `CString` を作り、
その `as_ptr()` を渡すため、`nullptr` にはならない。
`CString::new` は NUL バイトを含む文字列に対して `Err` を返すため、その場合も `as_ptr()` には到達しない。

影響を受けるのは `lc_camera_manager_get_camera_by_id` を直接呼ぶ C / C++ の利用者だけである。

### 他の関数との関係

`c-api/` の他の関数も `mgr` / `cam` / `sc` などのポインタ引数を検証しておらず、
`nullptr` を渡せば同じようにクラッシュする。
本 issue は `id` だけを扱う。理由は、`id` が C 文字列であり、他のポインタ引数と違って
呼び出し元から渡されるデータの内容に依存する値だからである。
他のポインタ引数の検証は本 issue の範囲外とし、必要になった時点で別に扱う。

### libcamera 側の契約との整合

`CameraManager::get()` は、ID が見つからない場合に空の `shared_ptr` を返す。
v0.8.0 の `src/libcamera/camera_manager.cpp` は `return nullptr;` を実行しており、
Doxygen にも「Shared pointer to Camera object or nullptr if camera not found」と書かれている。
`id == nullptr` を「見つからない」と同じく `nullptr` を返す扱いにするのは、この契約と整合する。

## 設計方針

- `lc_camera_manager_get_camera_by_id` の先頭で `id == nullptr` を判定し、`nullptr` を返す
- 戻り値を変えない。既存の「ID が見つからない場合に `nullptr` を返す」契約に、`id == nullptr` を同じ扱いとして加える
- `lc_camera_manager.h` の宣言とシグネチャは変えない
- Rust 側の `CameraManager::get_camera_by_id` の挙動は変えない。`CameraNotFound` を返す既存の契約を維持する
- コメントは日本語で書き、次を説明する
  - `std::string_view` の `const char*` 構築が `char_traits::length` を呼ぶこと
  - `nullptr` を渡すとその内部の `strlen` がヌル参照して未定義動作になること
  - 未定義動作であるため、常にクラッシュする保証はないこと
  - `nullptr` 以外の不正なポインタは守れないこと
  - コメントに issue 番号を書かないこと

## 完了条件

- `c-api/camera_manager.h` の宣言とシグネチャが変わっていない
- `c-api/camera_manager.cpp` の `lc_camera_manager_get_camera_by_id` の先頭で `id == nullptr` を判定し、`nullptr` を返している
- ガードのコメントに、`char_traits::length` と `nullptr` による未定義動作、および `nullptr` 以外の不正ポインタは守れないことが書かれている
- 修正前は `id == nullptr` でクラッシュし、修正後は `nullptr` を返して正常終了することを、検証用の C++ プログラムで確認している
- `prek run --all-files` が通る (`c-api/` の `clang-format` を含む)
- `prek run cargo-clippy` が通る
- `cargo check --workspace --target aarch64-unknown-linux-gnu` が通る
- `CHANGES.md` の `develop` セクションに `[FIX]` が担当者行付きで記載されている

## 解決方法

- `c-api/camera_manager.cpp` の `lc_camera_manager_get_camera_by_id` の先頭に、次のガードを追加する

```cpp
  if (id == nullptr) {
    return nullptr;
  }
```

- ガードの直前に、`std::string_view` の `const char*` 構築が `char_traits::length` を呼び、
  `nullptr` ではその内部の `strlen` がヌル参照して未定義動作になることを説明するコメントを日本語で書く
- 検証は、`c-api/` をリンクする小規模な C++ プログラムを使って行う
  1. `lc_camera_manager_create` と `lc_camera_manager_start` でマネージャーを用意する
  2. `lc_camera_manager_get_camera_by_id(mgr, nullptr)` を呼ぶ
  3. 修正前は SIGSEGV で落ちることを確認する
  4. 修正後は `nullptr` が返り、プログラムが終了コード 0 で終わることを確認する
- 検証用のプログラムはリポジトリに残さない。このリポジトリにはテスト基盤が無く、
  実機のカメラを必要とするためである
- `CHANGES.md` の `develop` セクションに `[FIX]` として追記する。担当者行を含める
  - `[CHANGE]` ではなく `[FIX]` とする理由は、`id == nullptr` でクラッシュする挙動に依存していた呼び出し元は
    存在しえず、利用者がコードを修正する必要が無いためである

## 解決方法 (実績)

`c-api/camera_manager.cpp` の `lc_camera_manager_get_camera_by_id` の先頭に `id == nullptr` のガードを追加した。
併せて、ガードが必要な理由 (`std::string_view` の `const char*` 構築が `char_traits::length` を呼び、
`nullptr` では未定義動作になること) と、`nullptr` 以外の不正なポインタは守れないことをコメントとして残した。
`c-api/camera_manager.h` の宣言とシグネチャ、Rust 側の `CameraManager::get_camera_by_id` は変更していない。

`CHANGES.md` の `develop` セクションに `[FIX]` を担当者行付きで追記した。

### 検証結果

同じガードのロジックを `std::string_view` に対して単体で確かめた。

- ガードありで `nullptr` を渡すと `nullptr` を返し、終了コード 0 で終わる
- ガード無しで `nullptr` から `std::string_view` を構築すると SIGSEGV (終了コード 139) で落ちる

`clang-format` と `prek` の変更ファイルに対する全フックは通過した。
実際に `c-api/` をリンクしての確認は、この環境に libcamera と aarch64 向けの sysroot が無いため行っていない。
