# 対応コントロール一覧

libcamera v0.7.0+rpt20260205 の全コントロール (80 件) に対応しています。

## Core (52 件)

### AeEnable (ID: 1)

- 定数名: `core::AE_ENABLE`
- 型: Bool
- 方向: In

AEGC アルゴリズムの有効/無効を制御する。true に設定すると ExposureTimeMode と AnalogueGainMode が両方 Auto に、false に設定すると両方 Manual に設定される。同じリクエストで ExposureTimeMode や AnalogueGainMode も設定された場合はそちらが優先される。

### AeState (ID: 2)

- 定数名: `core::AE_STATE`
- 型: Int32
- 方向: Out

AEGC アルゴリズムの状態を報告する。露出時間とアナログゲインの両方が Manual の場合は Idle、少なくとも一方が Auto の場合は収束状態に応じて Searching または Converged を報告する。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `ae_state::IDLE` | AEGC が非アクティブ |
| 1 | `ae_state::SEARCHING` | 収束中 |
| 2 | `ae_state::CONVERGED` | 収束完了 |

### AeMeteringMode (ID: 3)

- 定数名: `core::AE_METERING_MODE`
- 型: Int32
- 方向: InOut

AE アルゴリズムの測光モードを指定する。画像のどの部分を使ってシーンの明るさを判定するかを決定する。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `ae_metering_mode::CENTRE_WEIGHTED` | 中央重点測光 |
| 1 | `ae_metering_mode::SPOT` | スポット測光 |
| 2 | `ae_metering_mode::MATRIX` | マトリクス測光 |
| 3 | `ae_metering_mode::CUSTOM` | カスタム測光 |

### AeConstraintMode (ID: 4)

- 定数名: `core::AE_CONSTRAINT_MODE`
- 型: Int32
- 方向: InOut

AE アルゴリズムの制約モードを指定する。測定されたシーンの明るさを目標露出に調整する方法を決定する。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `ae_constraint_mode::NORMAL` | デフォルト (バランス重視) |
| 1 | `ae_constraint_mode::HIGHLIGHT` | ハイライト優先 (白飛び防止) |
| 2 | `ae_constraint_mode::SHADOWS` | シャドウ優先 (黒つぶれ防止) |
| 3 | `ae_constraint_mode::CUSTOM` | カスタム |

### AeExposureMode (ID: 5)

- 定数名: `core::AE_EXPOSURE_MODE`
- 型: Int32
- 方向: InOut

AE の露出モードを指定する。目標露出を露出時間とアナログゲインにどう配分するかを決定する。AnalogueGainMode または ExposureTimeMode が Manual の場合、固定値が優先される。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `ae_exposure_mode::NORMAL` | デフォルト |
| 1 | `ae_exposure_mode::SHORT` | 短時間露出のみ |
| 2 | `ae_exposure_mode::LONG` | 長時間露出を許可 |
| 3 | `ae_exposure_mode::CUSTOM` | カスタム |

### ExposureValue (ID: 6)

- 定数名: `core::EXPOSURE_VALUE`
- 型: Float
- 方向: InOut

露出補正値 (EV) を指定する。AE アルゴリズムが有効な場合のみ適用される。EV は log2 スケールで、例えば EV=1 で 2 倍、EV=-1 で 1/2 倍の露出となる。

### ExposureTime (ID: 7)

- 定数名: `core::EXPOSURE_TIME`
- 型: Int32
- 方向: InOut

センサーの露出時間をマイクロ秒単位で指定する。ExposureTimeMode が Manual の場合のみ有効。Auto モードでは無視される。メタデータでは現在のフレームの実際の露出時間が報告される。

### ExposureTimeMode (ID: 8)

- 定数名: `core::EXPOSURE_TIME_MODE`
- 型: Int32
- 方向: InOut

露出時間の制御ソースを指定する。Auto ではAE アルゴリズムが計算し、Manual ではアプリケーションが ExposureTime で直接指定する。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `exposure_time_mode::AUTO` | AE アルゴリズムが自動計算 |
| 1 | `exposure_time_mode::MANUAL` | アプリケーションが手動指定 |

