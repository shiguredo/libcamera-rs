---
name: shiguredo-libcamera
description: 時雨堂の libcamera Rust バインディング shiguredo_libcamera の機能・API リファレンス。カメラ列挙、ストリーム設定、フレームキャプチャ、コントロール (露出・ホワイトバランス・フォーカス等) の取得・設定、コールバックによるフレーム完了通知に関する質問時に使用。
---

# shiguredo_libcamera

[libcamera](https://libcamera.org/) C++ ライブラリの Rust バインディング。安全な所有権・ライフタイム管理を提供する高レベル API と、bindgen で生成した FFI、自前 C++ → C ラッパー層で構成される。

## 特徴

- **libcamera は動的リンク**: `pkg-config` 経由でシステムの `libcamera` を参照
- **自前 C ラッパーは静的リンク**: 同梱の C++ → C ラッパーは `cc` でビルドしてクレートに埋め込み
- **安全な Rust API**: `unsafe` は内部のみ、利用側コードは safe
- **コールバックによるフレームキャプチャ**: `on_request_completed()` でクロージャを登録
- **コントロール対応**: 露出・ホワイトバランス・フォーカス等の取得・設定 (対応コントロール一覧ドキュメントを参照)
- **Send + Sync**: 主要型は `Send`、必要に応じて `Sync` も実装

## バージョン情報

- crate 名: `shiguredo_libcamera`
- バージョン: 2026.1.0
- Rust Edition: 2024
- 最小 Rust バージョン: 1.88
- ライセンス: Apache-2.0
- 対応 libcamera: **v0.7.0+rpt20260205** (動的リンク先のバージョンが互換でないと動作しない可能性あり)

## 動作環境

- Raspberry Pi OS など、libcamera が動作する環境
- `libcamera-dev` パッケージが必要 (`sudo apt install libcamera-dev`)
- macOS arm64 でも Dev Containers を使えばクロスコンパイル可能 (Dev Container セットアップガイドを参照)

## 設計概要

ライブラリは以下の層に分かれる。

| 層 | 役割 |
|----|------|
| C++ ラッパー (同梱の C/C++ ソース) | libcamera C++ API を C ABI に変換 |
| FFI バインディング (`sys` モジュール, 非公開, `bindgen` 生成) | C ヘッダーから自動生成された Rust 宣言 |
| safe Rust API (公開モジュール) | 所有権・ライフタイム・Result でラップ |
| サンプル | API の利用例 |

C++ ラッパーは静的にコンパイルされて crate に埋め込まれるため、利用側は libcamera 本体のみ動的リンクすればよい。

## コア API

### CameraManager

libcamera の `CameraManager` を保持し、新規作成時に `start()` まで実行する所有型。`Drop` で `stop()` + `destroy()` を呼ぶ。

| メソッド | 戻り値 | 説明 |
|----------|--------|------|
| `CameraManager::new()` | `Result<Self>` | マネージャーを作成して開始する。失敗時は `Error::StartFailed` |
| `cameras_count()` | `usize` | 検出されたカメラの数 |
| `get_camera(index)` | `Result<Camera>` | インデックスでカメラを取得 (`Error::IndexOutOfRange`) |
| `get_camera_by_id(id)` | `Result<Camera>` | ID 文字列でカメラを取得 (`Error::CameraNotFound`) |

`Send + Sync` 実装あり。

### Camera

libcamera の `shared_ptr<Camera>` を保持する所有型。`Drop` で参照を解放する (`lc_camera_release_ref`)。

| メソッド | 戻り値 | 説明 |
|----------|--------|------|
| `id()` | `&str` | カメラ ID |
| `acquire()` | `Result<()>` | カメラを取得 (排他使用権を確保) |
| `release()` | `Result<()>` | カメラを解放 |
| `generate_configuration(&[StreamRole])` | `Result<CameraConfiguration>` | ストリームロールから設定を生成 |
| `configure(&mut CameraConfiguration)` | `Result<()>` | カメラに設定を適用 |
| `create_request(cookie)` | `Result<Request>` | 任意の `cookie: u64` 付きでリクエストを作成 |
| `properties()` | `ControlList` | カメラのプロパティ (非所有 `ControlList`) |
| `queue_request(&Request)` | `Result<()>` | リクエストをキャプチャパイプラインに投入 |
| `start()` | `Result<()>` | キャプチャを開始 |
| `stop()` | `Result<()>` | キャプチャを停止 |
| `on_request_completed(F)` | `()` | リクエスト完了コールバックを登録 (`F: Fn(CompletedRequest<'_>) + Send + Sync + 'static`) |

`Send + Sync` 実装あり。コールバックは既存のものがあれば差し替えられ、`Drop` 時に切断・解放される。

### CameraConfiguration

`Camera::generate_configuration()` で生成される設定。`Drop` で破棄される。

| メソッド | 戻り値 | 説明 |
|----------|--------|------|
| `validate()` | `Result<ConfigStatus>` | 設定を検証 (`Valid` / `Adjusted` / `Invalid`)。`Invalid` は `Error::InvalidConfig` |
| `size()` | `usize` | ストリーム設定の数 |
| `at(index)` | `Result<StreamConfiguration<'_>>` | インデックスでストリーム設定への借用を取得 |

`ConfigStatus` enum: `Valid`, `Adjusted`, `Invalid`。

`Send` 実装あり (`Sync` なし)。

### StreamConfiguration

`CameraConfiguration::at()` で得られる、ストリーム単位の設定。`CameraConfiguration` の借用 (`PhantomData<&'a mut ()>`) で寿命を縛る。

| メソッド | 戻り値 | 説明 |
|----------|--------|------|
| `pixel_format()` / `set_pixel_format(PixelFormat)` | `PixelFormat` / `()` | ピクセルフォーマット |
| `size()` / `set_size(Size)` | `Size` / `()` | 出力サイズ |
| `stride()` | `u32` | ライン単位のバイト数 |
| `frame_size()` | `u32` | 1 フレームのバイト数 |
| `buffer_count()` / `set_buffer_count(u32)` | `u32` / `()` | リング上のバッファ数 |
| `stream()` | `Option<Stream>` | このストリーム設定に対応する `Stream` を取得 |

### Stream

`Camera` 内部データへの不透明参照 (所有権なし)。`Clone + Send + Sync`。複数のリクエストにまたがって使い回す。

### StreamRole

`Camera::generate_configuration()` に渡す用途種別。

| バリアント | 用途 |
|-----------|------|
| `Raw` | RAW センサーデータ |
| `StillCapture` | 静止画キャプチャ |
| `VideoRecording` | 動画録画 |
| `Viewfinder` | ファインダー (低解像度プレビュー) |

### FrameBufferAllocator

カメラ用フレームバッファを確保するアロケーター。`Drop` で破棄。

| メソッド | 戻り値 | 説明 |
|----------|--------|------|
| `FrameBufferAllocator::new(&Camera)` | `Self` | アロケーターを生成 |
| `allocate(&Stream)` | `Result<usize>` | バッファを割り当てて確保数を返す |
| `free(&Stream)` | `Result<()>` | バッファを解放 |
| `buffers_count(&Stream)` | `usize` | 確保済みバッファの数 |
| `get_buffer(&Stream, index)` | `Result<FrameBuffer>` | 指定インデックスのバッファを取得 |

### FrameBuffer / FrameBufferRef

`FrameBuffer` は所有型 (`Drop` で `lc_frame_buffer_ref_destroy`)、`FrameBufferRef<'a>` はコールバック内で使う非所有参照。両者とも同じ getter を持つ。

| メソッド | 戻り値 | 説明 |
|----------|--------|------|
| `planes_count()` | `usize` | プレーン数 |
| `plane(index)` | `Option<FrameBufferPlane>` | プレーン情報 (DMA-BUF の `fd`/`offset`/`length`) |
| `metadata()` | `FrameMetadata` | ステータス・シーケンス番号・タイムスタンプ |

`FrameBufferPlane`: `pub fd: i32`, `pub offset: u32`, `pub length: u32`。

`FrameMetadata`: `pub status: FrameStatus`, `pub sequence: u32`, `pub timestamp: u64`。

`FrameStatus` enum: `Success`, `Error`, `Cancelled`, `Startup`。

`FrameBuffer` は `Send + Sync`。

### Request / CompletedRequest

`Request` は `Camera::create_request()` で生成する所有型。`CompletedRequest<'a>` はコールバック (`on_request_completed`) 内で受け取る非所有参照。

| 型 | 主要メソッド |
|----|-------------|
| `Request` | `reuse()`, `add_buffer(&Stream, &FrameBuffer) -> Result<()>`, `status() -> RequestStatus`, `cookie() -> u64`, `sequence() -> u32`, `controls() -> ControlList`, `metadata() -> ControlList`, `find_buffer(&Stream) -> Option<FrameBuffer>` |
| `CompletedRequest<'a>` | `status() -> RequestStatus`, `cookie() -> u64`, `sequence() -> u32`, `metadata() -> ControlList`, `find_buffer(&Stream) -> Option<FrameBufferRef<'a>>` |

`RequestStatus` enum: `Pending`, `Complete`, `Cancelled`。

`Request` は `Send` (Sync なし)。`Drop` で `lc_request_destroy`。`controls()` / `metadata()` で返る `ControlList` は非所有 (`owned = false`)。

### ControlList

`Request::controls()` / `Request::metadata()` / `Camera::properties()` から取得する。コントロール値のコンテナ。

#### 取得 (`get_*`)

`id: &ControlId` を取り、対応する型で値を取得する。失敗時は `Err(Error::ControlGetFailed)`。

| メソッド | 戻り値 |
|----------|--------|
| `get_bool(&ControlId)` | `Result<bool>` |
| `get_u8(&ControlId)` | `Result<u8>` |
| `get_i32(&ControlId)` | `Result<i32>` |
| `get_i64(&ControlId)` | `Result<i64>` |
| `get_f32(&ControlId)` | `Result<f32>` |
| `get_string(&ControlId)` | `Result<&str>` |
| `get_size(&ControlId)` | `Result<Size>` |
| `get_rectangle(&ControlId)` | `Result<Rectangle>` |
| `get_u8_array(&ControlId)` | `Result<&[u8]>` |
| `get_i32_array(&ControlId)` | `Result<&[i32]>` |
| `get_i64_array(&ControlId)` | `Result<&[i64]>` |
| `get_f32_array(&ControlId)` | `Result<&[f32]>` |
| `get_rectangle_array(&ControlId)` | `Result<Vec<Rectangle>>` |
| `get_point_array(&ControlId)` | `Result<Vec<Point>>` |

その他: `size() -> usize`, `contains(&ControlId) -> bool`。

#### 設定 (`set_*`)

戻り値なし (libcamera 側でバリデーション)。

| メソッド | 引数 |
|----------|------|
| `set_bool(&ControlId, bool)` | |
| `set_u8(&ControlId, u8)` | |
| `set_i32(&ControlId, i32)` | |
| `set_i64(&ControlId, i64)` | |
| `set_f32(&ControlId, f32)` | |
| `set_rectangle(&ControlId, Rectangle)` | |
| `set_u8_array(&ControlId, &[u8])` | |
| `set_i32_array(&ControlId, &[i32])` | |
| `set_i64_array(&ControlId, &[i64])` | |
| `set_f32_array(&ControlId, &[f32])` | |
| `set_rectangle_array(&ControlId, &[Rectangle])` | |
| `set_point_array(&ControlId, &[Point])` | |

`Send` 実装あり (`Sync` なし)。

### ControlId

`const fn` で構築する不変なコントロール識別子。コントロール定数モジュール (`core` / `draft` / `rpi`) およびプロパティ定数モジュール (`properties::core` / `properties::draft`) に定義済みの定数を使う。

| メソッド | 戻り値 | 説明 |
|----------|--------|------|
| `ControlId::new(id, name, control_type, direction)` | `Self` (const) | 新規定義 |
| `id()` | `u32` | 数値 ID |
| `name()` | `&'static str` | コントロール名 |
| `control_type()` | `ControlType` | 値の型 |
| `direction()` | `Direction` | 入出力方向 |

`ControlType` enum: `None`, `Bool`, `Byte`, `Unsigned16`, `Unsigned32`, `Int32`, `Int64`, `Float`, `String`, `Rectangle`, `Size`, `Point`。

`Direction` enum: `In` (リクエスト時のみ), `Out` (メタデータのみ), `InOut` (両方)。

### コントロール定数

クレート直下で `core` / `draft` / `rpi` の 3 モジュールとして再公開されている。

| モジュール | 名前空間 | 主要定数 |
|-----------|---------|---------|
| `core` | libcamera 標準 (base = 0, ID 1..52) | `AE_ENABLE`, `EXPOSURE_TIME`, `ANALOGUE_GAIN`, `BRIGHTNESS`, `CONTRAST`, `SATURATION`, `SHARPNESS`, `AWB_ENABLE`, `AWB_MODE`, `COLOUR_GAINS`, `COLOUR_TEMPERATURE`, `AF_MODE`, `AF_TRIGGER`, `LENS_POSITION`, `HDR_MODE`, `GAMMA`, `WDR_MODE`, `FRAME_DURATION_LIMITS`, `SENSOR_TIMESTAMP` 等 |
| `draft` | ドラフト (base = 10000) | `AE_PRECAPTURE_TRIGGER`, `NOISE_REDUCTION_MODE`, `AWB_STATE`, `TEST_PATTERN_MODE`, `FACE_DETECT_MODE`, `FACE_DETECT_FACE_RECTANGLES` 等 |
| `rpi` | Raspberry Pi 拡張 (base = 20000) | `STATS_OUTPUT_ENABLE`, `BCM2835_STATS_OUTPUT`, `SCALER_CROPS`, `SYNC_MODE`, `SYNC_READY`, `CNN_OUTPUT_TENSOR` 等 |

各モジュール配下には enum 値用のサブモジュールが定義されている。例: `core::ae_state::{IDLE, SEARCHING, CONVERGED}`, `core::awb_mode::{AUTO, INCANDESCENT, DAYLIGHT, ...}`, `core::af_mode::{MANUAL, AUTO, CONTINUOUS}`, `rpi::sync_mode::{OFF, SERVER, CLIENT}` 等。

### プロパティ定数

`properties::core` / `properties::draft` として公開されているカメラの読み取り専用プロパティ用の定数群。

| モジュール | 主要定数 |
|-----------|---------|
| `properties::core` | `LOCATION`, `ROTATION`, `MODEL`, `UNIT_CELL_SIZE`, `PIXEL_ARRAY_SIZE`, `PIXEL_ARRAY_ACTIVE_AREAS`, `SCALER_CROP_MAXIMUM`, `SENSOR_SENSITIVITY`, `SYSTEM_DEVICES` |
| `properties::draft` | `COLOR_FILTER_ARRANGEMENT` |

enum 値: `properties::core::location::{FRONT, BACK, EXTERNAL}`, `properties::draft::color_filter_arrangement::{RGGB, GRBG, GBRG, BGGR, RGB, MONO}`。

### Geometry

ピクセル単位のジオメトリ型。すべて `Copy + Eq`。

| 型 | フィールド | コンストラクタ |
|----|-----------|---------------|
| `Rectangle` | `x: i32`, `y: i32`, `width: u32`, `height: u32` | `Rectangle::new(x, y, w, h)` |
| `Size` | `width: u32`, `height: u32` | `Size::new(w, h)` |
| `Point` | `x: i32`, `y: i32` | `Point::new(x, y)` |

### PixelFormat

DRM fourcc + modifier の組。

| メソッド | 戻り値 | 説明 |
|----------|--------|------|
| `PixelFormat::new(fourcc, modifier)` | `Self` | 完全指定 |
| `PixelFormat::from_fourcc(fourcc)` | `Self` | modifier = 0 |

`Display` 実装あり: ASCII 表示可能なら 4 文字、modifier がある場合は `XXXX/0x<hex>`。

### エラー型

`Error` enum (variants):

| バリアント | 説明 |
|-----------|------|
| `StartFailed` | カメラマネージャーの開始失敗 |
| `AcquireFailed` | `acquire()` / `release()` 失敗 |
| `ConfigureFailed` | `configure()` 失敗 |
| `GenerateConfigFailed` | `generate_configuration()` 失敗 |
| `InvalidConfig` | `validate()` が Invalid |
| `CreateRequestFailed` | `create_request()` 失敗 |
| `QueueRequestFailed` | `queue_request()` 失敗 |
| `CameraStartFailed` | `start()` 失敗 |
| `CameraStopFailed` | `stop()` 失敗 |
| `AllocateFailed` | バッファ割り当て失敗 |
| `FreeFailed` | バッファ解放失敗 |
| `AddBufferFailed` | `Request::add_buffer()` 失敗 |
| `IndexOutOfRange` | インデックスが範囲外 |
| `CameraNotFound` | ID 指定でカメラが見つからない |
| `ControlGetFailed` | コントロール値の取得失敗 |

`std::error::Error` と `Display` を実装。`Result<T> = std::result::Result<T, Error>` のエイリアスあり。

## コード例

### カメラの列挙

```rust
use shiguredo_libcamera::CameraManager;

let manager = CameraManager::new()?;
let count = manager.cameras_count();
for i in 0..count {
    let camera = manager.get_camera(i)?;
    println!("Camera {i}: {}", camera.id());
}
```

### フレームキャプチャ (最小構成)

```rust
use std::sync::{Arc, Mutex};

use shiguredo_libcamera::{
    CameraManager, ConfigStatus, FrameBufferAllocator, StreamRole,
};

let manager = CameraManager::new()?;
let mut camera = manager.get_camera(0)?;
camera.acquire()?;

// VideoRecording ロールで設定を生成
let mut config = camera.generate_configuration(&[StreamRole::VideoRecording])?;
if config.validate()? == ConfigStatus::Invalid {
    return Err("invalid config".into());
}
camera.configure(&mut config)?;

// 設定から Stream を取り出す
let stream = config.at(0)?.stream().expect("stream not assigned");

// バッファを割り当てる (確保数は libcamera が決定)
let allocator = FrameBufferAllocator::new(&camera);
let buffer_count = allocator.allocate(&stream)?;

// コールバック登録 (start() 前)
let frames = Arc::new(Mutex::new(Vec::<u32>::new()));
let frames_cb = Arc::clone(&frames);
let stream_cb = stream.clone();
camera.on_request_completed(move |completed| {
    if let Some(buffer) = completed.find_buffer(&stream_cb) {
        if let Ok(mut g) = frames_cb.lock() {
            g.push(buffer.metadata().sequence);
        }
    }
});

camera.start()?;

// バッファ数分のリクエストをキューに積む
let mut requests = Vec::new();
for i in 0..buffer_count {
    let buffer = allocator.get_buffer(&stream, i)?;
    let request = camera.create_request(i as u64)?;
    request.add_buffer(&stream, &buffer)?;
    requests.push(request);
}
for request in &requests {
    camera.queue_request(request)?;
}

// 完了を待つ (フレーム数で終了判定する例)
while frames.lock().unwrap().len() < 5 {
    std::thread::sleep(std::time::Duration::from_millis(100));
}

camera.stop()?;
camera.release()?;
```

### コントロールの設定 (リクエスト単位)

リクエストに紐づく `controls()` は **キューに入る前** に設定する。`on_request_completed` の `metadata()` は、libcamera 側で実際に適用された結果を含む。

```rust
use shiguredo_libcamera::core;

let request = camera.create_request(0)?;
request.add_buffer(&stream, &buffer)?;

let mut controls = request.controls();
controls.set_f32(&core::BRIGHTNESS, 0.2);
controls.set_f32(&core::CONTRAST, 1.5);
controls.set_f32(&core::SATURATION, 1.2);

camera.queue_request(&request)?;
```

### メタデータの読み取り (コールバック内)

```rust
use shiguredo_libcamera::core;

camera.on_request_completed(move |completed| {
    let meta = completed.metadata();
    // 取得は型ごとに get_xxx を使い分け、`.ok()` で Option に丸める
    let exposure_time = meta.get_i32(&core::EXPOSURE_TIME).ok();
    let analogue_gain = meta.get_f32(&core::ANALOGUE_GAIN).ok();
    let colour_temperature = meta.get_i32(&core::COLOUR_TEMPERATURE).ok();
    let sensor_timestamp = meta.get_i64(&core::SENSOR_TIMESTAMP).ok();
    // ...
});
```

### カメラプロパティの読み取り

`acquire()` 後、`configure()` 前に静的なプロパティを参照できる。

```rust
use shiguredo_libcamera::properties::core as props;

let props_list = camera.properties();
let model = props_list.get_string(&props::MODEL).ok();
let pixel_array = props_list.get_size(&props::PIXEL_ARRAY_SIZE).ok();
```

### DMA-BUF プレーンの参照

`FrameBufferPlane.fd` は libcamera 側が管理する DMA-BUF の fd で、所有権はバッファ側にある。読み出す場合は `dup` するなりして寿命を制御する。

```rust
if let Some(buffer) = completed.find_buffer(&stream) {
    for i in 0..buffer.planes_count() {
        if let Some(plane) = buffer.plane(i) {
            // plane.fd, plane.offset, plane.length を使ってマップする
        }
    }
}
```

### リクエストの再利用

`Request::reuse()` で同じインスタンスを再キューイングできる (バッファは再利用される)。実運用では `buffer_count` 個のリクエストをリングで回す。

```rust
for request in &requests {
    request.reuse();
    camera.queue_request(request)?;
}
```

## サンプル

サンプルとして動作例が同梱されている。出力は [`nojson`](https://github.com/sile/nojson) で JSON 化している (dev-dependency)。

| サンプル | 内容 |
|---------|------|
| `cargo run --example list_cameras` | カメラ一覧と Viewfinder ロールの推奨ストリーム情報を表示 |
| `cargo run --example capture` | `VideoRecording` ロールで 5 フレーム取得し、メタデータを表示 |
| `cargo run --example controls` | `BRIGHTNESS` / `CONTRAST` / `SATURATION` を設定して 3 フレーム取得し、結果のメタデータを表示 |

## 既知の制限事項・利用上の注意

- **libcamera のバージョン整合**: 動的リンク先の `libcamera` がビルド時の C++ ABI/構造体レイアウトと一致していないと未定義動作になる。`v0.7.0+rpt20260205` 系を想定している。
- **コールバックの登録順序**: `on_request_completed()` は `start()` より前に登録する必要がある。`start()` 後に登録すると初期フレームを取りこぼす可能性がある。
- **コールバックは libcamera のスレッドから呼ばれる**: `Fn + Send + Sync + 'static` を要求する。共有状態は `Arc<Mutex<...>>` 等で同期する (サンプル参照)。
- **`controls()` / `metadata()` / `properties()` の戻り値は非所有**: 元の `Request` / `Camera` が破棄されると参照は無効になる。コールバック外への持ち出しは行わない。
- **配列系 getter (`get_*_array`)**: `&[T]` を返すものは `ControlList` の借用が生きている間のみ有効。`Vec` を返すもの (`get_rectangle_array`, `get_point_array`) はコピーされる。
- **`set_*` メソッドは Result を返さない**: 値の妥当性チェックは libcamera 側で行う。不正な値はリクエスト処理時に検出される。
- **`FrameBufferRef<'a>` はコールバック内専用**: `'a` はコールバック呼び出しのスコープに縛られる。コールバック外で使いたい場合は `Request::find_buffer()` 経由で `FrameBuffer` を取得する (こちらは所有型)。
- **`bindgen` 生成の `sys` モジュールは内部用途**: 公開 API からは使わない。
