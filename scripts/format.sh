#!/bin/sh
# Formats every source file of the project with clang-format. Pass
# `--dry-run -Werror` to check instead of rewrite.
set -eu

cd "$(dirname "$0")/.."

find apps client core platform tests \
    \( -name '*.cpp' -o -name '*.hpp' -o -name '*.h' -o -name '*.c' \) -print0 |
    xargs -0 clang-format -i "$@"
