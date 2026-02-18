# libcamera-rs

[![shiguredo_libcamera](https://img.shields.io/crates/v/shiguredo_libcamera.svg)](https://crates.io/crates/shiguredo_libcamera)
[![Documentation](https://docs.rs/shiguredo_libcamera/badge.svg)](https://docs.rs/shiguredo_libcamera)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

## About Shiguredo's open source software

We will not respond to PRs or issues that have not been discussed on Discord. Also, Discord is only available in Japanese.

Please read <https://github.com/shiguredo/oss> before use.

## 時雨堂のオープンソースソフトウェアについて

利用前に <https://github.com/shiguredo/oss> をお読みください。

## 対応 libcamera バージョン

> [!IMPORTANT]
> **libcamera v0.7.0+rpt20260205**
>
> libcamera は動的リンクで利用しています。上記バージョンと互換性のない libcamera がインストールされている環境では正常に動作しない可能性があります。

## 概要

Rust で実装された [libcamera](https://libcamera.org/) のバインディングです。

## 特徴

- libcamera 本体は `pkg-config` 経由で動的リンク
- 自前 C ラッパーは静的コンパイルしてクレートに埋め込み
- 安全な Rust API による所有権・ライフタイム管理
- コールバックによるフレームキャプチャ
- コントロール (露出、ホワイトバランス、フォーカス等) の取得・設定 ([対応コントロール一覧](docs/CONTROLS.md))

## 構成

```
libcamera-rs/
├── c-api/       # C/C++ ラッパー (libcamera C++ API → C API)
├── src/         # 高レベル safe Rust API + FFI (bindgen 生成)
└── examples/    # サンプル
```

## 動作環境

- Raspberry Pi OS で libcamera が動作する環境
- `libcamera-dev` パッケージが必要

```bash
sudo apt install libcamera-dev
```

## 使い方

### カメラの列挙

```rust
use shiguredo_libcamera::{CameraManager, StreamRole};

let manager = CameraManager::new().unwrap();
let count = manager.cameras_count();
for i in 0..count {
    let camera = manager.get_camera(i).unwrap();
    println!("Camera {i}: {}", camera.id());
}
```

### フレームキャプチャ

```rust
use shiguredo_libcamera::{
    CameraManager, FrameBufferAllocator, StreamRole,
};

let manager = CameraManager::new().unwrap();
let mut camera = manager.get_camera(0).unwrap();
camera.acquire().unwrap();

let mut config = camera.generate_configuration(&[StreamRole::VideoRecording]).unwrap();
config.validate().unwrap();
camera.configure(&mut config).unwrap();

let stream = config.at(0).unwrap().stream().unwrap();
let allocator = FrameBufferAllocator::new(&camera);
let buffer_count = allocator.allocate(&stream).unwrap();

camera.on_request_completed(move |completed| {
    if let Some(buffer) = completed.find_buffer(&stream) {
        let meta = buffer.metadata();
        println!("Frame {}: timestamp={}", meta.sequence, meta.timestamp);
    }
});

camera.start().unwrap();
// ...
camera.stop().unwrap();
```

### コントロールの設定

```rust
use shiguredo_libcamera::core;

let mut controls = request.controls();
controls.set_f32(&core::BRIGHTNESS, 0.2);
controls.set_f32(&core::CONTRAST, 1.5);
controls.set_f32(&core::SATURATION, 1.2);
```

## サンプル

サンプルの JSON 出力には [nojson](https://github.com/sile/nojson) を利用しています。

### list_cameras

カメラ一覧とストリーム情報を表示します。

```bash
cargo run --example list_cameras
```

### capture

フレームキャプチャを行い、メタデータを表示します。

```bash
cargo run --example capture
```

### controls

コントロールを設定してキャプチャし、メタデータを読み取ります。

```bash
cargo run --example controls
```

## ライセンス

Apache License 2.0

```text
Copyright 2026-2026, Shiguredo Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```
