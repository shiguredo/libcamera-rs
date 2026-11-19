# Container

macOS 上で Apple Container (`container` CLI) を使い、CI と同等の clippy 検証をローカルで実行する方法です。

なお実機 Raspberry Pi での動作確認はコンテナでは行えないため、末尾の「実機 Raspberry Pi での確認」に別手順としてまとめています。

## 動作確認環境

- macOS 26.5.1
- container CLI 1.0.0 (Homebrew)

## 準備

Homebrew で container をインストールし、サービスを起動します。

```bash
brew install container
brew services start container
```

## 検証用イメージのビルド

```bash
container build -t libcamera-rs-check -f Dockerfile.check .
```

## clippy の実行

```bash
container run --rm libcamera-rs-check
```

デフォルトで `cargo clippy --workspace --target aarch64-unknown-linux-gnu -- -D warnings` が実行されます。

## その他のコマンドを実行する

```bash
container run --rm libcamera-rs-check cargo check --workspace --target aarch64-unknown-linux-gnu
container run --rm libcamera-rs-check cargo build --examples --target aarch64-unknown-linux-gnu --release
container run --rm libcamera-rs-check cargo test --workspace --target aarch64-unknown-linux-gnu
```

## 注意事項

- `cargo test` はカメラデバイスを必要とするテストを除いて実行可能です
- `brew services start container` を実行しないと `container run` が失敗することがあります

## 実機 Raspberry Pi での確認

ここまでの手順は macOS 上で Apple Container を使う clippy 検証です。
この節の手順はコンテナではなく Raspberry Pi の実機で行うものであり、コンテナで代わりにはできません。
Apple Container を使えない環境でも、使える環境でも、同じように実機で実行します。

`libcamera` は動的リンクで利用しているため、ビルドに使ったヘッダーと実行時の共有ライブラリのバージョンが
揃っている必要があります。実機のバージョンと、共有ライブラリが解決できるかを確認します。

### サンプルの用意

CI と同じ成果物を使う場合は、`Build (aarch64)` で生成したサンプルを実機へコピーします。
実機でビルドする場合は、リポジトリのルートで次を実行します。

```bash
cargo build --examples --target aarch64-unknown-linux-gnu --release
```

### libcamera のバージョン確認

`dpkg-query` で `libcamera0.8` のバージョンを確認します。

```bash
dpkg-query -W -f='${Version}' libcamera0.8
```

期待値は `0.8.0+rpt20261104-1` です。
`v` は付けません。リリースタグは `v0.8.0+rpt20261104` ですが、`dpkg-query` が返す Debian のバージョンには `v` が付きません。

CI は同じ値を `sysroot/raspberry-pi-os_armv8.json` の `libcamera-dev:arm64=0.8.0+rpt20261104-1` でも固定しています。
両方が一致しないと、ビルドに使ったヘッダーと実機の共有ライブラリのバージョンがずれることになります。

### 共有ライブラリの解決確認

サンプルが `libcamera-base.so.0.8` を解決できるかを `ldd` で確認します。

```bash
ldd ./list_cameras | grep libcamera-base
```

`libcamera-base.so.0.8` が含まれ、`not found` が出なければ解決できています。
`libcamera v0.8.0` で SONAME が 0.7 から 0.8 に上がっているため、0.7 系の libcamera しか入っていない実機では解決に失敗します。

### サンプルの実行

```bash
./list_cameras
./controls
./capture
```

`controls` はカメラが 1 台も接続されていないと「カメラが見つかりません」と出力して終了コード 1 で終わります。
`capture` は取得したフレーム数が目標に達するまで終了しないため、上限を設けて実行します。

```bash
timeout 120 ./capture
```

終了コードが 124 の場合は、フレームが目標数に達せずに打ち切られています。
