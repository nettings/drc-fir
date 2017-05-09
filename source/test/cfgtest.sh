# General DRC configuration testing.
# This file need the DRC executable to be either in the
# current path or in the current directory.
# It also needs a dirac.pcm file with a Dirac delta few seconds long in it.

# 44.1 kHz

./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/44.1 kHz/flat-44.1.txt" --PSPointsFile="../target/44.1 kHz/flat-44.1.txt" --PSOutFile=dxp-erb-44.1.pcm --MSOutFile=dmp-erb-44.1.pcm --TCOutFile=dtc-erb-44.1.pcm "../config/44.1 kHz/erb-44.1.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/44.1 kHz/flat-44.1.txt" --PSPointsFile="../target/44.1 kHz/flat-44.1.txt" --PSOutFile=dxp-minimal-44.1.pcm --MSOutFile=dmp-minimal-44.1.pcm --TCOutFile=dtc-minimal-44.1.pcm "../config/44.1 kHz/minimal-44.1.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/44.1 kHz/flat-44.1.txt" --PSPointsFile="../target/44.1 kHz/flat-44.1.txt" --PSOutFile=dxp-soft-44.1.pcm --MSOutFile=dmp-soft-44.1.pcm --TCOutFile=dtc-soft-44.1.pcm "../config/44.1 kHz/soft-44.1.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/44.1 kHz/flat-44.1.txt" --PSPointsFile="../target/44.1 kHz/flat-44.1.txt" --PSOutFile=dxp-normal-44.1.pcm --MSOutFile=dmp-normal-44.1.pcm --TCOutFile=dtc-normal-44.1.pcm "../config/44.1 kHz/normal-44.1.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/44.1 kHz/flat-44.1.txt" --PSPointsFile="../target/44.1 kHz/flat-44.1.txt" --PSOutFile=dxp-strong-44.1.pcm --MSOutFile=dmp-strong-44.1.pcm --TCOutFile=dtc-strong-44.1.pcm "../config/44.1 kHz/strong-44.1.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/44.1 kHz/flat-44.1.txt" --PSPointsFile="../target/44.1 kHz/flat-44.1.txt" --PSOutFile=dxp-extreme-44.1.pcm --MSOutFile=dmp-extreme-44.1.pcm --TCOutFile=dtc-extreme-44.1.pcm "../config/44.1 kHz/extreme-44.1.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/44.1 kHz/flat-44.1.txt" --PSPointsFile="../target/44.1 kHz/flat-44.1.txt" --PSOutFile=dxp-insane-44.1.pcm --MSOutFile=dmp-insane-44.1.pcm --TCOutFile=dtc-insane-44.1.pcm "../config/44.1 kHz/insane-44.1.drc"

# 48.0 kHz

./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/48.0 kHz/flat-48.0.txt" --PSPointsFile="../target/48.0 kHz/flat-48.0.txt" --PSOutFile=dxp-erb-48.0.pcm --MSOutFile=dmp-erb-48.0.pcm --TCOutFile=dtc-erb-48.0.pcm "../config/48.0 kHz/erb-48.0.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/48.0 kHz/flat-48.0.txt" --PSPointsFile="../target/48.0 kHz/flat-48.0.txt" --PSOutFile=dxp-minimal-48.0.pcm --MSOutFile=dmp-minimal-48.0.pcm --TCOutFile=dtc-minimal-48.0.pcm "../config/48.0 kHz/minimal-48.0.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/48.0 kHz/flat-48.0.txt" --PSPointsFile="../target/48.0 kHz/flat-48.0.txt" --PSOutFile=dxp-soft-48.0.pcm --MSOutFile=dmp-soft-48.0.pcm --TCOutFile=dtc-soft-48.0.pcm "../config/48.0 kHz/soft-48.0.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/48.0 kHz/flat-48.0.txt" --PSPointsFile="../target/48.0 kHz/flat-48.0.txt" --PSOutFile=dxp-normal-48.0.pcm --MSOutFile=dmp-normal-48.0.pcm --TCOutFile=dtc-normal-48.0.pcm "../config/48.0 kHz/normal-48.0.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/48.0 kHz/flat-48.0.txt" --PSPointsFile="../target/48.0 kHz/flat-48.0.txt" --PSOutFile=dxp-strong-48.0.pcm --MSOutFile=dmp-strong-48.0.pcm --TCOutFile=dtc-strong-48.0.pcm "../config/48.0 kHz/strong-48.0.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/48.0 kHz/flat-48.0.txt" --PSPointsFile="../target/48.0 kHz/flat-48.0.txt" --PSOutFile=dxp-extreme-48.0.pcm --MSOutFile=dmp-extreme-48.0.pcm --TCOutFile=dtc-extreme-48.0.pcm "../config/48.0 kHz/extreme-48.0.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/48.0 kHz/flat-48.0.txt" --PSPointsFile="../target/48.0 kHz/flat-48.0.txt" --PSOutFile=dxp-insane-48.0.pcm --MSOutFile=dmp-insane-48.0.pcm --TCOutFile=dtc-insane-48.0.pcm "../config/48.0 kHz/insane-48.0.drc"

