use crate::sys as ffi;

/// mmap されたプレーンデータの RAII ラッパー
///
/// Drop 時に munmap を呼び出す。
pub struct MappedPlane {
    ptr: *mut libc::c_void,
    len: usize,
}

// mmap されたメモリはプロセス内でスレッド間共有可能
unsafe impl Send for MappedPlane {}
unsafe impl Sync for MappedPlane {}

impl MappedPlane {
    /// プレーンデータのスライスを返す
    pub fn as_slice(&self) -> &[u8] {
        unsafe { std::slice::from_raw_parts(self.ptr as *const u8, self.len) }
    }

    /// プレーンデータのバイト長を返す
    pub fn len(&self) -> usize {
        self.len
    }

    /// プレーンデータが空かどうかを返す
    pub fn is_empty(&self) -> bool {
        self.len == 0
    }
}

impl Drop for MappedPlane {
    fn drop(&mut self) {
        unsafe { libc::munmap(self.ptr, self.len) };
    }
}

/// フレームバッファのプレーン情報
#[derive(Debug, Clone, Copy)]
pub struct FrameBufferPlane {
    pub fd: i32,
    pub offset: u32,
    pub length: u32,
}

/// フレームステータス
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum FrameStatus {
    Success,
    Error,
    Cancelled,
    Startup,
}

/// フレームメタデータ
#[derive(Debug, Clone, Copy)]
pub struct FrameMetadata {
    pub status: FrameStatus,
    pub sequence: u32,
    pub timestamp: u64,
}

/// フレームバッファ
///
/// Allocator が所有するバッファへの参照。所有権は持たない。
pub struct FrameBuffer {
    pub(crate) ptr: *mut ffi::lc_frame_buffer_t,
}

// FrameBuffer は内部ポインタのラッパーであり libcamera 側でスレッドセーフ
unsafe impl Send for FrameBuffer {}
unsafe impl Sync for FrameBuffer {}

impl FrameBuffer {
    pub(crate) fn from_raw(ptr: *mut ffi::lc_frame_buffer_t) -> Self {
        Self { ptr }
    }

    /// プレーン数を返す
    pub fn planes_count(&self) -> usize {
        unsafe { ffi::lc_frame_buffer_planes_count(self.ptr) }
    }

    /// 指定インデックスのプレーン情報を返す
    pub fn plane(&self, index: usize) -> Option<FrameBufferPlane> {
        if index >= self.planes_count() {
            return None;
        }
        let raw = unsafe { ffi::lc_frame_buffer_get_plane(self.ptr, index) };
        if raw.fd < 0 {
            return None;
        }
        Some(FrameBufferPlane {
            fd: raw.fd,
            offset: raw.offset,
            length: raw.length,
        })
    }

    /// フレームメタデータを返す
    pub fn metadata(&self) -> FrameMetadata {
        let raw = unsafe { ffi::lc_frame_buffer_metadata(self.ptr) };
        let status = match raw.status {
            0 => FrameStatus::Success,
            1 => FrameStatus::Error,
            2 => FrameStatus::Cancelled,
            _ => FrameStatus::Startup,
        };
        FrameMetadata {
            status,
            sequence: raw.sequence,
            timestamp: raw.timestamp,
        }
    }
}

impl Drop for FrameBuffer {
    fn drop(&mut self) {
        unsafe { ffi::lc_frame_buffer_ref_destroy(self.ptr) };
    }
}

/// コールバック内で使うフレームバッファ参照
///
/// Drop で C 側ラッパーを解放しない (所有権を持たない)。
pub struct FrameBufferRef<'a> {
    pub(crate) ptr: *mut ffi::lc_frame_buffer_t,
    pub(crate) _phantom: std::marker::PhantomData<&'a ()>,
}

impl FrameBufferRef<'_> {
    /// プレーン数を返す
    pub fn planes_count(&self) -> usize {
        unsafe { ffi::lc_frame_buffer_planes_count(self.ptr) }
    }

    /// 指定インデックスのプレーン情報を返す
    pub fn plane(&self, index: usize) -> Option<FrameBufferPlane> {
        if index >= self.planes_count() {
            return None;
        }
        let raw = unsafe { ffi::lc_frame_buffer_get_plane(self.ptr, index) };
        if raw.fd < 0 {
            return None;
        }
        Some(FrameBufferPlane {
            fd: raw.fd,
            offset: raw.offset,
            length: raw.length,
        })
    }

    /// フレームメタデータを返す
    pub fn metadata(&self) -> FrameMetadata {
        let raw = unsafe { ffi::lc_frame_buffer_metadata(self.ptr) };
        let status = match raw.status {
            0 => FrameStatus::Success,
            1 => FrameStatus::Error,
            2 => FrameStatus::Cancelled,
            _ => FrameStatus::Startup,
        };
        FrameMetadata {
            status,
            sequence: raw.sequence,
            timestamp: raw.timestamp,
        }
    }

    /// 指定インデックスのプレーンを mmap してデータを返す
    ///
    /// 失敗した場合は None を返す。
    pub fn map_plane(&self, index: usize) -> Option<MappedPlane> {
        let plane = self.plane(index)?;
        let len = plane.length as usize;
        if len == 0 {
            return None;
        }
        let ptr = unsafe {
            libc::mmap(
                std::ptr::null_mut(),
                len,
                libc::PROT_READ,
                libc::MAP_SHARED,
                plane.fd,
                plane.offset as libc::off_t,
            )
        };
        if ptr == libc::MAP_FAILED {
            return None;
        }
        Some(MappedPlane { ptr, len })
    }
}
