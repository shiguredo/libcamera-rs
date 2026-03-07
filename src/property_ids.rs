use crate::controls::{ControlId, ControlType, Direction};

/// コアプロパティ ID (libcamera vendor, base = 0)
pub mod core {
    use super::*;

    pub const LOCATION: ControlId =
        ControlId::new(1, "Location", ControlType::Int32, Direction::Out);
    pub const ROTATION: ControlId =
        ControlId::new(2, "Rotation", ControlType::Int32, Direction::Out);
    pub const MODEL: ControlId =
        ControlId::new(3, "Model", ControlType::String, Direction::Out);
    pub const UNIT_CELL_SIZE: ControlId =
        ControlId::new(4, "UnitCellSize", ControlType::Size, Direction::Out);
    pub const PIXEL_ARRAY_SIZE: ControlId =
        ControlId::new(5, "PixelArraySize", ControlType::Size, Direction::Out);
    pub const PIXEL_ARRAY_OPTICAL_BLACK_RECTANGLES: ControlId = ControlId::new(
        6,
        "PixelArrayOpticalBlackRectangles",
        ControlType::Rectangle,
        Direction::Out,
    );
    pub const PIXEL_ARRAY_ACTIVE_AREAS: ControlId = ControlId::new(
        7,
        "PixelArrayActiveAreas",
        ControlType::Rectangle,
        Direction::Out,
    );
    pub const SCALER_CROP_MAXIMUM: ControlId = ControlId::new(
        8,
        "ScalerCropMaximum",
        ControlType::Rectangle,
        Direction::Out,
    );
    pub const SENSOR_SENSITIVITY: ControlId =
        ControlId::new(9, "SensorSensitivity", ControlType::Float, Direction::Out);
    pub const SYSTEM_DEVICES: ControlId =
        ControlId::new(10, "SystemDevices", ControlType::Int64, Direction::Out);

    /// Location の列挙値
    pub mod location {
        pub const FRONT: i32 = 0;
        pub const BACK: i32 = 1;
        pub const EXTERNAL: i32 = 2;
    }
}

/// ドラフトプロパティ ID (draft vendor, base = 10000)
pub mod draft {
    use super::*;

    pub const COLOR_FILTER_ARRANGEMENT: ControlId = ControlId::new(
        10001,
        "ColorFilterArrangement",
        ControlType::Int32,
        Direction::Out,
    );

    /// ColorFilterArrangement の列挙値
    pub mod color_filter_arrangement {
        pub const RGGB: i32 = 0;
        pub const GRBG: i32 = 1;
        pub const GBRG: i32 = 2;
        pub const BGGR: i32 = 3;
        pub const RGB: i32 = 4;
        pub const MONO: i32 = 5;
    }
}