### AnalogueGain (ID: 9)

- 定数名: `core::ANALOGUE_GAIN`
- 型: Float
- 方向: InOut

センサーのアナログゲイン倍率を指定する。全カラーチャンネルに適用され、1.0 未満にはできない。AnalogueGainMode が Manual の場合のみ有効。

### AnalogueGainMode (ID: 10)

- 定数名: `core::ANALOGUE_GAIN_MODE`
- 型: Int32
- 方向: InOut

アナログゲインの制御ソースを指定する。Auto では AEGC アルゴリズムが計算し、Manual ではアプリケーションが AnalogueGain で直接指定する。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `analogue_gain_mode::AUTO` | AEGC が自動計算 |
| 1 | `analogue_gain_mode::MANUAL` | アプリケーションが手動指定 |

### AeFlickerMode (ID: 11)

- 定数名: `core::AE_FLICKER_MODE`
- 型: Int32
- 方向: InOut

AGC/AEC のフリッカー回避モードを設定する。人工照明のデューティサイクルによるフリッカーを抑制する。多くの実装では露出時間を周期の整数倍に制限することで回避する。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `ae_flicker_mode::OFF` | フリッカー回避なし |
| 1 | `ae_flicker_mode::MANUAL` | 手動設定 (AeFlickerPeriod で周期を指定) |
| 2 | `ae_flicker_mode::AUTO` | 自動検出・回避 |

### AeFlickerPeriod (ID: 12)

- 定数名: `core::AE_FLICKER_PERIOD`
- 型: Int32
- 方向: InOut

手動フリッカー周期をマイクロ秒単位で指定する。AeFlickerMode が Manual の場合に使用される。50Hz 電源の場合は 10000 (100Hz)、60Hz 電源の場合は 8333 (120Hz) を設定する。

### AeFlickerDetected (ID: 13)

- 定数名: `core::AE_FLICKER_DETECTED`
- 型: Int32
- 方向: Out

検出されたフリッカー周期をマイクロ秒単位で報告する。フリッカーが検出されない場合は 0。50Hz 電源の場合は 10000、60Hz 電源の場合は 8333 が報告される。

### Brightness (ID: 14)

- 定数名: `core::BRIGHTNESS`
- 型: Float
- 方向: InOut

明るさパラメータを指定する。正の値 (最大 1.0) で明るく、負の値 (最小 -1.0) で暗くなる。0.0 は変更なし。

### Contrast (ID: 15)

- 定数名: `core::CONTRAST`
- 型: Float
- 方向: InOut

コントラストパラメータを指定する。1.0 が標準。大きい値でコントラストが強くなる。

### Lux (ID: 16)

- 定数名: `core::LUX`
- 型: Float
- 方向: Out

現在の照度の推定値をルクス単位で報告する。メタデータでのみ返される。

### AwbEnable (ID: 17)

- 定数名: `core::AWB_ENABLE`
- 型: Bool
- 方向: InOut

AWB (自動ホワイトバランス) の有効/無効を制御する。有効時はアルゴリズムが色温度を推定し、カラーゲインと色補正マトリクスを自動計算する。無効時はこれらの値を手動で設定できる。

### AwbMode (ID: 18)

- 定数名: `core::AWB_MODE`
- 型: Int32
- 方向: InOut

AWB アルゴリズムが使用する光源の範囲を指定する。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `awb_mode::AUTO` | 全色温度範囲で検索 |
| 1 | `awb_mode::INCANDESCENT` | 白熱灯 |
| 2 | `awb_mode::TUNGSTEN` | タングステン灯 |
| 3 | `awb_mode::FLUORESCENT` | 蛍光灯 |
| 4 | `awb_mode::INDOOR` | 屋内 |
| 5 | `awb_mode::DAYLIGHT` | 日光 |
| 6 | `awb_mode::CLOUDY` | 曇天 |
| 7 | `awb_mode::CUSTOM` | カスタム |

### AwbLocked (ID: 19)

- 定数名: `core::AWB_LOCKED`
- 型: Bool
- 方向: Out

