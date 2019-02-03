#!/bin/bash
#
# Recursively re-formats *.c and *.h using clang-format, and WebKit style.
# 
find ./ -iname *.h -o -iname *.c | xargs clang-format -i -style=file

