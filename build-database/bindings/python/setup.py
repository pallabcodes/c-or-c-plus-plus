"""
Setup script for AuroraDB Python SDK
"""

from setuptools import setup, find_packages
import os

# Read README
this_directory = os.path.abspath(os.path.dirname(__file__))
with open(os.path.join(this_directory, 'README.md'), encoding='utf-8') as f:
    long_description = f.read()

# Read version
with open(os.path.join(this_directory, 'auroradb', '__init__.py'), 'r') as f:
    for line in f:
        if line.startswith('__version__'):
            version = line.split('=')[1].strip().strip("'\"")
            break

setup(
    name="auroradb",
    version=version,
    author="AuroraDB Team",
    author_email="team@auroradb.com",
    description="Python SDK for AuroraDB - Revolutionary Database for AI-Native Applications",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/auroradb/auroradb-python",
    packages=find_packages(),
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: Apache Software License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Topic :: Database",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Topic :: Software Development :: Libraries :: Python Modules",
    ],
    keywords="database, vector, search, ai, ml, sql, embeddings, similarity",
    python_requires=">=3.8",
    install_requires=[
        "requests>=2.28.0",
        "numpy>=1.21.0",
        "dataclasses>=0.6; python_version < '3.7'",
    ],
    extras_require={
        "dev": [
            "pytest>=7.0.0",
            "pytest-asyncio>=0.21.0",
            "black>=22.0.0",
            "isort>=5.10.0",
            "mypy>=0.950",
            "flake8>=4.0.0",
        ],
        "docs": [
            "sphinx>=4.0.0",
            "sphinx-rtd-theme>=1.0.0",
        ],
        "examples": [
            "pandas>=1.3.0",
            "matplotlib>=3.5.0",
            "scikit-learn>=1.0.0",
            "transformers>=4.0.0",
            "torch>=1.9.0",
            "sentence-transformers>=2.0.0",
        ],
    },
    entry_points={
        "console_scripts": [
            "auroradb=auroradb.cli:main",
        ],
    },
    project_urls={
        "Documentation": "https://docs.auroradb.com",
        "Source": "https://github.com/auroradb/auroradb",
        "Tracker": "https://github.com/auroradb/auroradb/issues",
        "Discord": "https://discord.gg/auroradb",
    },
)
