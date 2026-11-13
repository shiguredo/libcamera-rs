# ControlList の set_* が書き込み不可のリストでも成功してしまう問題に対応する

- Created: 2026-11-13
- Completed: {YYYY-MM-DD}
- Branch: feature/change-control-list-set-result
- Polished: 2026-11-13

## 目的

本リポジトリの `ControlList::set_*` は、値を設定できたかどうかを呼び出し側に返さない。
そのため、書き込みを想定していないリストに対しても `set_*` を呼べてしまい、失敗を検知できない。

対象となるリストは次のとおり。

- `Camera::properties()` が返すリストは、カメラのプロパティである。libcamera は `const ControlList &` を返しており、
  カメラの生存期間中は一定の値として扱う設計である
- `Request::metadata()` が返すリストは、完了したリクエストのメタデータである。libcamera は `const ControlList &` を返している
- `Request::controls()` が返すリストは、リクエストに設定する値であり、libcamera は `ControlList &` を返している。
  `ControlList::set()` で更新することが想定されている

本クレートの `ControlList` は、これらのリストを区別せずに公開している。
`owned` というフィールドは `c-api/internal.h` に定義されているが、判定には使われていない。
そのため利用者は、書き込みを想定していないリストに `set_*` を呼べてしまう。

libcamera の `ControlList::set(unsigned int id, const ControlValue&)` は、書き込めなかった場合に何も返さずに戻る。
本リポジトリの `set_*` は戻り値が `()` のため、失敗を呼び出し側が一切検知できない。

カメラ制御値の設定漏れは画質や露出の誤動作に直結するため、失敗を明示的に返す必要がある。

**libcamera v0.8.0 では `ControlList::set` の挙動は変化していない。**
v0.6.0+rpt20251202 / v0.7.2+rpt20261009 / v0.8.0+rpt20261104 の `src/libcamera/controls.cpp` を確認したところ、
`ControlList::set(unsigned int id, const ControlValue&)` の実装はすべて同じであり、
v0.7.2 と v0.8.0 のファイルは内容が完全に一致している。
本 issue は v0.8.0 追従のための作業ではなく、本クレートの API 契約を追加するものである。

## 現状

`c-api/controls.cpp` の setter は 12 個あり、いずれも `ControlValue` を作ってから `list->list->set(id, cv)` を呼ぶだけで、結果を返さない。

```cpp
void lc_control_list_set_bool(lc_control_list_t* list,
                              unsigned int id,
                              bool value) {
  ControlValue cv;
  cv.set<bool>(value);
  list->list->set(id, cv);
}
```

- スカラー: `lc_control_list_set_bool` / `_byte` / `_int32` / `_int64` / `_float` / `_rectangle`
- 配列: `lc_control_list_set_byte_array` / `_int32_array` / `_int64_array` / `_float_array` / `_rectangle_array` / `_point_array`

Rust 側の `src/controls.rs` の `ControlList::set_*` も 12 個あり、すべて `()` を返している。
いずれも `&mut self` を取るため、`Camera::properties()` の戻り値を `mut` で束縛すれば呼び出せてしまう。

### libcamera の `ControlList::set` の契約

v0.8.0 の `src/libcamera/controls.cpp` の説明は次のとおり。

```text
 * This function sets the value of a control in the control list. If the control
 * is already present in the list, its value is updated, otherwise it is added
 * to the list.
 *
 * The behaviour is undefined if the control id is not supported by the
 * object that the list refers to.
```

つまり `set` は、リストに無いコントロールを**追加する**。`Request::controls()` が返すリストは空であるため、
`set` を呼ぶ時点でコントロールが存在しないのが正常な状態である。

実装は次のとおり。

```cpp
void ControlList::set(unsigned int id, const ControlValue &value)
{
	ControlValue *val = find(id);
	if (!val)
		return;

	*val = value;
}
```

書き込まれずに戻るのは、`find(id)` が `nullptr` を返した場合である。非 const 版の `find` は次のとおり。

