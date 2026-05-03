@echo off
pushd %~dp0..
python3 Tools/codegen.py
popd
pause
