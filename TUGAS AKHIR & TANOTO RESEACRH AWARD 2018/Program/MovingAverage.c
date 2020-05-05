#if defined(ARDUINO) && ARDUINO >= 100 
#include "Arduino.h" 
#else #include "WProgram.h" 
#endif 

#include "MovingAverage.h" 

MovingAverage::MovingAverage(float alphaOrN, real startValue) :
_value(startValue) { 
	alphaOrN = max(alphaOrN, 0); // make sure alphaOrN >= 0 
	_alpha = (alphaOrN > 1 ? 
	2 / (alphaOrN + 1) : 
	alphaOrN); 
} 

void MovingAverage::reset(real startValue) { 
	_value = startValue; 
}
void MovingAverage::reset(real (*valueFunc)(void)) { 
 	// a = 2/(n+1) ==> n = 2/a - 1 ==> (n-1) / 2 = 1/a - 1 
 	int n = ceil( 1.0f/_alpha - 1); 
 	Serial.print("N: "); 
	 Serial.println(n); real avg = 0; 
 	for (int i=0; i<n; i++) 
 		avg += valueFunc(); 
 	avg /= n; reset(avg); 
} 

 MovingAverage::real MovingAverage::update(real v) { 
 	return (_value -= _alpha * (_value - v)); 
 }