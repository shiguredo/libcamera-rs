# libcamera v0.8.0 追従を実機 Raspberry Pi で検証できる手順を整備する

- Created: 2026-11-13
- Completed: {YYYY-MM-DD}
- Branch: feature/update-verify-0_8_0-on-device
- Polished: 2026-11-18

## 目的

本リポジトリの CI は、aarch64 へのクロスビルドが成功しただけでは動作を保証できない。
`libcamera` は動的リンクで利用しており、`README.md` にも「上記バージョンと互換性のない libcamera がインストールされている環境では正常に動作しない可能性があります」
と記載されているとおり、リンク時点のヘッダーと実行時の共有ライブラリのバージョンが揃って初めて意味のある検証になる。

libcamera v0.8.0+rpt20261104 では SONAME が 0.7 から 0.8 に上がった
(libcamera v0.8.0 のリリース告知に「the SONAME is increased to 0.8」とある)。

`libcamera.so` は `libcamera-base.so` に依存しており、`libcamera.pc` も `Requires: libcamera-base` を持つ。
ただし `libcamera-base.so` は独立したパッケージではなく、`libcamera0.7` と `libcamera0.8` のそれぞれに同梱されている。
v0.7.2+rpt20261009-1 の deb には `libcamera-base.so.0.7` と `libcamera-base.so.0.7.2` が、
v0.8.0+rpt20261104-1 の deb には `libcamera-base.so.0.8` と `libcamera-base.so.0.8.0` が含まれる。
つまり SONAME が変わると、`libcamera-base.so` の解決先もパッケージ単位で切り替わる。

v0.8.0 のバイナリを 0.7 系の libcamera しか入っていない実機に持ち込むと、`libcamera-base.so.0.8` を解決できずに失敗する。
既存の `list_cameras` と `controls` が通っていたことは、0.7 系の `libcamera-base.so.0.7` が解決できたことしか示していない。

しかし CI は実機の libcamera バージョンを確認していない。
実機のバージョンと SONAME の解決を検証する手順を常設化する必要がある。

### 本 issue で検知できるずれの範囲

SONAME が違えば、既存の `list_cameras` でも動的リンカの時点で失敗する。
つまり「実機が 0.7 系でビルドが 0.8 系」のようなずれは、今の CI でも検知できる。

本 issue が新たに検知できるようになるのは、SONAME が 0.8 のままパッケージの版だけが違う場合である。
`sysroot/raspberry-pi-os_armv8.json` の `libcamera-dev` はバージョンを固定していないため、
ビルド側だけが同じ SONAME 0.8 の新しいパッケージを拾うと、実機のバージョンを確認するだけでは検知できない。

`sysroot` に取り込まれた deb の Debian 版は、`libcamera.pc` からは分からない。
`libcamera.pc` の `Version` は meson の版 (`0.8.0`) であり、`+rpt20261104-1` のような
Raspberry Pi のパッケージリビジョンを含まないためである。
同じ `0.8.0` のまま日付やリビジョンだけが進んだパッケージでは、この値は変わらない。

そこで本 issue では、`sysroot/raspberry-pi-os_armv8.json` の `packages` で `libcamera-dev` のバージョンを固定する。
固定した版と実機の版が違えば、どちらかが期待値から外れることになる。
`sysroot` に取り込まれる deb の版が期待値と一致することを、指定そのもので担保する。

## 現状

`.github/workflows/ci.yml` の `verify` ジョブ (`name: Verify (Raspberry Pi)`) は、self-hosted ランナー
(`group: Self`、`labels: [self-hosted, linux, ARM64, Raspberry-Pi]`) で `list_cameras` と `controls` を実行している。
ビルドは `build` ジョブが `sysroot/raspberry-pi-os_armv8.json` を使って行い、成果物を `actions/download-artifact` で受け取るだけの構成であり、
`verify` ジョブは `actions/checkout` も Rust のビルドも行っていない。

現状の検証で不足している点は次のとおり。

