clang -o test.exe test.c -I. -std=c17 -MJ compile_commands.json -ggdb -Wall -Wextra -Werror -Wpedantic -Wswitch -Wno-deprecated-declarations -luser32
