use crate::controls::{ControlId, ControlType, Direction};

/// コアコントロール ID (libcamera vendor, base = 0)
pub mod core {
    use super::*;

    pub const AE_ENABLE: ControlId =
        ControlId::new(1, "AeEnable", ControlType::Bool, Direction::In);
    pub const AE_STATE: ControlId =
        ControlId::new(2, "AeState", ControlType::Int32, Direction::Out);
    pub const AE_METERING_MODE: ControlId =
        ControlId::new(3, "AeMeteringMode", ControlType::Int32, Direction::InOut);
    pub const AE_CONSTRAINT_MODE: ControlId =
        ControlId::new(4, "AeConstraintMode", ControlType::Int32, Direction::InOut);
    pub const AE_EXPOSURE_MODE: ControlId =
        ControlId::new(5, "AeExposureMode", ControlType::Int32, Direction::InOut);
    pub const EXPOSURE_VALUE: ControlId =
        ControlId::new(6, "ExposureValue", ControlType::Float, Direction::InOut);
    pub const EXPOSURE_TIME: ControlId =
        ControlId::new(7, "ExposureTime", ControlType::Int32, Direction::InOut);
    pub const EXPOSURE_TIME_MODE: ControlId =
        ControlId::new(8, "ExposureTimeMode", ControlType::Int32, Direction::InOut);
    pub const ANALOGUE_GAIN: ControlId =
        ControlId::new(9, "AnalogueGain", ControlType::Float, Direction::InOut);
    pub const ANALOGUE_GAIN_MODE: ControlId =
        ControlId::new(10, "AnalogueGainMode", ControlType::Int32, Direction::InOut);
    pub const AE_FLICKER_MODE: ControlId =
        ControlId::new(11, "AeFlickerMode", ControlType::Int32, Direction::InOut);
    pub const AE_FLICKER_PERIOD: ControlId =
        ControlId::new(12, "AeFlickerPeriod", ControlType::Int32, Direction::InOut);
    pub const AE_FLICKER_DETECTED: ControlId =
        ControlId::new(13, "AeFlickerDetected", ControlType::Int32, Direction::Out);
    pub const BRIGHTNESS: ControlId =
        ControlId::new(14, "Brightness", ControlType::Float, Direction::InOut);
    pub const CONTRAST: ControlId =
        ControlId::new(15, "Contrast", ControlType::Float, Direction::InOut);
    pub const LUX: ControlId = ControlId::new(16, "Lux", ControlType::Float, Direction::Out);
    pub const AWB_ENABLE: ControlId =
        ControlId::new(17, "AwbEnable", ControlType::Bool, Direction::InOut);
    pub const AWB_MODE: ControlId =
        ControlId::new(18, "AwbMode", ControlType::Int32, Direction::InOut);
    pub const AWB_LOCKED: ControlId =
        ControlId::new(19, "AwbLocked", ControlType::Bool, Direction::Out);
    pub const COLOUR_GAINS: ControlId =
        ControlId::new(20, "ColourGains", ControlType::Float, Direction::InOut);
    pub const COLOUR_TEMPERATURE: ControlId =
        ControlId::new(21, "ColourTemperature", ControlType::Int32, Direction::Out);
    pub const SATURATION: ControlId =
        ControlId::new(22, "Saturation", ControlType::Float, Direction::InOut);
    pub const SENSOR_BLACK_LEVELS: ControlId =
        ControlId::new(23, "SensorBlackLevels", ControlType::Int32, Direction::Out);
    pub const SHARPNESS: ControlId =
        ControlId::new(24, "Sharpness", ControlType::Float, Direction::InOut);
    pub const FOCUS_FOM: ControlId =
        ControlId::new(25, "FocusFoM", ControlType::Int32, Direction::Out);
    pub const COLOUR_CORRECTION_MATRIX: ControlId = ControlId::new(
        26,
        "ColourCorrectionMatrix",
        ControlType::Float,
        Direction::InOut,
    );
    pub const SCALER_CROP: ControlId =
        ControlId::new(27, "ScalerCrop", ControlType::Rectangle, Direction::InOut);
    pub const DIGITAL_GAIN: ControlId =
        ControlId::new(28, "DigitalGain", ControlType::Float, Direction::InOut);
    pub const FRAME_DURATION: ControlId =
        ControlId::new(29, "FrameDuration", ControlType::Int64, Direction::Out);
    pub const FRAME_DURATION_LIMITS: ControlId = ControlId::new(
        30,
        "FrameDurationLimits",
        ControlType::Int64,
        Direction::InOut,
    );
    pub const SENSOR_TEMPERATURE: ControlId =
        ControlId::new(31, "SensorTemperature", ControlType::Float, Direction::Out);
    pub const SENSOR_TIMESTAMP: ControlId =
        ControlId::new(32, "SensorTimestamp", ControlType::Int64, Direction::Out);
    pub const AF_MODE: ControlId =
        ControlId::new(33, "AfMode", ControlType::Int32, Direction::InOut);
    pub const AF_RANGE: ControlId =
        ControlId::new(34, "AfRange", ControlType::Int32, Direction::InOut);
    pub const AF_SPEED: ControlId =
        ControlId::new(35, "AfSpeed", ControlType::Int32, Direction::InOut);
    pub const AF_METERING: ControlId =
        ControlId::new(36, "AfMetering", ControlType::Int32, Direction::InOut);
    pub const AF_WINDOWS: ControlId =
        ControlId::new(37, "AfWindows", ControlType::Rectangle, Direction::InOut);
    pub const AF_TRIGGER: ControlId =
        ControlId::new(38, "AfTrigger", ControlType::Int32, Direction::In);
    pub const AF_PAUSE: ControlId =
        ControlId::new(39, "AfPause", ControlType::Int32, Direction::In);
    pub const LENS_POSITION: ControlId =
        ControlId::new(40, "LensPosition", ControlType::Float, Direction::InOut);
    pub const AF_STATE: ControlId =
        ControlId::new(41, "AfState", ControlType::Int32, Direction::Out);
    pub const AF_PAUSE_STATE: ControlId =
        ControlId::new(42, "AfPauseState", ControlType::Int32, Direction::Out);
    pub const HDR_MODE: ControlId =
        ControlId::new(43, "HdrMode", ControlType::Int32, Direction::InOut);
    pub const HDR_CHANNEL: ControlId =
        ControlId::new(44, "HdrChannel", ControlType::Int32, Direction::Out);
    pub const GAMMA: ControlId = ControlId::new(45, "Gamma", ControlType::Float, Direction::InOut);
    pub const DEBUG_METADATA_ENABLE: ControlId = ControlId::new(
        46,
        "DebugMetadataEnable",
        ControlType::Bool,
        Direction::InOut,
    );
    pub const FRAME_WALL_CLOCK: ControlId =
        ControlId::new(47, "FrameWallClock", ControlType::Int64, Direction::Out);
    pub const WDR_MODE: ControlId =
        ControlId::new(48, "WdrMode", ControlType::Int32, Direction::InOut);
    pub const WDR_STRENGTH: ControlId =
        ControlId::new(49, "WdrStrength", ControlType::Float, Direction::In);
    pub const WDR_MAX_BRIGHT_PIXELS: ControlId =
        ControlId::new(50, "WdrMaxBrightPixels", ControlType::Float, Direction::In);
    pub const LENS_DEWARP_ENABLE: ControlId =
        ControlId::new(51, "LensDewarpEnable", ControlType::Bool, Direction::InOut);
    pub const LENS_SHADING_CORRECTION_ENABLE: ControlId = ControlId::new(
        52,
        "LensShadingCorrectionEnable",
        ControlType::Bool,
        Direction::InOut,
    );