- 実機にインストールされている libcamera のバージョンを確認していない。`sysroot` 側と実機側がずれていても CI は成功しうる
- `libcamera-base.so.0.8` が実行時に解決できることを確認していない
- `capture` サンプルを実機で実行していない。`capture` は `FrameBufferAllocator` 経由でバッファのプレーン情報を取得するため、
  フレーム取得まで動くことを確認する経路が無い

各サンプルのカメラ未接続時の挙動は次のとおり。

- `examples/list_cameras.rs` の `main` はカメラ 0 台でも終了コード 0 で、JSON に `camera_count` を出力する
- `examples/capture.rs` の `main` と `examples/controls.rs` の `main` は、カメラ 0 台のとき「カメラが見つかりません」と出力して終了コード 1 で終わる

`docs/CONTAINER.md` は「CI と同等の clippy 検証をローカルで実行する方法」を扱う文書であり、
aarch64 向けのクロスビルドとテストまでの記載で、実機動作は対象外である。

CI ワークフローの `on.push.paths` は `c-api/**`、`src/**`、`examples/**`、`sysroot/**`、`Cargo.toml`、
`.github/workflows/ci.yml` に限られている。
そのため `docs/` だけを変更した PR では CI は起動しない。
本 issue は `.github/workflows/ci.yml` を変更するため、その PR では `build` と `verify` が実行される。

## 設計方針

### 変更対象

変更対象は次の 3 ファイルに限定する。

- `.github/workflows/ci.yml` (`verify` ジョブに検証ステップを追加する)
- `sysroot/raspberry-pi-os_armv8.json` (`libcamera-dev` のバージョンを固定する)
- `docs/CONTAINER.md` (実機での確認手順を追記する)

`c-api/`、`src/`、`examples/` は変更しない。
カメラの有無は、既存の `controls` の成否で判定する。新たに JSON を解析する処理は追加しない。

### バージョン検証

- 実機の libcamera のバージョンは `dpkg-query -W -f='${Version}' libcamera0.8` で取得する
  - 期待値は `0.8.0+rpt20261104-1` (Raspberry Pi アーカイブの `libcamera0.8` のバージョン)。パッケージリビジョンを含む完全一致で比較する。`dpkg-query` が返す Debian のバージョンは先頭に `v` を付けない (リリースタグの `v0.8.0+rpt20261104` とは表記が異なる) ため、期待値にも `v` を付けない
  - `dpkg -l` は列位置で切れるため使わない
  - `pkg-config --modversion libcamera` は `0.8.0` を返し `+rpt20261104` を含まないため、バージョンの正本には使わない
- 取得できなかった場合と、期待値と一致しなかった場合はジョブを失敗させる
- `dpkg-query` が実行できるかどうかはランナー環境に依存するため、コマンドの有無を先に確認し、無い場合は明示的なエラーで失敗させる

### `libcamera-base` の解決確認

- 実行時依存の確認は `ldd` を使う。取得したサンプルバイナリ (例: `list_cameras`) に対して `ldd` を実行し、
  出力に `libcamera-base.so.0.8` が含まれない場合は失敗させる
- 併せて `ldd` の出力に `not found` が含まれないことを確認する

### `capture` が終了しない場合の扱い

`examples/capture.rs` の `main` は、取得したフレーム数が目標に達するまで待ち続ける。
次のループには待機の期限も上限も無い。

```rust
loop {
    std::thread::sleep(std::time::Duration::from_millis(100));
    let Ok(guard) = frames.lock() else { ... };
    if guard.len() >= target_frame_count {
        break;
    }
}
```

目標のフレーム数は `CAPTURE_FRAME_COUNT (5)` と確保したバッファ数の小さい方である。
フレームが目標に達しなかった場合、`capture` は永久に待ち続ける。
`verify` ジョブの `timeout-minutes: 10` に達するとジョブは失敗するが、
その時点ではフレーム未到達なのか別の要因なのかを判別できない。

そのため、`capture` の実行はジョブのタイムアウトに任せず、ステップ側で打ち切る。

