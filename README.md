Deconspectrum
=============

Deconspectrum is a collection of sound-responsive cubes whose color represents parts of the aural spectrum around them.

Full installation description [here](http://log.liminastudio.com/projects/deconspectrum).

Individual units consist of the PCB in the /hardware directory (Eagle format) and the ATmega/Arduino software in the /software directory.

The software uses the [PlainFFT library by Didier Longueville](http://www.arduinoos.com/2010/10/fast-fourier-transform-fft/).  I have included the version of the library I used for completeness and accuracy; it is slightly modified to optimize compiled object size.