```cpp
ControlValue *ControlList::find(unsigned int id)
{
	if (validator_ && !validator_->validate(id)) {
		LOG(Controls, Error)
			<< "Control " << utils::hex(id)
			<< " is not valid for " << validator_->name();
		return nullptr;
	}

	return &controls_[id];
}
```

`nullptr` が返るのは、`ControlValidator` がその ID を許可しない場合だけである。

したがって「リストに ID が無いこと」を事前に確認して失敗とする設計は誤りである。
それをすると `Request::controls()` への正常な追加がすべて失敗になる。

`ControlValidator` は `Request::controls()` にだけ付与されており、`Camera::properties()` と `Request::metadata()` には付与されていない。
そのため、これらのリストへの `set` は libcamera 側では失敗せず、値を書き換えてしまう。

### `owned` フラグの現状

`c-api/internal.h` の `lc_control_list` は `ControlList* list` と `bool owned` を持つが、
`owned` を読むコードは `c-api/` にも `src/controls.rs` にも存在しない。
`c-api/camera.cpp` の `lc_camera_properties`、`c-api/request.cpp` の `lc_request_controls` と
`lc_request_metadata` が `owned = false` を代入しているだけで、判定には使われていない。

`owned` という名前は「ラッパーがリストを所有しているか」を意味しているように読めるが、
実際にはどの関数もラッパー構造体を新しく確保して呼び出し側に渡しており、所有の意味は無い。
`src/controls.rs` の `ControlList` の `Drop` も `owned` の値に関係なくラッパーを解放している。

### 影響を受けるファイル

`set_*` を呼んでいる箇所は次のとおり。

- `README.md` の「コントロールの設定」
- `examples/controls.rs` の `main` 内の `set_f32` 3 箇所
- `skills/shiguredo-libcamera/SKILL.md` の `set_*` の表、コード例、既知の制限事項の記述

## 設計方針

### 変更対象

`c-api/internal.h`、`c-api/controls.h`、`c-api/controls.cpp`、`c-api/camera.cpp`、`c-api/request.cpp`、
`src/controls.rs`、`src/request.rs`、`src/error.rs`、`examples/controls.rs`、`README.md`、`CHANGES.md` を変更する。

`skills/shiguredo-libcamera/SKILL.md` の更新はドキュメント更新の issue が扱う。

### 失敗の判定方法

- C ABI の setter を `void` から `bool` に変更し、設定できた場合に `true`、できなかった場合に `false` を返す
- 書き込みを想定していないリストに対する `set_*` は、libcamera を呼ぶ前に `false` を返す
- `lc_control_list` の `owned` を `writable` に改名し、判定に使う
  - `lc_camera_properties` と `lc_request_metadata` は `writable = false` を代入する
  - `lc_request_controls` は `writable = true` を代入する
- `Camera::properties()` を `writable = false` とするのは、libcamera に「書き込み禁止」の明示的な文言があるからではなく、
  本クレートの公開 API として書き込みを許可する意味が無いという判断によるものである
- libcamera の `ControlValidator` による拒否は、現状の C ABI から観測できない。
  本 issue では対応せず、範囲外であることを明記する

### `Result` 化で解消される範囲

`ControlList::set_*` が `Err` を返すのは、`writable == false` のリストに対して呼んだ場合だけである。
`ControlValidator` による拒否は引き続き `Ok(())` となり、値は捨てられる。
この制約は `ControlList::set_*` と `Request::controls()` の doc コメントに明記し、利用者が過信しないようにする。

### Rust 側

- `ControlList::set_*` を `()` から `Result<()>` に変更する
- 失敗時は `Error::ControlSetFailed` を返す
- `src/error.rs` に `Error::ControlSetFailed` を追加し、表示文言を既存の `ControlGetFailed` と対称な
  英語の `"failed to set control value"` とする