- `timeout` コマンドで上限を設けて実行し、打ち切られた場合はステップを失敗させる
- `timeout` は打ち切り時に終了コード 124 を返すため、`|| true` のような形で握りつぶさない
- 上限は `verify` ジョブの `timeout-minutes: 10` より十分短くし、打ち切りと他の要因を区別できるようにする
- `capture` の合格条件は、終了コード 0 で終わることとする

### カメラ 0 台の扱い

`verify` ジョブは既に `list_cameras` と `controls` を実行しており、`controls` はカメラ 0 台で終了コード 1 になる。
GitHub Actions は先行するステップが失敗すると後続を実行しないため、カメラ 0 台の場合は `capture` のステップに到達しない。
つまり「実機にカメラが接続されていること」は現行 CI の暗黙の前提であり、本 issue でもこの前提を変えない。

`capture` のステップに `list_cameras` の判定を置く必要は無い。
カメラの有無は `controls` の成否で既に判定されている。

### `verify` ジョブの追加ステップ

`shiguredo-github-actions` スキルの規約に従い、次の点を守る。

- 実機でしか確認できない検証であるため self-hosted ランナーを使う。runner の選定理由をワークフローのコメントに残す
- self-hosted ランナーでは `actions/cache` を使わない
- 新たに action を追加しない (`actions/*` 以外の action を使う必要が生じた場合は、実装前にユーザーへ確認する)

なお `.github/workflows/ci.yml` と `.github/workflows/release.yml` の `slack_notify` は
`shiguredo/github-actions/.github/actions/slack-notify@main` を参照している。
`shiguredo-github-actions` スキルは時雨堂のリポジトリの action を `@main` のままにすることを定めているため、
これは規約どおりであり、本 issue では変更しない。

### 前提

libcamera v0.8.0 への追従作業はすべて完了しており、`verify` ジョブは追従作業の各 PR で pass している。
実機で `list_cameras` と `controls` が動くことは、その時点で 1 回確認できている。

ただしそれは 1 回限りの確認であり、実機の libcamera が更新されたり 0.7 系に戻ったりしても CI は検知できない。
本 issue の成果は、その確認を CI に常設して回帰を防ぐことである。

### ローカル手順の追記

`docs/CONTAINER.md` の冒頭の責務 (macOS 上で Apple Container を使い clippy 検証を実行する) は変えず、
末尾に新しい章を追加して実機での確認手順を書く。

実機での確認は、コンテナを使うかどうかとは無関係な別の手順である。
Apple Container は clippy 検証をローカルで実行するためのものであり、実機でのサンプル実行や
`dpkg-query`、`ldd` の代替にはならない。

追加する章には次を書く。

- 実機上でサンプルを実行する手順
- libcamera のバージョンを確認する手順
  - ワークフローと同じコマンドを使い、`v` を付けない Debian のバージョン (`0.8.0+rpt20261104-1`) で示す
- `ldd` で `libcamera-base.so.0.8` の解決を確認する手順
- この章は Apple Container を使えない環境でも、使える環境でも同じように実機で行う手順であること

## 完了条件

- `.github/workflows/ci.yml` の `verify` ジョブに、`dpkg-query` による libcamera バージョンの検証ステップが追加され、期待値と一致しない場合に失敗する
- バージョンの検証と `ldd` による解決確認のステップが、`Run list_cameras` より前に置かれている
- `sysroot/raspberry-pi-os_armv8.json` の `packages` が `libcamera-dev:arm64=0.8.0+rpt20261104-1` となり、`Install sysroot` が成功する
- `verify` ジョブの期待値と、`sysroot/raspberry-pi-os_armv8.json` で固定したバージョンが一致している
- `verify` ジョブに、`ldd` による `libcamera-base.so.0.8` の解決確認ステップが追加され、未解決の場合に失敗する
- `verify` ジョブに `capture` の実行ステップが追加され、`timeout` で上限を設けて実行し、打ち切られた場合と異常終了した場合の両方で失敗する
- `docs/CONTAINER.md` に実機での確認手順が記載され、それが Apple Container を使う clippy 検証とは別の手順であることが分かる形になっている
- `.github/workflows/ci.yml` と `.github/workflows/release.yml` が YAML として解析できる
  (確認は対象を絞った `prek run check-yaml` で行う。`--all-files` を付けると既存の `.clang-format` が
  複数ドキュメントを含むため失敗する)
