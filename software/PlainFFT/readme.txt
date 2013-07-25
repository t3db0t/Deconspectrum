This is a C++ library for Arduino for computing FFT.
Tested on Arduino 0021 Alpha.

*** Modified by Ted Hayes 2011 to use only Hamming windowing to optimize compiled object size. ***

Installation
--------------------------------------------------------------------------------

To install this library, just place this entire folder as a subfolder in your


When installed, this library should look like:

Arduino\libraries\PlainFTT              			(this library's folder)
Arduino\libraries\PlainFTT\PlainFTT.cpp 			(the library implementation file)
Arduino\libraries\PlainFTT\PlainFTT.h   			(the library description file)
Arduino\libraries\PlainFTT\keywords.txt 			(the syntax coloring file)
Arduino\libraries\PlainFTT\examples     			(the examples in the "open" menu)
Arduino\libraries\PlainFTT\readme.txt   			(this file)

Building
--------------------------------------------------------------------------------

After this library is installed, you just have to start the Arduino application.
You may see a few warning messages as it's built.

To use this library in a sketch, go to the Sketch | Import Library menu and
select PlainFTT.  This will add a corresponding line to the top of your sketch:
#include <PlainFTT.h>

