#ifndef MOVINGAVERAGE_H_ 
#define MOVINGAVERAGE_H_ 

class MovingAverage { 
	public: 
	#ifdef MOVINGAVERAGE_USE_DOUBLE typedef double real; 
	#else typedef float real; 
	#endif 

		// variable alfa dalam [0,1]. 
		float _alpha;

	real _value; 
	MovingAverage(float alphaOrN=0.1, real startValue=0); 
	void reset(real startValue); 
	void reset(real (*valueFunc)(void)); 
	real update(real v); 
	real get() const { return _value; } 
}; 

#endif /* MOVINGAVERAGE_H_ */