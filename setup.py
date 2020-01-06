from setuptools import setup, find_packages

setup(
    name="bio",
    version="0.1",
    requires=["pysam"],
    description="algorithm prototypes for biological data",
    packages=find_packages(),
    entry_points={"console_scripts": ["countreads=bio.countreads:main"],},
)