AWB アルゴリズムのロック状態を報告する。ロック中は true、収束中は false。AWB が動作していない場合はメタデータに含まれない。

### ColourGains (ID: 20)

- 定数名: `core::COLOUR_GAINS`
- 型: Float (配列, サイズ 2)
- 方向: InOut

R チャンネルと B チャンネルのカラーゲインのペア。AWB 無効時のみリクエストで設定可能。ColourGains を設定して ColourTemperature を設定しない場合、実装が ColourTemperature を自動計算する。

### ColourTemperature (ID: 21)

- 定数名: `core::COLOUR_TEMPERATURE`
- 型: Int32
- 方向: Out

フレームの色温度をケルビン単位で報告する。AWB 無効時にリクエストで設定した場合、実装が対応する ColourGains を自動計算する。

### Saturation (ID: 22)

- 定数名: `core::SATURATION`
- 型: Float
- 方向: InOut

彩度パラメータを指定する。1.0 が標準。大きい値で彩度が上がり、0.0 でグレースケールになる。

### SensorBlackLevels (ID: 23)

- 定数名: `core::SENSOR_BLACK_LEVELS`
- 型: Int32 (配列, サイズ 4)
- 方向: Out

フレーム処理に使用されたセンサーの黒レベルを R, Gr, Gb, B の順で報告する。値は 16 ビットピクセル範囲 (0-65535) で表される。メタデータでのみ返される。

### Sharpness (ID: 24)

- 定数名: `core::SHARPNESS`
- 型: Float
- 方向: InOut

シャープニングの強度を指定する。0.0 はシャープニングなし。デフォルト値は一般的なユースケースに適した値。RAW ストリームには適用されない。

### FocusFoM (ID: 25)

- 定数名: `core::FOCUS_FOM`
- 型: Int32
- 方向: Out

フォーカスの合焦度を示す Figure of Merit (FoM) を報告する。大きい値ほど合焦状態が良い。複数のフォーカス領域の統計を組み合わせた値であり、その方法はプラットフォームに依存する。

### ColourCorrectionMatrix (ID: 26)

- 定数名: `core::COLOUR_CORRECTION_MATRIX`
- 型: Float (配列, サイズ 3x3)
- 方向: InOut

カメラ RGB から sRGB への 3x3 色補正マトリクス。ホワイトバランス後、ガンマ変換前に適用される。9 個の浮動小数点値の配列として格納される。AWB 無効時のみリクエストで設定可能。

### ScalerCrop (ID: 27)

- 定数名: `core::SCALER_CROP`
- 型: Rectangle
- 方向: InOut

最終出力画像にスケーリングされる画像領域を設定する。座標は PixelArrayActiveAreas に対する相対位置で、単位はセンサーのネイティブピクセル。デジタルズームの実装に使用できる。

### DigitalGain (ID: 28)

- 定数名: `core::DIGITAL_GAIN`
- 型: Float
- 方向: InOut

処理パイプラインで適用されるデジタルゲイン値。全カラーチャンネルに適用されるグローバルデジタルゲイン係数。パイプラインが複数の処理ステップでデジタルゲインを適用する場合、この値はそれらの合計を示す。

### FrameDuration (ID: 29)

- 定数名: `core::FRAME_DURATION`
- 型: Int64
- 方向: Out

フレーム露出開始から次のフレーム露出開始までの瞬時フレーム間隔をマイクロ秒単位で報告する。メタデータでのみ返される。

### FrameDurationLimits (ID: 30)

- 定数名: `core::FRAME_DURATION_LIMITS`
- 型: Int64 (配列, サイズ 2)
- 方向: InOut

フレーム間隔の最小値と最大値をマイクロ秒単位で指定する。最大フレーム間隔は AE が使用できる最大露出時間を制限する。最小値と最大値を同じにすると固定フレームレートになる。両方 0 にするとデフォルトに戻る。

### SensorTemperature (ID: 31)

- 定数名: `core::SENSOR_TEMPERATURE`
- 型: Float
- 方向: Out

カメラセンサーの温度を摂氏で報告する。ダイ上またはカメラモジュール内のサーマルセンサーから取得される。サーマルセンサーが存在する場合のみメタデータに含まれる。

