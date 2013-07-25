/*
FFT libray
No warranty, no claims, just fun
Didier Longueville invenit et fecit October 2010
*/

#ifndef PlainFFT_h // Prevent loading library twice
#define PlainFFT_h

#include "WProgram.h" // This is where the standard Arduino code lies

#define FFT_LIB_REV 0x01
// Custom constants
#define FFT_FORWARD 0x01
#define FFT_REVERSE 0x00
// Windowing type
#define FFT_WIN_TYP_RECTANGLE 0x00 // rectangle (Box car)
#define FFT_WIN_TYP_HAMMING 0x01 // hamming
#define FFT_WIN_TYP_HANN 0x02 // hann
#define FFT_WIN_TYP_TRIANGLE 0x03 // triangle (Bartlett)
#define FFT_WIN_TYP_BLACKMAN 0x04 // blackmann
#define FFT_WIN_TYP_FLT_TOP 0x05 // flat top
#define FFT_WIN_TYP_WELCH 0x06 // welch
	
class PlainFFT {
	public:
		// Constructor
		PlainFFT(void);
		// Destructor
		~PlainFFT(void);
		// Functions
		void complexToMagnitude(double *vReal, double *vImag, uint16_t samples);
		void compute(double *vReal, double *vImag, uint16_t samples, uint8_t dir);
		double majorPeak(double *vD, uint16_t samples, double samplingFrequency);
		void printMagnitudes(double *vM, uint16_t samples);
		uint8_t revision(void);
		void windowing(double *vData, uint16_t samples); 
	
	private:
		// Functions
		uint8_t exponent(uint16_t value);
		void swap(double *x, double *y);
	
};

#endif
