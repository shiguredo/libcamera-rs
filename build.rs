use std::env;
use std::path::PathBuf;

fn main() {
    let out_dir = PathBuf::from(env::var("OUT_DIR").unwrap());
    let manifest_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap());
    let c_api_dir = manifest_dir.join("c-api");

    println!("cargo::rerun-if-changed=c-api/");

    // pkg-config で libcamera を検出
    let libcamera = pkg_config::Config::new()
        .probe("libcamera")
        .expect("libcamera not found. Please install libcamera-dev");

    // C++ ラッパーをコンパイル
    let mut build = cc::Build::new();
    build.cpp(true).std("c++17").include(&c_api_dir);

    for path in &libcamera.include_paths {
        build.include(path);
    }

    // 全 .cpp ファイルをコンパイル
    let cpp_files = [
        "camera_manager.cpp",
        "camera.cpp",
        "configuration.cpp",
        "stream.cpp",
        "request.cpp",
        "frame_buffer.cpp",
        "controls.cpp",
    ];
    for file in &cpp_files {
        build.file(c_api_dir.join(file));
    }

    build.compile("camera_wrapper");

    // libstdc++ をリンク
    println!("cargo::rustc-link-lib=stdc++");

    // bindgen でバインディングを生成
    let mut builder = bindgen::Builder::default()
        .header(c_api_dir.join("libcamera.h").to_str().unwrap())
        .allowlist_function("lc_.*")
        .allowlist_type("lc_.*")
        .allowlist_var("LC_.*")
        .derive_default(true)
        .derive_debug(true)
        .parse_callbacks(Box::new(bindgen::CargoCallbacks::new()));

    for path in &libcamera.include_paths {
        builder = builder.clang_arg(format!("-I{}", path.display()));
    }

    let bindings = builder.generate().expect("Failed to generate bindings");

    bindings
        .write_to_file(out_dir.join("bindings.rs"))
        .expect("Failed to write bindings");
}
