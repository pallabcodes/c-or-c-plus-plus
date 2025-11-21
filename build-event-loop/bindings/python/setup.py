#!/usr/bin/env python3
"""
Setup script for Cyclone Python bindings
"""

from setuptools import setup, Extension, find_packages
import os
import platform
import subprocess

def get_cyclone_version():
    """Get Cyclone version from Cargo.toml"""
    try:
        with open('../../../Cargo.toml', 'r') as f:
            for line in f:
                if line.startswith('version = '):
                    return line.split('"')[1]
    except:
        pass
    return "2.0.0"

def build_cyclone_library():
    """Build Cyclone shared library"""
    print("Building Cyclone shared library...")

    # Build with Cargo
    result = subprocess.run([
        'cargo', 'build', '--release',
        '--features', 'full-optimization'
    ], cwd='../..')

    if result.returncode != 0:
        raise RuntimeError("Failed to build Cyclone library")

    print("Cyclone library built successfully")

# Build the library before setup
build_cyclone_library()

# Determine library extension
system = platform.system().lower()
if system == "linux":
    lib_ext = ".so"
elif system == "darwin":
    lib_ext = ".dylib"
elif system == "windows":
    lib_ext = ".dll"
else:
    raise RuntimeError(f"Unsupported platform: {system}")

setup(
    name="cyclone-python",
    version=get_cyclone_version(),
    description="High-performance networking for Python with 2M+ RPS capability",
    long_description="""
Cyclone Python Bindings

Leverage Cyclone's bleeding-edge networking research from Python:

🚀 Features:
- 2M+ RPS sustained throughput
- RDMA-accelerated database queries (5µs latency)
- DPDK user-space packet processing
- XDP kernel-level DDoS protection
- SIMD-accelerated data processing
- Zero-copy networking
- Memory safety guarantees from Rust

📚 Example:
    import cyclone

    app = cyclone.WebApp()
    app.configure(target_rps=2000000, enable_rdma=True)

    @app.route("GET", "/api/data")
    def get_data(request):
        return cyclone.json_response({"performance": "2M_RPS"})

    app.run()
    """,
    long_description_content_type="text/markdown",
    author="Cyclone Team",
    author_email="cyclone@cyclone.dev",
    url="https://github.com/cyclone-rs/cyclone",
    packages=find_packages(),
    package_data={
        'cyclone': [f'libcyclone{lib_ext}'],
    },
    include_package_data=True,
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Topic :: Internet :: WWW/HTTP",
        "Topic :: System :: Networking",
        "Topic :: Software Development :: Libraries :: Python Modules",
    ],
    python_requires=">=3.8",
    keywords=[
        "networking", "high-performance", "web-framework", "async",
        "rdma", "dpdk", "xdp", "simd", "zero-copy", "rust"
    ],
    project_urls={
        "Documentation": "https://docs.cyclone.dev/python",
        "Source": "https://github.com/cyclone-rs/cyclone",
        "Tracker": "https://github.com/cyclone-rs/cyclone/issues",
    },
)
