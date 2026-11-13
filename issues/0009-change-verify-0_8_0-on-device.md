# libcamera v0.8.0 追従を実機 Raspberry Pi で検証できる手順を整備する

- Created: 2026-11-13
- Completed: {YYYY-MM-DD}
- Branch: feature/change-verify-0_8_0-on-device
- Polished: 2026-11-13

## 目的

本リポジトリの CI は、aarch64 へのクロスビルドが成功しただけでは動作を保証できない。
`libcamera` は動的リンクで利用しており、`README.md` にも「上記バージョンと互換性のない libcamera がインストールされている環境では正常に動作しない可能性があります」
と記載されているとおり、リンク時点のヘッダーと実行時の共有ライブラリのバージョンが揃って初めて意味のある検証になる。

libcamera v0.8.0+rpt20261104 では SONAME が 0.7 から 0.8 に上がった
(libcamera v0.8.0 のリリース告知に「the SONAME is increased to 0.8」とある)。
さらに v0.8.0 の `libcamera.pc` は `Requires: libcamera-base` を持つため、`libcamera-base.so.0.8` はアプリケーションの必須の実行時依存である。
SONAME が上がると、実機に旧バージョンの `libcamera-base` しか無い場合はライブラリ解決に失敗する。

しかし CI は実機の libcamera バージョンを確認していないため、`sysroot` 側と実機側がずれていても成功しうる。
追従作業の正しさを確認するには、実機のバージョンを検証する手順を常設化する必要がある。

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

## 設計方針

### 変更対象

変更対象は `.github/workflows/ci.yml` と `docs/CONTAINER.md` に限定する。
`c-api/`、`src/`、`examples/` は変更しない。
カメラの有無の判定は、サンプルの終了コードではなく `list_cameras` が出力する JSON の `camera_count` を使って行う。

### バージョン検証

- 実機の libcamera のバージョンは `dpkg-query -W -f='${Version}' libcamera0.8` で取得する
  - 期待値は `0.8.0+rpt20261104-1` (Raspberry Pi アーカイブの `libcamera0.8` のバージョン)。先頭の `v` とパッケージリビジョンを含む完全一致で比較する
  - `dpkg -l` は列位置で切れるため使わない
  - `pkg-config --modversion libcamera` は `0.8.0` を返し `+rpt20261104` を含まないため、バージョンの正本には使わない
- 取得できなかった場合と、期待値と一致しなかった場合はジョブを失敗させる
- `dpkg-query` が実行できるかどうかはランナー環境に依存するため、コマンドの有無を先に確認し、無い場合は明示的なエラーで失敗させる

### `libcamera-base` の解決確認

- 実行時依存の確認は `ldd` を使う。取得したサンプルバイナリ (例: `list_cameras`) に対して `ldd` を実行し、
  出力に `libcamera-base.so.0.8` が含まれない場合は失敗させる
- 併せて `ldd` の出力に `not found` が含まれないことを確認する

### カメラ未接続時の扱い

`verify` ジョブは既に `list_cameras` と `controls` を実行しており、`controls` はカメラ 0 台で終了コード 1 になる。
つまり「実機にカメラが接続されていること」は現行 CI の暗黙の前提である。
この前提を変えず、**カメラ 0 台は失敗とする**方針に揃える。

- カメラの有無は `list_cameras` の JSON の `camera_count` で判定する
- カメラ 0 台の場合は、以降の `capture` 実行ステップを実行せずに明示的に失敗させる
- カメラが 1 台以上の場合は `capture` を実行し、その終了コードをそのまま CI の成否にする

`list_cameras` の JSON は `camera_count` のほかに `camera_id` などのキーを含むため、`python3` の `json` モジュールで
`camera_count` だけを取り出す。`python3` が無い環境では代替の判定方法を使う必要があるため、実装時にランナーで確認する。

### `verify` ジョブの追加ステップ

`shiguredo-github-actions` スキルの規約に従い、次の点を守る。

- 実機でしか確認できない検証であるため self-hosted ランナーを使う。runner の選定理由をワークフローのコメントに残す
- self-hosted ランナーでは `actions/cache` を使わない
- 新たに action を追加しない (`actions/*` 以外の action を使う必要が生じた場合は、実装前にユーザーへ確認する)

なお `.github/workflows/ci.yml` と `.github/workflows/release.yml` の `slack_notify` は
`shiguredo/github-actions/.github/actions/slack-notify@main` とブランチを参照しており、commit hash 固定の規約と不整合である。
この是正は本 issue の目的外であるため、本 issue では扱わない。

### 依存関係

- 本 issue の変更は追従作業の実装を伴わないため、`0001` から `0007` の完了を待たずに実施できる
- ただし `.github/workflows/ci.yml` を変更すると `build` ジョブも同じ実行で走る。追従作業が未完了の間は
  `build` ジョブが v0.8.0 のヘッダーで失敗しうるため、本 issue の PR では `verify` ジョブの検証が実施できないことがある
- 実機での動作確認は、追従作業 (`0001` から `0007`) の PR 上で `verify` ジョブが通ることをもって確認する
- `capture` の実機でのフレーム取得は、`0005` (`FrameBuffer::Plane::fd` の `SharedFD` 化への追従) の完了条件と重なる。
  本 issue はその確認を CI で常設化するものであり、`0005` の完了条件は `0005` 側で満たす

### ローカル手順の追記

- `docs/CONTAINER.md` の冒頭の責務 (clippy 検証) は変えず、末尾に実機での確認手順を追加する
- 実機での確認手順は、実機上でサンプルを実行する手順と、libcamera のバージョンと `ldd` の結果を確認する手順を書く
- Apple Container を利用できない環境向けに、代替として実機へ直接入って確認する手順を記載する

## 完了条件

- `.github/workflows/ci.yml` の `verify` ジョブに、`dpkg-query` による libcamera バージョンの検証ステップが追加され、期待値 `0.8.0+rpt20261104-1` と一致しない場合に失敗する
- 同じジョブに、`ldd` による `libcamera-base.so.0.8` の解決確認ステップが追加され、未解決の場合に失敗する
- 同じジョブに `capture` の実行ステップが追加され、`list_cameras` の `camera_count` が 0 の場合は失敗し、1 以上の場合に `capture` の終了コードが CI の成否になる
- `docs/CONTAINER.md` に実機での確認手順と、Apple Container を利用できない環境向けの代替手順が記載されている
- 本 issue の PR で `.github/workflows/ci.yml` の YAML としての妥当性が確認できている
- `shiguredo-github-actions` スキルの規約 (runner 選定、`actions/cache` の不使用、action の追加なし) に違反していない

## 解決方法

- `.github/workflows/ci.yml` の `verify` ジョブに次のステップを追加する
  - `dpkg-query` の存在を確認し、無い場合は `exit 1` するステップ
  - `dpkg-query -W -f='${Version}' libcamera0.8` の結果を期待値 `0.8.0+rpt20261104-1` と比較するステップ
  - 取得済みのサンプルバイナリに対して `ldd` を実行し、`libcamera-base.so.0.8` の有無と `not found` の有無を確認するステップ
  - `list_cameras` を実行して JSON を保存し、`camera_count` を `python3` で読み取るステップ
  - `camera_count` が 1 以上の場合に `capture` を実行するステップ
- ログメッセージとステップの `name` は英語で書く。コメントは日本語で書く
- `docs/CONTAINER.md` の末尾に、実機での確認手順と代替手順を追記する
