#!/bin/bash
cmake --build build && ctest --test-dir build -R "^test_" --output-on-failure