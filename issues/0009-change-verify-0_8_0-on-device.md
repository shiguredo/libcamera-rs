# libcamera v0.8.0 追従を実機 Raspberry Pi で検証できる手順を整備する

- Created: 2026-11-13
- Completed: {YYYY-MM-DD}
- Branch: feature/change-verify-0_8_0-on-device
- Polished: {YYYY-MM-DD}

## 目的

本リポジトリの CI は、aarch64 へのクロスビルドが成功しただけでは動作を保証できない。
`libcamera` は動的リンクで利用しており、`README.md` にも「上記バージョンと互換性のない libcamera がインストールされている環境では正常に動作しない可能性があります」
と記載されているとおり、リンク時点のヘッダーと実行時の共有ライブラリのバージョンが揃って初めて意味のある検証になる。
v0.8.0 では SONAME が 0.7 から 0.8 に上がり、`ControlValue` / `ControlList` / `FrameBuffer::Plane` の ABI が変わっている。
追従作業の正しさを確認するには、v0.8.0 が入った実機 Raspberry Pi 上で動作確認できる手順を明示的に用意する必要がある。

## 現状

`.github/workflows/ci.yml` の `verify` ジョブは self-hosted ランナー (`self-hosted`, `linux`, `ARM64`, `Raspberry-Pi`) で
`list_cameras` と `controls` を実行している。ビルドは `build` ジョブで `sysroot/raspberry-pi-os_armv8.json` を使って行われる。

現状の検証で不足している点は次のとおり。

- 実機にインストールされている libcamera のバージョンを確認していない。`sysroot` 側と実機側がずれていても CI は成功しうる
- `capture` サンプルを実機で実行していない。`FrameBuffer::Plane::fd` の `SharedFD` 化はバッファのプレーン取得経路に影響するため、
  フレーム取得まで動くことを確認する必要がある
- 実行時の共有ライブラリ解決を確認していない。v0.8.0 では `libcamera-base` のシンボルを参照するコードが増えるため、
  `libcamera-base.so.0.8` が解決できることを確かめる必要がある

`docs/CONTAINER.md` は Apple Container を使ったローカル検証手順を記載しているが、
これは aarch64 向けのクロスビルドとテストまでで、実機動作は対象外である。
またこの環境では `container system start` が `~/Library/Application Support` への書き込み制限で失敗し、
Apple Container による検証自体が起動できなかった。

## 設計方針

- CI の `verify` ジョブに、実行環境の libcamera バージョンと `libcamera-base` の解決状況を出力するステップを追加する。
  期待バージョンと異なる場合は失敗させる
- `capture` サンプルを実機で実行するステップを追加する。カメラの有無で結果が変わるため、
  カメラが検出できない場合はスキップしたうえでその旨をログに残す扱いにし、一律失敗にはしない
- ローカル (macOS) からの検証手順は `docs/CONTAINER.md` に追記する。Apple Container が使えない場合の代替手順も記載する
- 検証用の手順はコード変更を伴わない範囲に留める。c-api や Rust 側のコードを修正しない

## 完了条件

- CI の `verify` ジョブが実行環境の libcamera バージョンを検証し、期待と異なる場合に失敗する
- `capture` サンプルが CI で実行され、結果がログから判別できる
- `docs/CONTAINER.md` に v0.8.0 向けの検証手順が記載されている
- 追従作業の PR で CI が通り、実機での動作が確認できる

## 解決方法

- `.github/workflows/ci.yml` の `verify` ジョブに、`pkg-config --modversion libcamera` または `dpkg -l libcamera0.8` で
  バージョンを取得して期待値と比較するステップを追加する
- 同じジョブに `capture` を実行するステップを追加し、実行結果をログに残す
- `docs/CONTAINER.md` に、実機 Raspberry Pi での確認手順と、Apple Container が使えない環境での代替手順を追記する
- GitHub Actions の変更は `shiguredo-github-actions` スキルの規約 (action の commit hash 固定とバージョンコメント) に従う
