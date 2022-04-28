#! /bin/bash

set -euo pipefail

main() {
    cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release -H. -Bbuild
    cmake --build build -j
}

main