    /// AeState の列挙値
    pub mod ae_state {
        pub const IDLE: i32 = 0;
        pub const SEARCHING: i32 = 1;
        pub const CONVERGED: i32 = 2;
    }

    /// AeMeteringMode の列挙値
    pub mod ae_metering_mode {
        pub const CENTRE_WEIGHTED: i32 = 0;
        pub const SPOT: i32 = 1;
        pub const MATRIX: i32 = 2;
        pub const CUSTOM: i32 = 3;
    }

    /// AeConstraintMode の列挙値
    pub mod ae_constraint_mode {
        pub const NORMAL: i32 = 0;
        pub const HIGHLIGHT: i32 = 1;
        pub const SHADOWS: i32 = 2;
        pub const CUSTOM: i32 = 3;
    }

    /// AeExposureMode の列挙値
    pub mod ae_exposure_mode {
        pub const NORMAL: i32 = 0;
        pub const SHORT: i32 = 1;
        pub const LONG: i32 = 2;
        pub const CUSTOM: i32 = 3;
    }

    /// ExposureTimeMode の列挙値
    pub mod exposure_time_mode {
        pub const AUTO: i32 = 0;
        pub const MANUAL: i32 = 1;
    }

    /// AnalogueGainMode の列挙値
    pub mod analogue_gain_mode {
        pub const AUTO: i32 = 0;
        pub const MANUAL: i32 = 1;
    }