### SensorTimestamp (ID: 32)

- 定数名: `core::SENSOR_TIMESTAMP`
- 型: Int64
- 方向: Out

センサーのアクティブアレイの最初の行が露出された時刻。ナノ秒単位で、Linux の CLOCK_BOOTTIME に基づく単調増加カウンタ。メタデータでのみ返される。

### AfMode (ID: 33)

- 定数名: `core::AF_MODE`
- 型: Int32
- 方向: InOut

AF (オートフォーカス) アルゴリズムのモードを指定する。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `af_mode::MANUAL` | 手動モード。LensPosition で直接制御。AfState は常に Idle |
| 1 | `af_mode::AUTO` | 自動モード。AfTrigger でスキャンを開始。スキャン結果を AfState で報告 |
| 2 | `af_mode::CONTINUOUS` | 連続モード。自動的にスキャンを開始・繰り返す。AfPause で一時停止可能 |

### AfRange (ID: 34)

- 定数名: `core::AF_RANGE`
- 型: Int32
- 方向: InOut

AF がスキャンするフォーカス距離の範囲を指定する。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `af_range::NORMAL` | 広範囲 (無限遠から近距離まで、最接写は含まない場合あり) |
| 1 | `af_range::MACRO` | 近距離のみ |
| 2 | `af_range::FULL` | 全範囲 (Normal + 最接写を含む) |

### AfSpeed (ID: 35)

- 定数名: `core::AF_SPEED`
- 型: Int32
- 方向: InOut

AF レンズの移動速度を指定する。動画撮影中は穏やかに、プレビュー時は高速に移動させたい場合に使用する。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `af_speed::NORMAL` | 通常速度 |
| 1 | `af_speed::FAST` | 高速 |

### AfMetering (ID: 36)

- 定数名: `core::AF_METERING`
- 型: Int32
- 方向: InOut

AF アルゴリズムがフォーカスを測定する画像領域を指定する。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `af_metering::AUTO` | アルゴリズムが自動決定 |
| 1 | `af_metering::WINDOWS` | AfWindows で指定した矩形領域を使用 |

### AfWindows (ID: 37)

- 定数名: `core::AF_WINDOWS`
- 型: Rectangle (配列)
- 方向: InOut

AfMetering が Windows に設定されている場合に AF アルゴリズムが使用するフォーカスウィンドウ。単位は ScalerCropMaximum プロパティ内のピクセル座標。幅と高さが 0 でないウィンドウのみ有効。ScalerCrop を変更しても AF ウィンドウは自動再計算されない。

### AfTrigger (ID: 38)

- 定数名: `core::AF_TRIGGER`
- 型: Int32
- 方向: In

AF スキャンを開始またはキャンセルする。AfMode が Auto の場合のみ有効。Manual または Continuous モードでは無視される。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `af_trigger::START` | スキャン開始 (実行中の場合は無視) |
| 1 | `af_trigger::CANCEL` | スキャンキャンセル (レンズは移動しない) |

### AfPause (ID: 39)

- 定数名: `core::AF_PAUSE`
- 型: Int32
- 方向: In

Continuous AF モードでのレンズ移動を一時停止/再開する。Continuous モード以外では無効。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `af_pause::IMMEDIATE` | 即座に一時停止 (スキャン中でも停止) |
| 1 | `af_pause::DEFERRED` | 現在のスキャン完了後に一時停止 |
| 2 | `af_pause::RESUME` | 再開 |

### LensPosition (ID: 40)

- 定数名: `core::LENS_POSITION`
- 型: Float
- 方向: InOut

フォーカスレンズの位置を設定・報告する。値はディオプター (焦点距離の逆数, 単位: 1/m)。0 で無限遠、0.5 で 2m、2 で 50cm にフォーカスする。AfMode が Manual の場合のみ設定が有効だが、全モードでメタデータに報告される。

### AfState (ID: 41)

- 定数名: `core::AF_STATE`
- 型: Int32
- 方向: Out

