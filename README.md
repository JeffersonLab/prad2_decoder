# prad2_decoder

This is a decoder to decode the data files written in evio.
It is adjusted to adapt Hall B DAQ system and the evio formats.

To run it, try
`cmake -B build -S . -DCMAKE_INSTALL_PREFIX=<some>`
`cmake --build build`
'./build/src/prad2_decoder <some_evio_file>'
