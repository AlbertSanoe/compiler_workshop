#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./read "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 21 "5+20-4"
assert 41 " 12 + 34 - 5 "
assert 103 " 1+3 *34"
assert 1696 "(34+7)*41+(3*5)"
echo OK