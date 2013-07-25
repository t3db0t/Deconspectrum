/*
PlainFFT Library: example of use
Input vectors receive computed results from FFT
No warranty, no claims, just fun
Didier Longueville invenit et fecit October 2010
*/
#include "PlainFFT.h"

PlainFFT FFT = PlainFFT(); // Create FFT object
// These values can be changed in order to evaluate the functions
const uint16_t samples = 64;
double signalFrequency = 1000;
double samplingFrequency = 5000;
uint8_t signalIntensity = 100;
// These are input and output vectors
double vReal[samples]; 
double vImag[samples];
uint8_t runOnce = 0x00;

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02

void setup(){
	Serial.begin(115200);
	Serial.println("Ready");
}

void loop() {
	if (runOnce == 0x00) {
		runOnce = 0x01;
		// Build raw data
		double cycles = (((samples-1) * signalFrequency) / samplingFrequency);
		for (uint8_t i = 0; i < samples; i++) {
			vReal[i] = uint8_t((signalIntensity * (sin((i * (6.2831 * cycles)) / samples) + 1.0)) / 2.0);
		}
		printVector(vReal, samples, SCL_TIME);
		FFT.windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);	// Weigh data
		printVector(vReal, samples, SCL_TIME);
		FFT.compute(vReal, vImag, samples, FFT_FORWARD); // Compute FFT
		printVector(vReal, samples, SCL_INDEX);
		printVector(vImag, samples, SCL_INDEX);
		FFT.complexToMagnitude(vReal, vImag, samples); // Compute magnitudes
		printVector(vReal, (samples >> 1), SCL_FREQUENCY);	
		double x = FFT.majorPeak(vReal, samples, samplingFrequency);
		Serial.println(x, 6);
	}
}

void printVector(double *vD, uint8_t n, uint8_t scaleType) {
	double timeInterval = (1.0 / samplingFrequency);
	for (uint16_t i = 0; i < n; i++) {
		// Print abscissa value
		switch (scaleType) {
		case SCL_INDEX:
			Serial.print(i, DEC);
			break;
		case SCL_TIME:
			Serial.print((i * timeInterval), 6);
			break;
		case SCL_FREQUENCY:
			Serial.print((i / (timeInterval * (samples-1))), 6);
			break;
		}
		Serial.print(" ");
		// Print ordinate value
		Serial.print(vD[i], 6);
		Serial.println();
	}
	Serial.println();
}