#include "util.h"

#ifndef _SAMPLER_
#define _SAMPLER_

enum SampleType {
	SAMPLE_SQUARE,
	SAMPLE_DISK,
	SAMPLE_BALL,
	SAMPLE_CUBE
};

class Sampler {
public:
	void beginSampling(SampleType st, int density) {
		n = density;
		n1 = 1.0 / n;
		finished = false;
		for (int j = 0; j < 3; ++j) {
			i[j] = 0;
		}
		switch (st) {
		case SAMPLE_SQUARE:
			currentSample = &Sampler::sampleSquare;
			sampled[2] = 0;
			break;
		case SAMPLE_DISK:
			currentSample = &Sampler::sampleDisk;
			sampled[2] = 0;
			break;
		case SAMPLE_BALL:
			currentSample = &Sampler::sampleBall;
			break;
		case SAMPLE_CUBE:
			currentSample = &Sampler::sampleCube;
			break;
		}
		return;
	}
	bool sample() {
		return (this->*currentSample)();
	}
	Point3D getSampled() {
		return sampled;
	}
private:
	bool sampleSquare();
	bool sampleCube();
	bool sampleDisk();
	bool sampleBall();

	bool (Sampler::*currentSample)();
	int n;
	double n1;
	int i[3];
	Point3D sampled;
	bool finished;
};

#endif