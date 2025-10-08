clang -o test.exe test.c -I. -std=c17 -fsanitize=address -MJ compile_commands.json -ggdb -Wall -Wextra -Wpedantic -Wswitch -Wno-deprecated-declarations -luser32
