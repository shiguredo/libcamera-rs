# FrameBuffer::Plane のプレーン取得が SharedFD の契約に適合しているか確認する

- Created: 2026-11-13
- Completed: 2026-11-13
- Branch: feature/change-frame-buffer-shared-fd
- Polished: {YYYY-MM-DD}

## 目的

libcamera v0.8.0+rpt20261104 への追従にあたり、`FrameBuffer` のプレーン取得経路が
`SharedFD` の契約と整合しているかを確認する。

`FrameBuffer::Plane::fd` は libcamera v0.6.0 以降 `SharedFD` (RAII なファイルディスクリプタ) であり、
本リポジトリは v0.7.2 で既にこの型を受けている。
`c-api/frame_buffer.cpp` の `lc_frame_buffer_get_plane` は `planes[index].fd.get()` を呼んでおり、
`SharedFD::get()` は無効なインスタンスに対して `-1` を返す。
Rust 側の `FrameBuffer::plane` と `FrameBufferRef::plane` は `fd < 0` を失敗条件として `None` を返すため、
無効なプレーンは現状でも `None` として扱われる。

つまり v0.8.0 で `SharedFD` 化が新たに起きるわけではなく、
本 issue は「現行実装が `SharedFD` の契約に適合しているかを確認し、確認結果をコードコメントに残す」ことを目的とする。

## 現状

`c-api/frame_buffer.cpp` の `lc_frame_buffer_get_plane` は次のようにプレーン情報を取得している。

```cpp
  plane.fd = planes[index].fd.get();
  plane.offset = planes[index].offset;
  plane.length = planes[index].length;
```

`SharedFD` は `include/libcamera/base/shared_fd.h` で次のように定義されている。

```cpp
	bool isValid() const { return fd_ != nullptr; }
	int get() const { return fd_ ? fd_->fd() : -1; }
	UniqueFD dup() const;
```

`isValid()` と `get()` はいずれもヘッダー内のインライン定義であり、
外部ライブラリのシンボルを参照しない。したがって `get()` を呼んでもリンク依存は増えない。

`build.rs` が出力するリンク関連ディレクティブは次のとおり。

- `pkg_config::Config::new().probe("libcamera")` 由来の `rustc-link-lib` と `rustc-link-search`
- `cargo::rustc-link-lib=stdc++`
- クロスコンパイル時の `cargo::rustc-link-search=native={sysroot}/usr/lib/aarch64-linux-gnu`

v0.8.0 の `src/libcamera/meson.build` は `pkg_mod.generate(libcamera, libraries : libcamera_base_lib, ...)` を呼び、
生成される `libcamera.pc` は `Requires: libcamera-base` を持つ。
`Requires:` はリンク時に伝播するため、`probe("libcamera")` の時点で `libcamera-base` もリンク指定に含まれる。
`build.rs` に `libcamera-base` の明示リンクを追加する必要はない。

`FrameBuffer::Plane::fd` の型は次のとおりで、v0.8.0 固有の変化はない。

| タグ | `FrameBuffer::Plane` の `fd` |
|---|---|
| v0.6.0+rpt20251202 | `SharedFD` |
| v0.7.0+rpt20260205 | `SharedFD` |
| v0.7.2+rpt20261009 | `SharedFD` |
| v0.8.0+rpt20261104 | `SharedFD` |

`examples/capture.rs` は `buffer.metadata()` のみを使い、`FrameBuffer::plane` を呼んでいない。
そのため `capture` の成否はプレーン取得の検証にはならない。

## 設計方針

### 変更対象

変更対象は `c-api/frame_buffer.cpp` のみとする。
`build.rs` は変更しない。`examples/` は変更しない。
`README.md` と `skills/shiguredo-libcamera/SKILL.md` の更新はドキュメント更新の issue が扱うため、本 issue では扱わない。

### 確認内容

- `SharedFD::get()` が無効時に `-1` を返すことを根拠に、`lc_frame_buffer_get_plane` が
  既に無効プレーンを `fd = -1` として表現できていることを確認する
- `isValid()` による分岐を追加するかは、`get()` が既に `-1` を返すため挙動は変わらない。
  可読性のために `isValid()` を使う形に整えることは許容するが、挙動を変えないこと
- 上記の根拠 (インライン定義であること、無効時に `-1` を返すこと) をコードコメントに日本語で残す。
  根拠資料名 `libcamera/base/shared_fd.h` をコメントに明記する
- `build.rs` に `libcamera-base` の明示リンクを追加しない。理由は `libcamera.pc` の `Requires:` により既に伝播しているため

### 検証方法

`FrameBuffer::plane` は実機のカメラと libcamera の実挙動に依存するため、
`tests/` や `pbt/` の新設は行わない。検証は次の手順で行う。

- aarch64 向けのビルドと clippy が通ること
- 実機で `FrameBuffer::plane` が `Some` を返し、`fd` が 0 以上であることを手動で確認すること

### ドキュメント

本 issue は公開 API のシグネチャと挙動を変えないため、`CHANGES.md` への追記は行わない。
v0.8.0 追従全体の変更履歴はドキュメント更新の issue が扱う。

## 完了条件

- `c-api/frame_buffer.cpp` の `lc_frame_buffer_get_plane` に、`SharedFD` のインライン定義と無効時の `-1` 返却を根拠とするコメントが日本語で記載されている
- `build.rs` が変更されていない (`libcamera-base` の明示リンクが追加されていない)
- `cargo clippy --workspace --target aarch64-unknown-linux-gnu -- -D warnings` が通る
- 実機 Raspberry Pi 上で `FrameBuffer::plane` が `Some` を返すことを手動で確認している

## 解決方法

コード変更を行わずに closed にする。理由は次のとおり。

- 起票時の目的は「libcamera v0.8.0+rpt20261104 で `FrameBuffer::Plane::fd` が `int` から `SharedFD` に変わったことへの追従」であったが、これは事実誤認であった
- `FrameBuffer::Plane` の `fd` は v0.6.0+rpt20251202 / v0.7.0+rpt20260205 / v0.7.2+rpt20261009 / v0.8.0+rpt20261104 のすべてで `SharedFD` であり、v0.8.0 で変化していない
- `git log -S "planes[index].fd" -- c-api/frame_buffer.cpp` の結果は初回インポートの 1 件のみであり、`c-api/frame_buffer.cpp` のプレーン取得コードは v0.7.2 への追従でも変更されていない。現行コードは v0.7.2 で `SharedFD::get()` を既に呼んでいる
- 起票時に想定した「`build.rs` に `libcamera-base` を明示リンクする」作業は不要である。`isValid()` と `get()` は `include/libcamera/base/shared_fd.h` 内のインライン定義であり外部シンボルを参照しない。また v0.8.0 の `src/libcamera/meson.build` は `pkg_mod.generate(libcamera, libraries : libcamera_base_lib, ...)` を呼び、`libcamera.pc` の `Requires: libcamera-base` を経由して `pkg_config` の `probe("libcamera")` が `libcamera-base` のリンク指定も出力する
- `libcamera-base` の実行時解決の確認は、CI に `ldd` による検証を追加する issue が担当する
- 無効なプレーンを `FrameBuffer::plane` が `None` として返す挙動は、`SharedFD::get()` が無効時に `-1` を返すことと `src/frame_buffer.rs` の `fd < 0` 判定によって既に成立しており、追加の変更は不要である
- 関連するコメントの補足は可読性の改善であり、v0.8.0 追従には不要であるため、必要になった時点で改めて起票する
