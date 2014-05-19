#!/usr/bin/env python
from setuptools import setup, find_packages
import os,re

# print find_packages()

# make setup
setup(
    name='illusooUi',
    version="0.0.1",
    author='Claude BIDEAU',
    author_email='claude.bideau@gmail.com',
    url="http://fablab-lannion.org/",
    scripts=['scripts/rpcUi.py', 'scripts/rpcUi.bat'],
    install_requires = ['setuptools', 'pyside>=1.1.2'],
    license='GPL',
    packages = find_packages(),
    include_package_data = True,
    description='package use to provide pyside GUI to manage rpclib methods ....',
    long_description=open('README.txt').read(),
)
