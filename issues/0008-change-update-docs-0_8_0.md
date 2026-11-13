# README とコントロール一覧とスキルを libcamera v0.8.0 に更新する

- Created: 2026-11-13
- Completed: {YYYY-MM-DD}
- Branch: feature/change-update-docs-0_8_0
- Polished: {YYYY-MM-DD}

## 目的

本リポジトリは `README.md` の「対応 libcamera バージョン」で動的リンク先のバージョンを明記しており、
`docs/CONTROLS.md` で全コントロールの一覧と件数を公開している。
v0.8.0 へ追従した後もこれらが v0.7.2 のまま残ると、利用者は対応バージョンを取り違え、一覧の件数と実装が食い違う。
また `skills/shiguredo-libcamera/SKILL.md` は 2026.1.0 時点の内容で更新が止まっており、対応 libcamera を
v0.7.0+rpt20260205 と記載したままである。v0.8.0 追従を機に、これらの記述を実態に合わせる必要がある。

## 現状

v0.8.0 追従の影響を受けるドキュメントと、現在の記述は次のとおり。

- `README.md`: 「対応 libcamera バージョン」に `libcamera v0.7.2+rpt20261009` と記載されている
- `docs/CONTROLS.md`: 冒頭に `libcamera v0.7.2+rpt20261009 の全コントロール (80 件) に対応しています。` と記載され、
  Core 52 件 / Draft 14 件 / Raspberry Pi 14 件の構成になっている
- `skills/shiguredo-libcamera/SKILL.md`: バージョンが 2026.1.0、対応 libcamera が `v0.7.0+rpt20260205` と記載されている。
  末尾の注意事項にも `v0.7.0+rpt20260205 系を想定している` という記述が残っている
- `CHANGES.md`: `develop` セクションが空であり、v0.8.0 追従の記載が無い

`docs/CONTROLS.md` は v0.8.0 で定義数が変わる (core 53 / draft 14 / rpi 15 の 82 件になる見込み)。
また `src/property_ids.rs` の `ColorFilterArrangement` は v0.7.2 時点から定義済みであるが、
`docs/CONTROLS.md` にはプロパティの記載が無い。

## 設計方針

- 記載するバージョンは `libcamera v0.8.0+rpt20261104` とし、Raspberry Pi のリリースタグを正本とする
- `docs/CONTROLS.md` の件数と見出しは、実際に `src/control_ids.rs` へ反映した結果に合わせる。
  コードを先に更新し、ドキュメントはその結果を転記する
- 各コントロールの説明文は libcamera の YAML の `description` を日本語にした既存の形式を維持する。
  新規追加分についても同じ形式で記載する
- `skills/shiguredo-libcamera/SKILL.md` は、バージョン表記だけでなく、v0.8.0 で変わった API
  (`ControlValue::get<T>()` の `std::optional` 化、`ControlList::set_*` の `Result` 化) の記述も見直す
- `CHANGES.md` は `shiguredo-changelog` スキルの規約に従って記載する。issue 番号は書かない
- コードの変更は含めない。この issue はドキュメントの更新のみを扱う

## 完了条件

- `README.md` の対応 libcamera バージョンが `v0.8.0+rpt20261104` になっている
- `docs/CONTROLS.md` の件数・見出し・各コントロールの記載が `src/control_ids.rs` の実装と一致している
- `skills/shiguredo-libcamera/SKILL.md` のバージョン表記と API の説明が現状の実装と一致している
- `CHANGES.md` の `develop` セクションに追従内容が記載されている
- `prek run --all-files` の markdown 関連チェックが通る

## 解決方法

- `README.md` の「対応 libcamera バージョン」の引用ブロックを更新する
- `docs/CONTROLS.md` を `src/control_ids.rs` の定数定義と突き合わせ、件数・見出し・追加分の記載を更新する
- `skills/shiguredo-libcamera/SKILL.md` のバージョン節と API 節を更新する
- `CHANGES.md` の `develop` セクションに、対応 libcamera の更新と API 変更を区分に応じて追記する
- 全角と半角の間に半角スペースを入れる規約に従う
