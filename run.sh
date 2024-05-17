#!/bin/bash

cd build/

# 运行 cmake
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make
sudo make install
sudo ldconfig

sudo chmod +x /usr/bin/stackcollapse-perf.pl /usr/bin/flamegraph.pl /usr/bin/difffolded.pl 

