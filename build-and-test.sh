#!/bin/bash

make clean
make
# Generate a 3-read, 2-write port generic register file.
./mprfgen -infer -nwp 2 -nrp 3 regfile_w2r3.vhd
# Generate a 1-read, 1-write port 32x2048 memory.
./mprfgen -infer -read-first -nwp 1 -nrp 1 -bw 32 -nregs 2048 mymem.vhd
# Generate a 2-read, 1-write port LUT-based register file.
./mprfgen -read-async luts_w1r2.vhd
# Generate a 2-read, 1-write port block RAM register file with direct 
# instantiation.
./mprfgen -read-first bram_w1r2.vhd

if [ "$SECONDS" -eq 1 ]
then
  units=second
else
  units=seconds
fi
echo "This script has been running for $SECONDS $units."
exit 0
