# CameraManager の create / destroy 廃止に追従する

- Created: 2026-11-13
- Completed: {YYYY-MM-DD}
- Branch: feature/change-camera-manager-lifecycle
- Polished: {YYYY-MM-DD}

## 目的

libcamera v0.8.0+rpt20261104 の `include/libcamera/camera_manager.h` では、これまで存在した
`static CameraManager *create()` と `static void destroy(CameraManager *cm)` が廃止され、
コンストラクタ `CameraManager()` とデストラクタ `~CameraManager()` を直接使う形になった。
本リポジトリの `c-api/camera_manager.cpp` は `lc_camera_manager_create` / `lc_camera_manager_destroy` の中で
`std::make_unique<CameraManager>()` を呼んでいるため、この点だけでは v0.8.0 でもそのままコンパイルできるように見える。
しかし `CameraManager` の生成・破棄の扱いが変わること、およびカメラ列挙の取得経路が
`cameras()` (内部をコピーして返す) から `cameras() const` の値返しへ明確化されたことを踏まえ、
ライフサイクル管理が v0.8.0 の契約に適合しているか確認と修正が必要である。

## 現状

`c-api/camera_manager.cpp` の実装は次のとおり。

- `lc_camera_manager_create` が `new lc_camera_manager()` の後、`std::make_unique<CameraManager>()` を代入している
- `lc_camera_manager_destroy` が `delete mgr` のみを行う。`lc_camera_manager` は `std::unique_ptr<CameraManager>` を保持しているため、
  unique_ptr のデストラクタで `CameraManager` のデストラクタが呼ばれる
- `lc_camera_manager_cameras_count` が `mgr->cm->cameras().size()` を返す。呼び出しごとに `std::vector` がコピーされる
- `lc_camera_manager_get_camera` が `cameras()` を呼んでからインデックスでアクセスしている
- `lc_camera_manager_get_camera_by_id` が `mgr->cm->get(id)` を呼んでいる

`src/camera_manager.rs` の `CameraManager::new` は C ABI の `lc_camera_manager_create` と
`lc_camera_manager_start` を呼び、`Drop` で `lc_camera_manager_stop` と `lc_camera_manager_destroy` を呼ぶ。

v0.8.0 の `camera_manager.h` の該当部分は次のとおり。

```cpp
 CameraManager();
 ~CameraManager();

 int start();
 void stop();

 std::vector<std::shared_ptr<Camera>> cameras() const;
 std::shared_ptr<Camera> get(std::string_view id);
```

## 設計方針

- `CameraManager` の生成と破棄は、v0.8.0 のコンストラクタとデストラクタにそのまま委ねる。
  `lc_camera_manager` が `std::unique_ptr<CameraManager>` を持つ現状の構造は維持する
- `get(std::string_view id)` に変わったことへ追従する。`const char*` から `std::string_view` への暗黙変換で足りるが、
  `nullptr` が渡された場合に未定義動作になるため、`lc_camera_manager_get_camera_by_id` で `id == nullptr` を明示的に弾く
- `cameras()` が毎回 `std::vector` を返すため、`lc_camera_manager_get_camera` のようにインデックス指定で都度呼ぶ実装は、
  カメラ数の多い環境で不要なコピーを繰り返す。v0.8.0 追従の範囲内で、取得処理を 1 回の `cameras()` 呼び出しに留める形へ寄せる
- Rust 側の公開 API (`CameraManager::new` / `cameras_count` / `get_camera` / `get_camera_by_id`) のシグネチャは変えない
- C ABI の関数名とシグネチャは変えない。`create` / `destroy` という名前は本リポジトリの C ABI の名前であり、
  libcamera の廃止された static メソッドとは別物であるため改名しない

## 完了条件

- v0.8.0 のヘッダーに対して `c-api/camera_manager.cpp` がコンパイルできる
- `lc_camera_manager_get_camera_by_id` に `nullptr` を渡した場合に、クラッシュせず `nullptr` を返す
- `lc_camera_manager_get_camera` が `cameras()` を複数回呼ばない
- 実機 Raspberry Pi 上で `list_cameras` サンプルが従来どおり動作する
- `cargo clippy --workspace --all-targets --target aarch64-unknown-linux-gnu -- -D warnings` が通る

## 解決方法

- `c-api/camera_manager.cpp` の `lc_camera_manager_create` は `std::make_unique<CameraManager>()` のままとし、
  v0.8.0 で廃止された `CameraManager::create()` / `destroy()` を新たに使わないよう確認する
- `lc_camera_manager_get_camera_by_id` の先頭で `if (id == nullptr) { return nullptr; }` を追加する
- `lc_camera_manager_get_camera` は `const auto cameras = mgr->cm->cameras();` の結果を 1 回だけ取得してインデックスを検証する形に整える
- コメントは日本語、ログメッセージは英語で書く