    /// AeFlickerMode の列挙値
    pub mod ae_flicker_mode {
        pub const OFF: i32 = 0;
        pub const MANUAL: i32 = 1;
        pub const AUTO: i32 = 2;
    }

    /// AwbMode の列挙値
    pub mod awb_mode {
        pub const AUTO: i32 = 0;
        pub const INCANDESCENT: i32 = 1;
        pub const TUNGSTEN: i32 = 2;
        pub const FLUORESCENT: i32 = 3;
        pub const INDOOR: i32 = 4;
        pub const DAYLIGHT: i32 = 5;
        pub const CLOUDY: i32 = 6;
        pub const CUSTOM: i32 = 7;
    }

    /// AfMode の列挙値
    pub mod af_mode {
        pub const MANUAL: i32 = 0;
        pub const AUTO: i32 = 1;
        pub const CONTINUOUS: i32 = 2;
    }

    /// AfRange の列挙値
    pub mod af_range {
        pub const NORMAL: i32 = 0;
        pub const MACRO: i32 = 1;
        pub const FULL: i32 = 2;
    }

    /// AfSpeed の列挙値
    pub mod af_speed {
        pub const NORMAL: i32 = 0;
        pub const FAST: i32 = 1;
    }

    /// AfMetering の列挙値
    pub mod af_metering {
        pub const AUTO: i32 = 0;
        pub const WINDOWS: i32 = 1;
    }

    /// AfTrigger の列挙値
    pub mod af_trigger {
        pub const START: i32 = 0;
        pub const CANCEL: i32 = 1;
    }

    /// AfPause の列挙値
    pub mod af_pause {
        pub const IMMEDIATE: i32 = 0;
        pub const DEFERRED: i32 = 1;
        pub const RESUME: i32 = 2;
    }

    /// AfState の列挙値
    pub mod af_state {
        pub const IDLE: i32 = 0;
        pub const SCANNING: i32 = 1;
        pub const FOCUSED: i32 = 2;
        pub const FAILED: i32 = 3;
    }

    /// AfPauseState の列挙値
    pub mod af_pause_state {
        pub const RUNNING: i32 = 0;
        pub const PAUSING: i32 = 1;
        pub const PAUSED: i32 = 2;
    }

    /// HdrMode の列挙値
    pub mod hdr_mode {
        pub const OFF: i32 = 0;
        pub const MULTI_EXPOSURE_UNMERGED: i32 = 1;
        pub const MULTI_EXPOSURE: i32 = 2;
        pub const SINGLE_EXPOSURE: i32 = 3;
        pub const NIGHT: i32 = 4;
    }

