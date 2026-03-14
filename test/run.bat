clang -o test_core.exe test\test_core.c -I. -std=c23 -ggdb -Wall -Wextra -Wpedantic -Wswitch -Wno-deprecated-declarations -L"C:\Program Files\Microsoft Visual Studio\2019\VC\Tools\MSVC\14.29.30133\lib\x64" -L"D:\Windows Kits\10\Lib\10.0.26100.0\um\x64" -L"D:\Windows Kits\10\Lib\10.0.26100.0\ucrt\x64" -luser32
if %errorlevel% neq 0 exit /b %errorlevel%
test_core.exe
