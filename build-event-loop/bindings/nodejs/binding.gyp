{
  "targets": [
    {
      "target_name": "cyclone",
      "sources": [
        "src/cyclone.cc",
        "src/web_app.cc",
        "src/metrics.cc"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "defines": [
        "NAPI_DISABLE_CPP_EXCEPTIONS",
        "NAPI_VERSION=6"
      ],
      "conditions": [
        ["OS=='linux'", {
          "libraries": [
            "<(module_root_dir)/../../../target/release/libcyclone.so"
          ],
          "include_dirs": [
            "../../../target/release/build/cyclone-*/out"
          ]
        }],
        ["OS=='mac'", {
          "libraries": [
            "<(module_root_dir)/../../../target/release/libcyclone.dylib"
          ]
        }],
        ["OS=='win'", {
          "libraries": [
            "<(module_root_dir)/../../../target/release/cyclone.lib"
          ]
        }]
      ],
      "cflags_cc": [
        "-std=c++17",
        "-O3",
        "-march=native"
      ]
    }
  ]
}