- `shiguredo-github-actions` スキルの規約 (runner 選定、`actions/cache` の不使用、action の追加なし) に違反していない

## 解決方法

### ビルド側のバージョン固定

`sysroot/raspberry-pi-os_armv8.json` の `packages` で、`libcamera-dev` のバージョンを固定する。

```json
"packages": [
    "libc6-dev",
    "libstdc++-14-dev",
    "libcamera-dev:arm64=0.8.0+rpt20261104-1"
]
```

`cargo shiguredo-sysroot` は、`packages` の要素に `:` が含まれない場合に末尾へ `:{arch}` を付けてから
`apt-get install` に渡す。`arch` は `arm64` である。
そのためアーキテクチャは末尾ではなくパッケージ名の直後に書く。書式は `libcamera-dev:arm64=0.8.0+rpt20261104-1` の順である。
`libcamera-dev=0.8.0+rpt20261104-1` と書くと `libcamera-dev=0.8.0+rpt20261104-1:arm64` に変換され、
apt が版を解釈できずに失敗する。

`libcamera.pc` の `Version` を実行時に比較する方法は採らない。
`libcamera.pc` の `Version` は meson の版 (`0.8.0`) であり、パッケージリビジョンを含まない。
同じ `0.8.0` のまま日付やリビジョンだけが進んだパッケージを `sysroot` が拾っても値が変わらないため、
パッケージの版のずれを検知できない。

指定した版がアーカイブに無ければ `cargo shiguredo-sysroot` が失敗するため、
`build` ジョブに追加のステップは要らない。

バージョンの更新が必要になったときは、`sysroot/raspberry-pi-os_armv8.json` と、
`verify` ジョブの期待値の 2 か所を更新する。

### ステップの並び順

`build` ジョブにはステップを追加しない。`Install sysroot` がバージョン固定された `libcamera-dev` を取り込むため、
版が変わればその時点で失敗する。

`verify` ジョブのステップを次の順にする。
`actions/download-artifact` で成果物を受け取った後、サンプルを実行する前にバージョンと SONAME を確認する。

| 順 | ステップ | 内容 |
|---|---|---|
| 1 | `actions/download-artifact` | 既存のまま |
| 2 | `dpkg-query` の有無の確認 | `dpkg-query` が無い場合は `exit 1` する |
| 3 | libcamera バージョンの検証 | `dpkg-query -W -f='${Version}' libcamera0.8` の結果を期待値と比較する |
| 4 | `ldd` による SONAME の解決確認 | 取得した `list_cameras` に対して `ldd` を実行し、`libcamera-base.so.0.8` の有無と `not found` の有無を確認する |
| 5 | `Run list_cameras` | 既存のまま |
| 6 | `Run controls` | 既存のまま |
| 7 | `capture` の実行 | `timeout` を付けて実行する |

**2 から 4 は既存の `Run list_cameras` より前に置く。**
GitHub Actions は先行するステップが失敗すると後続を実行しない。
`libcamera.so` は `libcamera-base.so` に依存しているため、SONAME が解決できない実機では
`list_cameras` が動的リンカの時点で落ちてしまい、バージョンと SONAME の確認まで到達しない。
原因をバージョンと SONAME で切り分けるには、サンプルの実行より前にこの 2 つを確認する必要がある。

`capture` は `Run controls` の後に置く。
カメラ 0 台の場合は `controls` が終了コード 1 になり、`capture` のステップには進まない。

### その他

- 期待値のバージョンはワークフローの `env` で 1 か所だけ定義し、各ステップから参照する
- `shiguredo-github-actions` スキルの規約に従い、runner の選定理由と、実機でしか確認できない検証であることをコメントに残す
- ログメッセージとステップの `name` は英語で書く。コメントは日本語で書く
- `docs/CONTAINER.md` の末尾に、実機での確認手順を新しい章として追記する
