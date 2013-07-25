#include "PlainFFT.h"

#define LED_RED			9
#define LED_GREEN		10
#define LED_BLUE		11
#define MIC_PIN         0
#define ADJUST_PIN      1
#define MODE_PIN		2

#define NUM_SAMPLES		64
#define FREQ_MIN		200
#define FREQ_MAX		2000
#define AMP_MIN			50
#define MAGNITUDE_MAX	12		// ??
// Bandwidth is determined by NUM_SAMPLES
//#define BANDWIDTH		12		// in hue; width of steps for Tuned mode
#define CURVE_FACTOR            1.5             // controls the exponential compensation

PlainFFT FFT = PlainFFT(); // Create FFT object

int i = 0;
int samplingFrequency = 7500;
double vReal[NUM_SAMPLES];
double vImag[NUM_SAMPLES];

unsigned long lastMicros;
int sampleDur;

int minSample = 1023;
int maxSample = 0;
int amp = 0;
float hueScale = 95.3;

/* 
  dim_curve 'lookup table' to compensate for the nonlinearity of human vision.
  Used in the getRGB function on saturation and brightness to make 'dimming' look more natural. 
  Exponential function used to create values below : 
  x from 0 - 255 : y = round(pow( 2.0, x+64/40.0) - 1)   
*/
const byte dim_curve[] = {
    0,   1,   1,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,
    3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,   4,   4,   4,   4,
    4,   4,   4,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   6,   6,   6,
    6,   6,   6,   6,   6,   7,   7,   7,   7,   7,   7,   7,   8,   8,   8,   8,
    8,   8,   9,   9,   9,   9,   9,   9,   10,  10,  10,  10,  10,  11,  11,  11,
    11,  11,  12,  12,  12,  12,  12,  13,  13,  13,  13,  14,  14,  14,  14,  15,
    15,  15,  16,  16,  16,  16,  17,  17,  17,  18,  18,  18,  19,  19,  19,  20,
    20,  20,  21,  21,  22,  22,  22,  23,  23,  24,  24,  25,  25,  25,  26,  26,
    27,  27,  28,  28,  29,  29,  30,  30,  31,  32,  32,  33,  33,  34,  35,  35,
    36,  36,  37,  38,  38,  39,  40,  40,  41,  42,  43,  43,  44,  45,  46,  47,
    48,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,
    63,  64,  65,  66,  68,  69,  70,  71,  73,  74,  75,  76,  78,  79,  81,  82,
    83,  85,  86,  88,  90,  91,  93,  94,  96,  98,  99,  101, 103, 105, 107, 109,
    110, 112, 114, 116, 118, 121, 123, 125, 127, 129, 132, 134, 136, 139, 141, 144,
    146, 149, 151, 154, 157, 159, 162, 165, 168, 171, 174, 177, 180, 183, 186, 190,
    193, 196, 200, 203, 207, 211, 214, 218, 222, 226, 230, 234, 238, 242, 248, 255,
};

void setup(){
//        Serial.begin(9600);
	///// LED SETUP /////
	pinMode(LED_RED, OUTPUT);
	pinMode(LED_GREEN, OUTPUT);
	pinMode(LED_BLUE, OUTPUT);
	
	pinMode(MODE_PIN, INPUT);           // set pin to input
	digitalWrite(MODE_PIN, HIGH);       // turn on pullup resistor
	
	//hueScale = 360 / log10(FREQ_MAX - FREQ_MIN);
	hueScale = 360 / (pow((FREQ_MAX - FREQ_MIN), (1/CURVE_FACTOR)));
        
	POST();
}

