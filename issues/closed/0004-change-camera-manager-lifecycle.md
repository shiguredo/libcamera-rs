# CameraManager のライフサイクルが libcamera v0.8.0 の契約に適合しているか確認する

- Created: 2026-11-13
- Completed: 2026-11-13
- Branch: feature/change-camera-manager-lifecycle
- Polished: {YYYY-MM-DD}

## 目的

libcamera v0.8.0+rpt20261104 への追従にあたり、本リポジトリの `CameraManager` の扱いが
v0.8.0 の `include/libcamera/camera_manager.h` の契約に適合しているかを確認する。

**libcamera v0.8.0 では `CameraManager` の API は変化していない。**
v0.6.0+rpt20251202 / v0.7.0+rpt20260205 / v0.7.2+rpt20261009 / v0.8.0+rpt20261104 の `camera_manager.h` は
943 バイトで内容が完全に一致しており、`cameras()` のシグネチャも `get()` のシグネチャも変わっていない。

したがって本 issue は v0.8.0 追従のための変更作業ではなく、現行実装が v0.8.0 の契約に適合しているかを確認し、
その結果を記録するものである。

## 現状

`c-api/camera_manager.cpp` の実装は次のとおり。

- `lc_camera_manager_create` が `new lc_camera_manager()` の後、`std::make_unique<CameraManager>()` を代入している。
  これは `CameraManager` のコンストラクタを直接呼ぶ形であり、v0.8.0 の契約に適合している
- `lc_camera_manager_destroy` が `delete mgr` のみを行う。`lc_camera_manager` は `std::unique_ptr<CameraManager>` を保持しているため、
  unique_ptr のデストラクタで `CameraManager` のデストラクタが呼ばれる
- `lc_camera_manager_cameras_count` が `mgr->cm->cameras().size()` を返す
- `lc_camera_manager_get_camera` が `auto cameras = mgr->cm->cameras();` で 1 回取得し、インデックスを検証している
- `lc_camera_manager_get_camera_by_id` が `mgr->cm->get(id)` を呼んでいる

`src/camera_manager.rs` の `CameraManager::new` は C ABI の `lc_camera_manager_create` と
`lc_camera_manager_start` を呼び、`start` が失敗した場合は `lc_camera_manager_destroy` を呼んでからエラーを返す。
`Drop` で `lc_camera_manager_stop` と `lc_camera_manager_destroy` を呼ぶ。ライフサイクルの解放漏れはない。

v0.8.0 の `camera_manager.h` の該当部分は次のとおり。

```cpp
 CameraManager();
 ~CameraManager();

 int start();
 void stop();

 std::vector<std::shared_ptr<Camera>> cameras() const;
 std::shared_ptr<Camera> get(std::string_view id);
```

`camera_manager.h` が v0.6.0 以降変化していないため、`c-api/camera_manager.cpp` は v0.8.0 でもそのままコンパイルできる。
`git log -- c-api/camera_manager.cpp` の結果は初回インポートの 1 件のみであり、v0.7.2 追従のコミットでもこのファイルは変更されていない。

## 設計方針

### 変更対象

本 issue ではコードを変更しない。
確認の結果をこの issue に記録し、closed にする。

`c-api/`、`src/`、`examples/` はいずれも変更しない。
`CHANGES.md` への追記は行わない。公開 API のシグネチャと挙動が変わらないためである。

### 確認した内容

- `lc_camera_manager_create` / `lc_camera_manager_destroy` が v0.8.0 のコンストラクタとデストラクタに委ねていること
- `src/camera_manager.rs` の `CameraManager::new` が `start` 失敗時に確実に解放していること
- カメラ列挙の取得経路が v0.8.0 の `cameras()` の契約と整合すること

### 実装上の改善候補 (本 issue の範囲外)

レビューで次の 2 点が見つかったが、いずれも v0.8.0 追従を根拠とする作業ではなく、本 issue の目的に含めない。

- `lc_camera_manager_get_camera_by_id` は `id == nullptr` を検証していない。
  `std::string_view` に `nullptr` を渡すと `std::char_traits::length` が未定義動作になり、実際にクラッシュする。
  ただし Rust 側の `get_camera_by_id` は `CString::as_ptr()` を渡すため到達できず、影響するのは C ABI を直接使う利用者だけである。
  `get()` 自体が `std::string_view` を受け取るのは v0.5.0+rpt20250429 からの変化であり、v0.8.0 とは無関係である
- `lc_camera_manager_cameras_count` は呼び出しごとに `std::vector` をコピーする。
  `lc_camera_manager_get_camera` は既に `cameras()` を 1 回しか呼んでいない。
  削減するなら C ABI の一括取得 API の追加が必要であり、性能改善として別に判断すべき内容である

これらが必要になった時点で、それぞれ別の issue として起票する。

## 完了条件

- `camera_manager.h` が v0.6.0 から v0.8.0 まで変化していないことを確認している
- `c-api/camera_manager.cpp` が v0.8.0 のヘッダーでコンパイルできることを確認している
- `src/camera_manager.rs` の `CameraManager::new` と `Drop` に解放漏れが無いことを確認している

## 解決方法

コード変更を行わずに closed にする。理由は次のとおり。

- 起票時の目的は「libcamera v0.8.0 で `static CameraManager *create()` と `static void destroy(CameraManager *cm)` が廃止されたことへの追従」であったが、これは事実誤認であった
- `create()` / `destroy()` は v0.3.2+rpt20241119 / v0.4.0+rpt20250213 / v0.5.2+rpt20250903 / v0.6.0+rpt20251202 / v0.7.2+rpt20261009 / v0.8.0+rpt20261104 のいずれの `camera_manager.h` にも存在しない。廃止されたのではなく、存在したことがない
- `get()` が `std::string_view` を受け取るようになったのは v0.5.0+rpt20250429 であり、本リポジトリは v0.7.2 追従の 2026.2.0 で既にこの変化を取り込んでいる
- `cameras()` のシグネチャは全バージョンで `std::vector<std::shared_ptr<Camera>> cameras() const;` のままであり、v0.8.0 で変化していない
- 現行の `c-api/camera_manager.cpp` は `std::make_unique<CameraManager>()` でコンストラクタを直接呼び、unique_ptr のデストラクタで破棄しており、v0.8.0 の契約に適合している。`src/camera_manager.rs` の `CameraManager::new` は `start` 失敗時に明示的に解放しており、`Drop` での解放も揃っている
- `lc_camera_manager_get_camera` は既に `cameras()` を 1 回しか呼んでおらず、完了条件として挙げていた改善は現状で成立している
- したがって v0.8.0 追従として本 issue で対応すべき作業は存在しない
- `lc_camera_manager_get_camera_by_id` の `id == nullptr` 検証と `lc_camera_manager_cameras_count` のコピー削減は、v0.8.0 追従ではなく別目的の改善である。`id == nullptr` によるクラッシュは実際に発生するため、不具合として別 issue を起票した。`cameras_count` のコピー削減は性能改善であり、必要性が確認されていないため起票しない
