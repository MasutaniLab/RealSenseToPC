@echo off
if not defined RTM_VC_CONFIG set RTM_VC_CONFIG=Debug
start "%NAME%Comp" "build\src\%RTM_VC_CONFIG%\%~n0.exe"
