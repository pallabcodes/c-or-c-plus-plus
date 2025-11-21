"""
Cyclone Python Bindings

High-performance networking for Python applications with 2M+ RPS capability.
Leverages Rust's memory safety and Cyclone's bleeding-edge research.

Example:
    import cyclone

    app = cyclone.WebApp()
    app.configure(target_rps=2000000, enable_rdma=True)

    @app.route("GET", "/api/data")
    def get_data(request):
        return cyclone.json_response({"data": "high-performance!"})

    app.run()
"""

from .cyclone import (
    init,
    shutdown,
    WebApp,
    json_response,
    text_response,
    html_response,
    Metrics,
    version,
    has_feature,
)

__version__ = "2.0.0"
__all__ = [
    "init",
    "shutdown",
    "WebApp",
    "json_response",
    "text_response",
    "html_response",
    "Metrics",
    "version",
    "has_feature",
]