AF アルゴリズムの現在の状態を報告する。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `af_state::IDLE` | Manual モードまたは Auto モードでトリガー前/キャンセル後 |
| 1 | `af_state::SCANNING` | スキャン中 |
| 2 | `af_state::FOCUSED` | スキャン完了、合焦 |
| 3 | `af_state::FAILED` | スキャン完了、合焦失敗 |

### AfPauseState (ID: 42)

- 定数名: `core::AF_PAUSE_STATE`
- 型: Int32
- 方向: Out

Continuous AF の一時停止状態を報告する。Continuous モードでのみ有効。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `af_pause_state::RUNNING` | 動作中 (自動的にスキャンを再開する可能性あり) |
| 1 | `af_pause_state::PAUSING` | 一時停止待ち (スキャン完了後に停止) |
| 2 | `af_pause_state::PAUSED` | 一時停止中 |

### HdrMode (ID: 43)

- 定数名: `core::HDR_MODE`
- 型: Int32
- 方向: InOut

HDR (ハイダイナミックレンジ) イメージングのモードを設定する。複数露出、画像合成、トーンマッピングの技術を使用してダイナミックレンジを向上させる。各フレームには HdrChannel で対応するチャンネル情報がタグ付けされる。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `hdr_mode::OFF` | HDR 無効 |
| 1 | `hdr_mode::MULTI_EXPOSURE_UNMERGED` | 複数露出 (未合成、アプリケーションが合成) |
| 2 | `hdr_mode::MULTI_EXPOSURE` | 複数露出 (合成済み) |
| 3 | `hdr_mode::SINGLE_EXPOSURE` | 単一露出から HDR 生成 |
| 4 | `hdr_mode::NIGHT` | ナイトモード |

### HdrChannel (ID: 44)

- 定数名: `core::HDR_CHANNEL`
- 型: Int32
- 方向: Out

フレームのキャプチャに使用された HDR チャンネルを報告する。HDR モードが有効な場合のみメタデータに含まれる。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `hdr_channel::NONE` | HDR キャプチャに対応しない |
| 1 | `hdr_channel::SHORT` | 短時間露出 |
| 2 | `hdr_channel::MEDIUM` | 中間露出 |
| 3 | `hdr_channel::LONG` | 長時間露出 |

### Gamma (ID: 45)

- 定数名: `core::GAMMA`
- 型: Float
- 方向: InOut

固定ガンマ値を指定する。デフォルトは 2.2 (sRGB に近似)。カメラガンマとして 1.0/gamma が適用される。

### DebugMetadataEnable (ID: 46)

- 定数名: `core::DEBUG_METADATA_ENABLE`
- 型: Bool
- 方向: InOut

デバッグメタデータの出力を有効/無効にする。

### FrameWallClock (ID: 47)

- 定数名: `core::FRAME_WALL_CLOCK`
- 型: Int64
- 方向: Out

SensorTimestamp と同じ瞬間に対応する壁時計タイムスタンプ。CLOCK_REALTIME で計測され、ナノ秒単位。異なるデバイス間のタイミング同期に使用できる。メタデータでのみ返される。

### WdrMode (ID: 48)

- 定数名: `core::WDR_MODE`
- 型: Int32
- 方向: InOut

WDR (ワイドダイナミックレンジ) のグローバルトーンマッピングアルゴリズムを選択する。センサーの露出時間を自動的に短縮して飽和ピクセルを減らし、トーンマッピングカーブで明るさを補償する。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `wdr_mode::OFF` | WDR 無効 |
| 1 | `wdr_mode::LINEAR` | 線形トーンマッピング (2 つの線形セクション) |
| 2 | `wdr_mode::POWER` | べき乗トーンマッピング (暗部高ゲイン、ノイズが目立つ) |
| 3 | `wdr_mode::EXPONENTIAL` | 指数トーンマッピング (自然な見た目、多くのシーンに最適) |
| 4 | `wdr_mode::HISTOGRAM_EQUALIZATION` | ヒストグラム均等化 (技術分析向け) |

### WdrStrength (ID: 49)

- 定数名: `core::WDR_STRENGTH`
- 型: Float
- 方向: In

