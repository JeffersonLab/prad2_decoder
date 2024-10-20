# prad2_decoder

This is a decoder to decode the data files written in evio format.

It is adjusted to adapt the Hall B DAQ system and data format.

Currently, the JLab FADC250 is the only module included in the decoder.

To run it
```
> cmake -B build -S . [-DCMAKE_INSTALL_PREFIX=<path_to_install>]
> cmake --build build
> ./build/src/prad2_decoder <some_evio_file>
```

