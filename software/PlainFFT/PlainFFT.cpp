/*
PlainFFT Library
No warranty, no claims, just fun
Didier Longueville invenit et fecit October 2010

Modified by Ted Hayes 2011 to use only Hamming windowing to optimize compiled object size.
*/

#include "PlainFFT.h"

#define twoPi 6.28318531
#define fourPi 12.56637061

PlainFFT::PlainFFT(void) {
// Constructor
}

PlainFFT::~PlainFFT(void){ 
// Destructor
}

uint8_t PlainFFT::revision(void){ 
	return(FFT_LIB_REV);
}

void PlainFFT::compute(double *vReal, double *vImag, uint16_t samples, uint8_t dir) {
// Computes in-place complex-to-complex FFT 
	// Reverse bits
	uint16_t j = 0;
	for (uint16_t i = 0; i < (samples - 1); i++) {
		if (i < j) {
			 swap(&vReal[i], &vReal[j]);
			 swap(&vImag[i], &vImag[j]);
		}
		uint16_t k = (samples >> 1);
		while (k <= j) {
			 j -= k;
			 k >>= 1;
		}
		j += k;
	}
	// Compute the FFT 
	double c1 = -1.0; 
	double c2 = 0.0;
	uint8_t l2 = 1;
	for (uint8_t l = 0; l < exponent(samples); l++) {
		uint8_t l1 = l2;
		l2 <<= 1;
		double u1 = 1.0; 
		double u2 = 0.0;
		for (j = 0; j < l1; j++) {
			 for (uint16_t i = j; i < samples; i += l2) {
					uint16_t i1 = i + l1;
					double t1 = u1 * vReal[i1] - u2 * vImag[i1];
					double t2 = u1 * vImag[i1] + u2 * vReal[i1];
					vReal[i1] = vReal[i] - t1; 
					vImag[i1] = vImag[i] - t2;
					vReal[i] += t1;
					vImag[i] += t2;
			 }
			 double z = (u1 * c1) - (u2 * c2);
			 u2 = (u1 * c2) + (u2 * c1);
			 u1 = z;
		}
		c2 = sqrt((1.0 - c1) / 2.0); 
		if (dir == FFT_FORWARD) c2 = -c2;
		c1 = sqrt((1.0 + c1) / 2.0);
	}
	// Scaling for forward transform
	if (dir == FFT_FORWARD) {
		for (uint16_t i = 0; i < samples; i++) {
			 vReal[i] /= samples;
			 vImag[i] /= samples;
		}
	}
}

void PlainFFT::complexToMagnitude(double *vReal, double *vImag, uint16_t samples) {
// vM is half the size of vReal and vImag
	for (uint8_t i = 0; i < samples; i++) vReal[i] = sqrt(sq(vReal[i]) + sq(vImag[i]));
}

void PlainFFT::windowing(double *vData, uint16_t samples) {
// Weighing factors are computed once before multiple use of FFT
// The weighing function is symetric; half the weighs are recorded
	double samplesMinusOne = (double(samples) - 1.0);
	for (uint16_t i = 0; i < (samples >> 1); i++) {
		double indexMinusOne = double(i);
		double ratio = (indexMinusOne / samplesMinusOne);
		double weighingFactor = 1.0;
		// compute and record weighting factor
		weighingFactor = 0.54 - (0.46 * cos(twoPi * ratio));
		vData[i] *= weighingFactor;
		vData[samples - (i + 1)] *= weighingFactor;
	}
}

double PlainFFT::majorPeak(double *vD, uint16_t samples, double samplingFrequency) {
	double maxY = 0;
	uint16_t IndexOfMaxY = 0;
	for (uint16_t i = 1; i < ((samples >> 1) - 1); i++) {
		if ((vD[i-1] < vD[i]) && (vD[i] > vD[i+1])) {
			if (vD[i] > maxY) {
				maxY = vD[i];
				IndexOfMaxY = i;
			}
		}
	}
	double delta = 0.5 * ((vD[IndexOfMaxY-1] - vD[IndexOfMaxY+1]) / (vD[IndexOfMaxY-1] - (2.0 * vD[IndexOfMaxY]) + vD[IndexOfMaxY+1]));
	double interpolatedX = ((IndexOfMaxY + delta)  * samplingFrequency) / (samples-1);
	// retuned value: interpolated frequency peak apex
	return(interpolatedX);
}

void PlainFFT::printMagnitudes(double *vM, uint16_t samples){
	for (uint16_t i = 1; i < ((samples >> 1) - 1); i++) {
		Serial.print(vM[i]);
		Serial.print("\t");
	}
}

/* Private */

void PlainFFT::swap(double *x, double *y) {
	double temp = *x;
	*x = *y;
	*y = temp;
}

uint8_t PlainFFT::exponent(uint16_t value) {
	// computes the exponent of a powered 2 value
	uint8_t result = 0;
	while (((value >> result) & 1) != 1) result++;
	return(result);
}