- `ControlList::from_raw` の第 2 引数と `_owned` フィールドを `writable` に改名する
- `ControlList` の `Drop` のコメントを、`writable` の値に関係なくラッパー構造体を解放する旨に書き換える
- `ControlList` の doc コメントの「非所有 (`owned = false`)」を `writable` に合わせて書き換える
- `unsafe impl Send for ControlList` の扱いは変えない。`writable` の判定は C ABI 側で行うため、
  どのスレッドから呼んでも同じ結果になる
- `set_*` の doc コメントに、`writable` でないリストに対して呼ぶと `Err(Error::ControlSetFailed)` になることを書く
- 後方互換のない変更であるため `CHANGES.md` に `[CHANGE]` として記載する

### 空の配列の扱い

`values.len() == 0` の配列は特別扱いしない。
`writable` の判定が先に行われ、`writable = true` のリストに対しては、要素数 0 の配列値も libcamera に渡して `true` を返す。
既存の挙動を変えない。

### 呼び出し側の更新

- `examples/controls.rs` は `?` を使わず、既存の `if let Err(e) = ... { eprintln!(...); std::process::exit(1); }` の形に揃える
- 新しく追加する `eprintln!` のメッセージは英語で書く。既存の日本語メッセージの是正は本 issue の範囲外とする
- `README.md` の使用例は、失敗を無視しない形に更新する

### 他の issue との関係

他の issue は `ControlId` のフィールドや定数を扱う。本 issue は `ControlList::set_*` の戻り値と C ABI の setter を扱う。
`set_*` の引数 (`&ControlId`) は変わらないため、他の issue とは競合しない。

## 完了条件

- `c-api/controls.h` の全 setter が `bool` を返す
- `c-api/controls.cpp` の全 setter の先頭で `writable` を判定し、`false` の場合は libcamera を呼ばずに `false` を返す
- `c-api/camera.cpp` の `lc_camera_properties` と `c-api/request.cpp` の `lc_request_metadata` が `writable = false` を代入している
- `c-api/request.cpp` の `lc_request_controls` が `writable = true` を代入している
- `c-api/internal.h` に `owned` が残っていない
- `ControlList::set_*` が `Result<()>` を返し、失敗時に `Error::ControlSetFailed` となる
- `ControlList` の doc コメントと `Drop` のコメントが `writable` に合わせて更新されている
- `ControlList::set_*` の doc コメントに、`ControlValidator` による拒否を検知できない制約が書かれている
- `src/request.rs` の `Request::controls()` の doc コメントに、同じ制約が書かれている
- `examples/controls.rs` が `Result` を処理する形になっている
- `README.md` の使用例が更新されている
- `prek run --all-files` が通る (`c-api/` の `clang-format` を含む)
- `prek run cargo-clippy` が通る
- `cargo check --workspace --target aarch64-unknown-linux-gnu` が通る
- `cargo build --examples --target aarch64-unknown-linux-gnu --release` が通る
- `CHANGES.md` の `develop` セクションに `[CHANGE]` が担当者行付きで記載されている

## 解決方法

- `c-api/internal.h` の `lc_control_list` の `owned` を `writable` に改名する
- `c-api/camera.cpp` の `lc_camera_properties` と `c-api/request.cpp` の `lc_request_metadata` で `writable = false` を代入する
- `c-api/request.cpp` の `lc_request_controls` で `writable = true` を代入する
- `c-api/controls.h` の全 setter 宣言を `bool` 戻り値に変更する
- `c-api/controls.cpp` の各 setter の先頭で `if (!list->writable) { return false; }` を判定し、
  `list->list->set(id, cv);` の後に `return true;` を置く
- `src/controls.rs` の `from_raw` の引数と `_owned` フィールドを `writable` に改名し、
  `set_*` 12 個を `Result<()>` に変更する。C ABI の戻り値が `false` の場合に `Err(Error::ControlSetFailed)` を返す
- `src/error.rs` に `ControlSetFailed` と `impl fmt::Display` の対応するアームを追加する
- `examples/controls.rs` と `README.md` の呼び出しを更新する
