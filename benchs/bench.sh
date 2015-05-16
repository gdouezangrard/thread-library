#!/bin/bash

set -eu

cd benchs/datfiles
rm *.dat || true

cd ../fill
./fill-create-many.sh
./fill-create-many-once.sh
./fill-create-many-recursive.sh
./fill-fibonacci.sh
./fill-switch-many.sh
./fill-switch-many2.sh
./fill-switch-many-join.sh
./fill-switch-many-join2.sh

cd ..
./plotall.sh
