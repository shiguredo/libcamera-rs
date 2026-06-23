# Container

macOS 上で Apple Container (`container` CLI) を使い、CI と同等の clippy 検証をローカルで実行する方法です。

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