    /// HdrChannel の列挙値
    pub mod hdr_channel {
        pub const NONE: i32 = 0;
        pub const SHORT: i32 = 1;
        pub const MEDIUM: i32 = 2;
        pub const LONG: i32 = 3;
    }

    /// WdrMode の列挙値
    pub mod wdr_mode {
        pub const OFF: i32 = 0;
        pub const LINEAR: i32 = 1;
        pub const POWER: i32 = 2;
        pub const EXPONENTIAL: i32 = 3;
        pub const HISTOGRAM_EQUALIZATION: i32 = 4;
    }
}

/// ドラフトコントロール ID (draft vendor, base = 10000)
pub mod draft {
    use super::*;

    pub const AE_PRECAPTURE_TRIGGER: ControlId = ControlId::new(
        10001,
        "AePrecaptureTrigger",
        ControlType::Int32,
        Direction::InOut,
    );
    pub const NOISE_REDUCTION_MODE: ControlId = ControlId::new(
        10002,
        "NoiseReductionMode",
        ControlType::Int32,
        Direction::InOut,
    );
    pub const COLOR_CORRECTION_ABERRATION_MODE: ControlId = ControlId::new(
        10003,
        "ColorCorrectionAberrationMode",
        ControlType::Int32,
        Direction::InOut,
    );
    pub const AWB_STATE: ControlId =
        ControlId::new(10004, "AwbState", ControlType::Int32, Direction::Out);
    pub const SENSOR_ROLLING_SHUTTER_SKEW: ControlId = ControlId::new(
        10005,
        "SensorRollingShutterSkew",
        ControlType::Int64,
        Direction::Out,
    );
    pub const LENS_SHADING_MAP_MODE: ControlId = ControlId::new(
        10006,
        "LensShadingMapMode",
        ControlType::Int32,
        Direction::InOut,
    );
    pub const PIPELINE_DEPTH: ControlId =
        ControlId::new(10007, "PipelineDepth", ControlType::Int32, Direction::Out);
    pub const MAX_LATENCY: ControlId =
        ControlId::new(10008, "MaxLatency", ControlType::Int32, Direction::Out);
    pub const TEST_PATTERN_MODE: ControlId = ControlId::new(
        10009,
        "TestPatternMode",
        ControlType::Int32,
        Direction::InOut,
    );
    pub const FACE_DETECT_MODE: ControlId = ControlId::new(
        10010,
        "FaceDetectMode",
        ControlType::Int32,
        Direction::InOut,
    );
    pub const FACE_DETECT_FACE_RECTANGLES: ControlId = ControlId::new(
        10011,
        "FaceDetectFaceRectangles",
        ControlType::Rectangle,
        Direction::Out,
    );
    pub const FACE_DETECT_FACE_SCORES: ControlId = ControlId::new(
        10012,
        "FaceDetectFaceScores",
        ControlType::Byte,
        Direction::Out,
    );
    pub const FACE_DETECT_FACE_LANDMARKS: ControlId = ControlId::new(
        10013,
        "FaceDetectFaceLandmarks",
        ControlType::Point,
        Direction::Out,
    );
    pub const FACE_DETECT_FACE_IDS: ControlId = ControlId::new(
        10014,
        "FaceDetectFaceIds",
        ControlType::Int32,
        Direction::Out,
    );

    /// AePrecaptureTrigger の列挙値
    pub mod ae_precapture_trigger {
        pub const IDLE: i32 = 0;
        pub const START: i32 = 1;
        pub const CANCEL: i32 = 2;
    }

    /// NoiseReductionMode の列挙値
    pub mod noise_reduction_mode {
        pub const OFF: i32 = 0;
        pub const FAST: i32 = 1;
        pub const HIGH_QUALITY: i32 = 2;
        pub const MINIMAL: i32 = 3;
        pub const ZSL: i32 = 4;
    }