WDR アルゴリズムの強度を指定する。0 はトーンマッピングなし、1 がデフォルト。1 より大きい値はトーンマッピング効果を強め、非現実的な画像になる場合がある。

### WdrMaxBrightPixels (ID: 50)

- 定数名: `core::WDR_MAX_BRIGHT_PIXELS`
- 型: Float
- 方向: In

許容される飽和近似ピクセルの割合 (%)。WDR アルゴリズムは飽和に近いピクセルの量がこの値より低くなるまで露出を調整する。

### LensDewarpEnable (ID: 51)

- 定数名: `core::LENS_DEWARP_ENABLE`
- 型: Bool
- 方向: InOut

レンズ歪み補正の有効/無効を制御する。

### LensShadingCorrectionEnable (ID: 52)

- 定数名: `core::LENS_SHADING_CORRECTION_ENABLE`
- 型: Bool
- 方向: InOut

レンズシェーディング補正の有効/無効を制御する。

## Draft (14 件)

### AePrecaptureTrigger (ID: 10001)

- 定数名: `draft::AE_PRECAPTURE_TRIGGER`
- 型: Int32
- 方向: InOut

プリキャプチャ AE 測光シーケンスのトリガーを制御する。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `ae_precapture_trigger::IDLE` | トリガーなし |
| 1 | `ae_precapture_trigger::START` | プリキャプチャ測光を開始 |
| 2 | `ae_precapture_trigger::CANCEL` | 測光シーケンスをキャンセル、AE をリセット |

### NoiseReductionMode (ID: 10002)

- 定数名: `draft::NOISE_REDUCTION_MODE`
- 型: Int32
- 方向: InOut

ノイズリダクションアルゴリズムのモードを選択する。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `noise_reduction_mode::OFF` | ノイズリダクションなし |
| 1 | `noise_reduction_mode::FAST` | フレームレートを低下させずに適用 |
| 2 | `noise_reduction_mode::HIGH_QUALITY` | 高品質 (フレームレートが低下する場合あり) |
| 3 | `noise_reduction_mode::MINIMAL` | 最小限 (フレームレートを低下させない) |
| 4 | `noise_reduction_mode::ZSL` | ストリームごとに異なるレベルで適用 |

### ColorCorrectionAberrationMode (ID: 10003)

- 定数名: `draft::COLOR_CORRECTION_ABERRATION_MODE`
- 型: Int32
- 方向: InOut

色収差補正アルゴリズムのモードを選択する。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `color_correction_aberration_mode::OFF` | 補正なし |
| 1 | `color_correction_aberration_mode::FAST` | フレームレートを低下させずに補正 |
| 2 | `color_correction_aberration_mode::HIGH_QUALITY` | 高品質補正 (フレームレートが低下する場合あり) |

### AwbState (ID: 10004)

- 定数名: `draft::AWB_STATE`
- 型: Int32
- 方向: Out

AWB アルゴリズムの現在の状態を報告する。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `awb_state::INACTIVE` | AWB が非アクティブ |
| 1 | `awb_state::SEARCHING` | 収束中 |
| 2 | `awb_state::CONVERGED` | 収束完了 |
| 3 | `awb_state::LOCKED` | ロック中 |

### SensorRollingShutterSkew (ID: 10005)

- 定数名: `draft::SENSOR_ROLLING_SHUTTER_SKEW`
- 型: Int64
- 方向: Out

最初の行の露出開始から最後の行の露出開始までの時間差を報告する。

### LensShadingMapMode (ID: 10006)

- 定数名: `draft::LENS_SHADING_MAP_MODE`
- 型: Int32
- 方向: InOut

レンズシェーディングマップの利用可否を制御する。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `lens_shading_map_mode::OFF` | 無効 |
| 1 | `lens_shading_map_mode::ON` | 有効 |

### PipelineDepth (ID: 10007)

- 定数名: `draft::PIPELINE_DEPTH`
- 型: Int32
- 方向: Out

フレームが露出から最終結果として利用可能になるまでに通過したパイプラインステージ数を報告する。通常は 3 (露出、キャプチャ、ISP 処理)。

