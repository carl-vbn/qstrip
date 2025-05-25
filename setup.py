from setuptools import setup, Extension

ext_modules = [
    Extension(
        "qstrip._core",
        sources=["qstrip/_core/markdown.c"],
        include_dirs=["qstrip/_core"],
    )
]

setup(
    name="qstrip",
    version="0.1.0",
    packages=["qstrip"],
    ext_modules=ext_modules,
    zip_safe=False,
)