    /// ColorCorrectionAberrationMode の列挙値
    pub mod color_correction_aberration_mode {
        pub const OFF: i32 = 0;
        pub const FAST: i32 = 1;
        pub const HIGH_QUALITY: i32 = 2;
    }

    /// AwbState の列挙値
    pub mod awb_state {
        pub const INACTIVE: i32 = 0;
        pub const SEARCHING: i32 = 1;
        pub const CONVERGED: i32 = 2;
        pub const LOCKED: i32 = 3;
    }

    /// LensShadingMapMode の列挙値
    pub mod lens_shading_map_mode {
        pub const OFF: i32 = 0;
        pub const ON: i32 = 1;
    }

    /// TestPatternMode の列挙値
    pub mod test_pattern_mode {
        pub const OFF: i32 = 0;
        pub const SOLID_COLOR: i32 = 1;
        pub const COLOR_BARS: i32 = 2;
        pub const COLOR_BARS_FADE_TO_GRAY: i32 = 3;
        pub const PN9: i32 = 4;
        pub const CUSTOM1: i32 = 256;
    }

    /// FaceDetectMode の列挙値
    pub mod face_detect_mode {
        pub const OFF: i32 = 0;
        pub const SIMPLE: i32 = 1;
        pub const FULL: i32 = 2;
    }
}

/// Raspberry Pi ベンダーコントロール ID (rpi vendor, base = 20000)
pub mod rpi {
    use super::*;

    pub const STATS_OUTPUT_ENABLE: ControlId = ControlId::new(
        20001,
        "StatsOutputEnable",
        ControlType::Bool,
        Direction::InOut,
    );
    pub const BCM2835_STATS_OUTPUT: ControlId = ControlId::new(
        20002,
        "Bcm2835StatsOutput",
        ControlType::Byte,
        Direction::Out,
    );
    pub const SCALER_CROPS: ControlId = ControlId::new(
        20003,
        "ScalerCrops",
        ControlType::Rectangle,
        Direction::InOut,
    );
    pub const PISP_STATS_OUTPUT: ControlId =
        ControlId::new(20004, "PispStatsOutput", ControlType::Byte, Direction::Out);
    pub const SYNC_MODE: ControlId =
        ControlId::new(20005, "SyncMode", ControlType::Int32, Direction::In);
    pub const SYNC_READY: ControlId =
        ControlId::new(20006, "SyncReady", ControlType::Bool, Direction::Out);
    pub const SYNC_TIMER: ControlId =
        ControlId::new(20007, "SyncTimer", ControlType::Int64, Direction::Out);
    pub const SYNC_FRAMES: ControlId =
        ControlId::new(20008, "SyncFrames", ControlType::Int32, Direction::In);
    pub const CNN_OUTPUT_TENSOR: ControlId =
        ControlId::new(20009, "CnnOutputTensor", ControlType::Float, Direction::Out);
    pub const CNN_OUTPUT_TENSOR_INFO: ControlId = ControlId::new(
        20010,
        "CnnOutputTensorInfo",
        ControlType::Byte,
        Direction::Out,
    );
    pub const CNN_ENABLE_INPUT_TENSOR: ControlId = ControlId::new(
        20011,
        "CnnEnableInputTensor",
        ControlType::Bool,
        Direction::In,
    );
    pub const CNN_INPUT_TENSOR: ControlId =
        ControlId::new(20012, "CnnInputTensor", ControlType::Byte, Direction::Out);
    pub const CNN_INPUT_TENSOR_INFO: ControlId = ControlId::new(
        20013,
        "CnnInputTensorInfo",
        ControlType::Byte,
        Direction::Out,
    );
    pub const CNN_KPI_INFO: ControlId =
        ControlId::new(20014, "CnnKpiInfo", ControlType::Int32, Direction::Out);

    /// SyncMode の列挙値
    pub mod sync_mode {
        pub const OFF: i32 = 0;
        pub const SERVER: i32 = 1;
        pub const CLIENT: i32 = 2;
    }
}
