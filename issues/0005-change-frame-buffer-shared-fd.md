# FrameBuffer::Plane::fd の SharedFD 化に追従し libcamera-base を明示リンクする

- Created: 2026-11-13
- Completed: {YYYY-MM-DD}
- Branch: feature/change-frame-buffer-shared-fd
- Polished: {YYYY-MM-DD}

## 目的

libcamera v0.8.0+rpt20261104 では `include/libcamera/framebuffer.h` の `FrameBuffer::Plane` の `fd` が
`int` から `SharedFD` (RAII なファイルディスクリプタ) に変わった。
`SharedFD` のメソッドは `libcamera-base.so.0.8` で定義される別ライブラリのシンボルであり、
本リポジトリの `build.rs` は `libcamera` と `stdc++` しかリンク指定していないため、
`SharedFD` のメソッドを呼ぶコードを書いた時点でリンクエラーになる。

現状のコードは `planes[index].fd.get()` を呼んでおり、v0.8.0 では `SharedFD::get()` を呼ぶことになるため、
この変更は必ずリンク設定の問題を引き起こす。`build.rs` のリンク設定を v0.8.0 の構成に合わせる必要がある。

## 現状

`build.rs` は `pkg_config::Config::new().probe("libcamera")` の結果を使い、
`cargo::rustc-link-lib=stdc++` だけを追加で出力している。`libcamera-base` は明示的にリンクしていない。

`c-api/frame_buffer.cpp` の `lc_frame_buffer_get_plane` は次のようにプレーン情報を取得している。

```cpp
  plane.fd = planes[index].fd.get();
  plane.offset = planes[index].offset;
  plane.length = planes[index].length;
```

v0.7.2 までは `fd` が `int` だったため、この呼び出しは libcamera-base のシンボルに依存しなかった。
v0.8.0 で `fd` が `SharedFD` になると、`.get()` は libcamera-base のシンボルを参照する。

Rust 側では `src/frame_buffer.rs` の `FrameBufferPlane.fd` が `i32` のまま公開されており、
`FrameBuffer::plane` と `FrameBufferRef::plane` が `fd < 0` を失敗条件として扱っている。

`sysroot/raspberry-pi-os_armv8.json` のパッケージ一覧は `libc6-dev` / `libstdc++-14-dev` / `libcamera-dev` であり、
`libcamera-base` の共有ライブラリは `libcamera-dev` に含まれる。

## 設計方針

- `build.rs` で `libcamera-base` を明示的にリンクする。`pkg-config` で `libcamera` を検出した結果に加えて、
  共有ライブラリとして `libcamera-base` を `cargo::rustc-link-lib=dylib=libcamera-base` で指定する
- クロスコンパイル時は `sysroot` 配下の `usr/lib/aarch64-linux-gnu` を探索パスに追加しているため、
  `libcamera-base.so` がそのディレクトリに存在することを前提にする
- `c-api/frame_buffer.cpp` では `SharedFD` から fd の数値を取り出す。`SharedFD` が有効かどうかを
  `isValid()` で判定し、無効な場合は既存の失敗表現 (`fd = -1`) に揃える
- Rust 側の `FrameBufferPlane.fd` が `i32` のままであることは変えない。fd の寿命管理は libcamera 側にあり、
  本リポジトリは数値を参照するだけという現在の契約を維持する
- `SharedFD` の寿命は `FrameBuffer::planes()` が返す view に紐づく。C ABI は数値だけを返すため、
  呼び出し側で fd を保持し続ける使い方をしないことを README などで明記するかを実装時に判断する

## 完了条件

- `build.rs` が `libcamera-base` をリンクし、aarch64 向けのビルドがリンクエラーなく完了する
- `lc_frame_buffer_get_plane` が v0.8.0 の `SharedFD` から正しい fd / offset / length を取得できる
- `FrameBuffer::plane` / `FrameBufferRef::plane` が無効なプレーンに対して `None` を返す挙動を維持する
- 実機 Raspberry Pi 上で `capture` サンプルがフレームを取得できる
- `cargo clippy --workspace --all-targets --target aarch64-unknown-linux-gnu -- -D warnings` が通る

## 解決方法

- `build.rs` に `println!("cargo::rustc-link-lib=dylib=libcamera-base");` を追加し、
  クロスコンパイル時の `rustc-link-search` が `libcamera-base.so` の配置先を指していることを確認する
- `c-api/frame_buffer.cpp` の `lc_frame_buffer_get_plane` で `planes[index].fd.isValid()` を判定し、
  有効な場合に `.get()` で数値を取り出す
- `libcamera-base` が pkg-config で別途検出できる場合は `pkg_config::Config::new().probe("libcamera-base")` を使う方が
  バージョン整合の面で安全か検討する。`libcamera-base.pc` の有無は sysroot 上で確認してから決める