# 88.2 kHz

./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/88.2 kHz/flat-88.2.txt" --PSPointsFile="../target/88.2 kHz/flat-88.2.txt" --PSOutFile=dxp-erb-88.2.pcm --MSOutFile=dmp-erb-88.2.pcm --TCOutFile=dtc-erb-88.2.pcm "../config/88.2 kHz/erb-88.2.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/88.2 kHz/flat-88.2.txt" --PSPointsFile="../target/88.2 kHz/flat-88.2.txt" --PSOutFile=dxp-minimal-88.2.pcm --MSOutFile=dmp-minimal-88.2.pcm --TCOutFile=dtc-minimal-88.2.pcm "../config/88.2 kHz/minimal-88.2.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/88.2 kHz/flat-88.2.txt" --PSPointsFile="../target/88.2 kHz/flat-88.2.txt" --PSOutFile=dxp-soft-88.2.pcm --MSOutFile=dmp-soft-88.2.pcm --TCOutFile=dtc-soft-88.2.pcm "../config/88.2 kHz/soft-88.2.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/88.2 kHz/flat-88.2.txt" --PSPointsFile="../target/88.2 kHz/flat-88.2.txt" --PSOutFile=dxp-normal-88.2.pcm --MSOutFile=dmp-normal-88.2.pcm --TCOutFile=dtc-normal-88.2.pcm "../config/88.2 kHz/normal-88.2.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/88.2 kHz/flat-88.2.txt" --PSPointsFile="../target/88.2 kHz/flat-88.2.txt" --PSOutFile=dxp-strong-88.2.pcm --MSOutFile=dmp-strong-88.2.pcm --TCOutFile=dtc-strong-88.2.pcm "../config/88.2 kHz/strong-88.2.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/88.2 kHz/flat-88.2.txt" --PSPointsFile="../target/88.2 kHz/flat-88.2.txt" --PSOutFile=dxp-extreme-88.2.pcm --MSOutFile=dmp-extreme-88.2.pcm --TCOutFile=dtc-extreme-88.2.pcm "../config/88.2 kHz/extreme-88.2.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/88.2 kHz/flat-88.2.txt" --PSPointsFile="../target/88.2 kHz/flat-88.2.txt" --PSOutFile=dxp-insane-88.2.pcm --MSOutFile=dmp-insane-88.2.pcm --TCOutFile=dtc-insane-88.2.pcm "../config/88.2 kHz/insane-88.2.drc"

# 96.0 kHz

./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/96.0 kHz/flat-96.0.txt" --PSPointsFile="../target/96.0 kHz/flat-96.0.txt" --PSOutFile=dxp-erb-96.0.pcm --MSOutFile=dmp-erb-96.0.pcm --TCOutFile=dtc-erb-96.0.pcm "../config/96.0 kHz/erb-96.0.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/96.0 kHz/flat-96.0.txt" --PSPointsFile="../target/96.0 kHz/flat-96.0.txt" --PSOutFile=dxp-minimal-96.0.pcm --MSOutFile=dmp-minimal-96.0.pcm --TCOutFile=dtc-minimal-96.0.pcm "../config/96.0 kHz/minimal-96.0.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/96.0 kHz/flat-96.0.txt" --PSPointsFile="../target/96.0 kHz/flat-96.0.txt" --PSOutFile=dxp-soft-96.0.pcm --MSOutFile=dmp-soft-96.0.pcm --TCOutFile=dtc-soft-96.0.pcm "../config/96.0 kHz/soft-96.0.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/96.0 kHz/flat-96.0.txt" --PSPointsFile="../target/96.0 kHz/flat-96.0.txt" --PSOutFile=dxp-normal-96.0.pcm --MSOutFile=dmp-normal-96.0.pcm --TCOutFile=dtc-normal-96.0.pcm "../config/96.0 kHz/normal-96.0.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/96.0 kHz/flat-96.0.txt" --PSPointsFile="../target/96.0 kHz/flat-96.0.txt" --PSOutFile=dxp-strong-96.0.pcm --MSOutFile=dmp-strong-96.0.pcm --TCOutFile=dtc-strong-96.0.pcm "../config/96.0 kHz/strong-96.0.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/96.0 kHz/flat-96.0.txt" --PSPointsFile="../target/96.0 kHz/flat-96.0.txt" --PSOutFile=dxp-extreme-96.0.pcm --MSOutFile=dmp-extreme-96.0.pcm --TCOutFile=dtc-extreme-96.0.pcm "../config/96.0 kHz/extreme-96.0.drc"
./drc --BCInFile=dirac.pcm --MCFilterType=M --MCPointsFile="../target/96.0 kHz/flat-96.0.txt" --PSPointsFile="../target/96.0 kHz/flat-96.0.txt" --PSOutFile=dxp-insane-96.0.pcm --MSOutFile=dmp-insane-96.0.pcm --TCOutFile=dtc-insane-96.0.pcm "../config/96.0 kHz/insane-96.0.drc"