### MaxLatency (ID: 10008)

- 定数名: `draft::MAX_LATENCY`
- 型: Int32
- 方向: Out

リクエスト送信後、結果の状態が同期されるまでの最大フレーム数。-1 は不明、0 はフレームごとの制御を示す。

### TestPatternMode (ID: 10009)

- 定数名: `draft::TEST_PATTERN_MODE`
- 型: Int32
- 方向: InOut

テストパターンモードを選択する。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `test_pattern_mode::OFF` | テストパターンなし (通常のセンサー画像) |
| 1 | `test_pattern_mode::SOLID_COLOR` | 単色パターン |
| 2 | `test_pattern_mode::COLOR_BARS` | 8 本のカラーバー |
| 3 | `test_pattern_mode::COLOR_BARS_FADE_TO_GRAY` | グレーへフェードするカラーバー |
| 4 | `test_pattern_mode::PN9` | PN9 擬似ランダムシーケンス |
| 256 | `test_pattern_mode::CUSTOM1` | カスタムテストパターン |

### FaceDetectMode (ID: 10010)

- 定数名: `draft::FACE_DETECT_MODE`
- 型: Int32
- 方向: InOut

顔検出モードを選択する。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `face_detect_mode::OFF` | 顔検出なし |
| 1 | `face_detect_mode::SIMPLE` | 矩形とスコアを報告 (ランドマークと ID はオプション) |
| 2 | `face_detect_mode::FULL` | 矩形、スコア、ランドマーク、ID をすべて報告 |

### FaceDetectFaceRectangles (ID: 10011)

- 定数名: `draft::FACE_DETECT_FACE_RECTANGLES`
- 型: Rectangle (配列)
- 方向: Out

検出された顔の境界矩形。配列の要素数が検出された顔の数に対応する。メタデータでのみ返される。

### FaceDetectFaceScores (ID: 10012)

- 定数名: `draft::FACE_DETECT_FACE_SCORES`
- 型: Byte (配列)
- 方向: Out

検出された各顔の信頼度スコア (0-100)。要素数は FaceDetectFaceRectangles で報告された顔の数と一致する。メタデータでのみ返される。

### FaceDetectFaceLandmarks (ID: 10013)

- 定数名: `draft::FACE_DETECT_FACE_LANDMARKS`
- 型: Point (配列)
- 方向: Out

検出された顔のランドマーク座標。左目、右目、口の順で、顔ごとに 3 点。要素数は顔の数 x 3。メタデータでのみ返される。

### FaceDetectFaceIds (ID: 10014)

- 定数名: `draft::FACE_DETECT_FACE_IDS`
- 型: Int32 (配列)
- 方向: Out

検出された各顔に割り当てられた一意の ID。顔が視野から出て再び現れた場合、新しい ID が割り当てられる場合がある。メタデータでのみ返される。

## Raspberry Pi (14 件)

### StatsOutputEnable (ID: 20001)

- 定数名: `rpi::STATS_OUTPUT_ENABLE`
- 型: Bool
- 方向: InOut

Raspberry Pi IPA がハードウェア生成の統計情報を出力するかを制御する。true に設定すると、Bcm2835StatsOutput コントロールを通じてリクエストメタデータに統計情報のバイナリダンプが出力される。

### Bcm2835StatsOutput (ID: 20002)

- 定数名: `rpi::BCM2835_STATS_OUTPUT`
- 型: Byte (配列)
- 方向: Out

BCM2835 ISP が生成した現在のフレームの統計情報。StatsOutputEnable が true の場合にリクエストメタデータに含まれる。構造体定義は `include/linux/bcm2835-isp.h` を参照。

### ScalerCrops (ID: 20003)

- 定数名: `rpi::SCALER_CROPS`
- 型: Rectangle (配列)
- 方向: InOut

出力ストリームごとのスケーラークロップ領域の配列。矩形の順序はアプリケーションが設定したストリームの順序と一致する必要がある。ScalerCrop と ScalerCrops の両方が設定された場合、ScalerCrop は無視される。ストリームごとに異なるクロップ領域を使用できるのは Pi5/PiSP プラットフォームのみ。

