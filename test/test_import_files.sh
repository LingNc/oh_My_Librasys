#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)

awk -F, 'NF != 5 { printf "Book.txt:%d: expected 5 fields, got %d\n", NR, NF; exit 1 }
    seen[$1]++ { printf "Book.txt:%d: duplicate ISBN %s\n", NR, $1; exit 1 }
    END { if (NR == 0) { print "Book.txt is empty"; exit 1 } }' "$root/Book.txt"

awk -F, 'NF != 4 { printf "Student.txt:%d: expected 4 fields, got %d\n", NR, NF; exit 1 }
    seen[$1]++ { printf "Student.txt:%d: duplicate student ID %s\n", NR, $1; exit 1 }
    END { if (NR == 0) { print "Student.txt is empty"; exit 1 } }' "$root/Student.txt"

printf 'Import file format tests passed.\n'