void loop() {
  if(i < NUM_SAMPLES){
    vReal[i] = analogRead(MIC_PIN);
    if(vReal[i] < minSample)
      minSample = vReal[i];
    if(vReal[i] > maxSample)
      maxSample = vReal[i];
   // sampleDur = micros() - lastMicros;
    //lastMicros = micros();
    i++;
  } else {
    
    // process the sample buffer
    FFT.windowing(vReal, NUM_SAMPLES);	// Weigh data
    FFT.compute(vReal, vImag, NUM_SAMPLES, FFT_FORWARD); // Compute FFT
    FFT.complexToMagnitude(vReal, vImag, NUM_SAMPLES); // Compute magnitudes
    //samplingFrequency = 1000000 / sampleDur;
    
//        Serial.print("x:\t");
//	Serial.print(x);
//        Serial.print("\thue:\t");
//        Serial.print(hueFreq);
//        Serial.print("\tsampFreq:\t");
//        Serial.print(samplingFrequency);
        
	if(digitalRead(MODE_PIN) == HIGH){
		// Tuned Mode switch turned OFF: Color to current peak; disregard frequency selection
		double x = FFT.majorPeak(vReal, NUM_SAMPLES, samplingFrequency);
		// Converts logarithmic input (acoustic frequency) to linear output (hue)
		//int hueFreq = log10(x - FREQ_MIN) * hueScale;
		int hueFreq = pow(x - FREQ_MIN, (1/CURVE_FACTOR)) * hueScale;
		hueFreq = constrain(hueFreq, 0, 359);
		int amp = maxSample - minSample;
		int ampNormal = map(amp, AMP_MIN, 1023, 0, 255);
		ampNormal = constrain(ampNormal, 0, 255);
		
		setLED(hueFreq, ampNormal);
	} else {
		// Tuned Mode switch turned ON: Only color to adjust selection
		int pot = analogRead(ADJUST_PIN);
		
		// Serial.print("\tpot:\t");
		// Serial.print(pot);
		
		// Given 64 samples, there are 32 spectrum bands
		// however, we must discard 0-3 and 28-31 as out of range, leaving 24 bands :-/
		pot = map(pot, 0, 1023, 0, 23);
		// Serial.print("\tpotMap:\t");
		// Serial.print(pot);
		int hueFreq = map(pot, 0, 23, 0, 359);
		
		float mag = vReal[pot+3];	// ignore indices 0-2 (freq 0 = meaningless; others are too low)
		mag = map(mag, 0, MAGNITUDE_MAX, 0, 255);
		
		setLED(hueFreq, mag);
	}
//	Serial.println();
    // reset
    //lastMicros = micros();
    i = 0;
    minSample = 1023;
    maxSample = 0;
  }
}

void POST(){
	ledWrite(255,0,0);
	delay(300);
	ledWrite(0,255,0);
	delay(300);
	ledWrite(0,0,255);
	delay(300);
	ledWrite(0, 0, 0);
}

void setLED(int hue, int l){
	int col[3] = {0,0,0};
	getRGB(hue, 255, l, col);
	ledWrite(col[0], col[1], col[2]);
}

void getRGB(int hue, int sat, int val, int colors[3]) {
	/*  
	  Control a RGB led with Hue, Saturation and Brightness (HSB / HSV )

	  getRGB() function based on <http://www.codeproject.com/miscctrl/CPicker.asp>  
	  dim_curve idea by Jims

	  created 05-01-2010 by kasperkamperman.com
	*/
	
	// hue: 0-259, sat: 0-255, val (lightness): 0-255
	int r, g, b, base;
	
	val = dim_curve[val];
	//sat = 255-dim_curve[255-sat];
	
	if (sat == 0) { // Achromatic color (gray); hue unaffected
		colors[0]=val;
		colors[1]=val;
		colors[2]=val;
	} else  { 
		base = ((255 - sat) * val)>>8;
		switch(hue/60) {
			case 0:
				r = val;
				g = (((val-base)*hue)/60)+base;
				b = base;
				break;
			case 1:
				r = (((val-base)*(60-(hue%60)))/60)+base;
				g = val;
				b = base;
				break;
			case 2:
				r = base;
				g = val;
				b = (((val-base)*(hue%60))/60)+base;
				break;
			case 3:
				r = base;
				g = (((val-base)*(60-(hue%60)))/60)+base;
				b = val;
				break;
			case 4:
				r = (((val-base)*(hue%60))/60)+base;
				g = base;
				b = val;
				break;
			case 5:
				r = val;
				g = base;
				b = (((val-base)*(60-(hue%60)))/60)+base;
				break;
		}
		colors[0]=r;
		colors[1]=g;
		colors[2]=b;
	}   
}

void ledWrite(int r, int g, int b){
	analogWrite(LED_RED, 255-r);
	analogWrite(LED_GREEN, 255-g);
	analogWrite(LED_BLUE, 255-b);
}
