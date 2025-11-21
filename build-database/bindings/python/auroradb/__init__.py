"""
AuroraDB Python SDK

A revolutionary database for AI-native applications with full SQL support,
ACID transactions, and state-of-the-art vector search capabilities.

Example:
    >>> from auroradb import AuroraDB
    >>> db = AuroraDB("http://localhost:8080")
    >>> db.sql("CREATE TABLE products (id INTEGER, name TEXT, embedding VECTOR(384))")
    >>> db.vector_search([0.1, 0.2, ...], limit=10)
"""

__version__ = "0.1.0"
__author__ = "AuroraDB Team"

from .client import AuroraDB
from .vector import VectorCollection
from .sql import SQLClient

__all__ = ["AuroraDB", "VectorCollection", "SQLClient"]