### PispStatsOutput (ID: 20004)

- 定数名: `rpi::PISP_STATS_OUTPUT`
- 型: Byte (配列)
- 方向: Out

PiSP Frontend ISP が生成した現在のフレームの統計情報。StatsOutputEnable が true の場合にリクエストメタデータに含まれる。

### SyncMode (ID: 20005)

- 定数名: `rpi::SYNC_MODE`
- 型: Int32
- 方向: In

カメラ同期モードを有効/無効にする。同一デバイスまたは異なるデバイスに接続されたカメラ間でフレームを時間的に同期させる。サーバーがタイミング情報をクライアントにブロードキャストし、クライアントがフレームタイミングを調整する。全カメラが同じ固定フレームレートで動作する必要がある。

| 値 | 定数 | 説明 |
|----|------|------|
| 0 | `sync_mode::OFF` | 同期モード無効 |
| 1 | `sync_mode::SERVER` | サーバーとして動作 (タイミング情報をブロードキャスト) |
| 2 | `sync_mode::CLIENT` | クライアントとして動作 (サーバーに同期) |

### SyncReady (ID: 20006)

- 定数名: `rpi::SYNC_READY`
- 型: Bool
- 方向: Out

カメラ同期の準備完了状態を報告する。false の間はフレームを使用せず待機する。true になった時点がサーバーとクライアントの同期が確立されたフレームであり、以降は true のまま。

### SyncTimer (ID: 20007)

- 定数名: `rpi::SYNC_TIMER`
- 型: Int64
- 方向: Out

同期の準備完了時刻までの残り時間をマイクロ秒単位で報告する。正の値は準備完了時刻が未来であることを示し、0 に近づいてから負の値に転じる。SyncReady が true になるフレームで値が 0 またはそれに近い値となり、クライアントとサーバーの同期精度を示す。

### SyncFrames (ID: 20008)

- 定数名: `rpi::SYNC_FRAMES`
- 型: Int32
- 方向: In

サーバーが SyncModeServer を有効にしてから SyncReady でフレーム使用を通知するまでの待機フレーム数。サーバー側のデバイスにのみ設定する。

### CnnOutputTensor (ID: 20009)

- 定数名: `rpi::CNN_OUTPUT_TENSOR`
- 型: Float (配列)
- 方向: Out

CNN (畳み込みニューラルネットワーク) の出力テンソル。サイズとフォーマットは使用するニューラルネットワークに依存する。構造は CnnOutputTensorInfo で記述される。

### CnnOutputTensorInfo (ID: 20010)

- 定数名: `rpi::CNN_OUTPUT_TENSOR_INFO`
- 型: Byte (配列)
- 方向: Out

CnnOutputTensor の構造情報。ネットワーク名、テンソル数、各テンソルの要素数・次元数・サイズを含む。

### CnnEnableInputTensor (ID: 20011)

- 定数名: `rpi::CNN_ENABLE_INPUT_TENSOR`
- 型: Bool
- 方向: In

CNN 入力テンソルの出力を有効/無効にする。入力テンソルは比較的大きいため、デバッグ目的でない限り無効にすることが推奨される。

### CnnInputTensor (ID: 20012)

- 定数名: `rpi::CNN_INPUT_TENSOR`
- 型: Byte (配列)
- 方向: Out

CNN の入力テンソルのピクセル値。CnnEnableInputTensor が true の場合に出力される。構造は CnnInputTensorInfo で記述される。

### CnnInputTensorInfo (ID: 20013)

- 定数名: `rpi::CNN_INPUT_TENSOR_INFO`
- 型: Byte (配列)
- 方向: Out

CnnInputTensor の構造情報。ネットワーク名、幅、高さ、チャンネル数を含む。

### CnnKpiInfo (ID: 20014)

- 定数名: `rpi::CNN_KPI_INFO`
- 型: Int32 (配列, サイズ 2)
- 方向: Out

CNN 処理ステージのパフォーマンス指標。CNN/DNN ステージと DSP ステージのランタイムをミリ秒単位で報告する。
