
#include "util.h"
#include "sampler.h"


bool Sampler::sampleSquare() {
	// [0, 1] x [0, 1]
	if (finished) {
		return false;
	}
	for (int j = 0; j < 2; ++j) {
		sampled[j] = (RAND01() + i[j]) * n1;
	}
	if (++i[1] == n) {
		i[1] = 0;
		if (++i[0] == n) {
			i[0] = 0;
			finished = true;
		}
	}
	return true;
}

bool Sampler::sampleCube() {
	// [0, 1] x [0, 1] x [0, 1]
	if (finished) {
		return false;
	}
	for (int j = 0; j < 3; ++j) {
		sampled[j] = (RAND01() + i[j]) * n1;
	}
	if (++i[2] == n) {
		if (++i[1] == n) {
			i[1] = 0;
			if (++i[0] == n) {
				i[0] = 0;
				finished = true;
			}
		}
	}
	return true;
}

bool Sampler::sampleBall() {
	// unit ball, centre = origin
	while (!finished) {
		for (int j = 0; j < 3; ++j) {
			sampled[j] = 2.0 * (-0.5 + RAND01() * n1 + double(i[j]) * n1);
		}
		if (++i[2] == n) {
			i[2] = 0;
			if (++i[1] == n) {
				i[1] = 0;
				if (++i[0] == n) {
					i[0] = 0;
					finished = true;
				}
			}
		}
		if (sampled.asVec().length2() <= 1) {
			return true;
		}
	}
	return false;
}

bool Sampler::sampleDisk() {
	// unit disk on xy plane
	while (!finished) {
		for (int j = 0; j < 2; ++j) {
			sampled[j] = 2.0 * (-0.5 + RAND01() * n1 + double(i[j]) * n1);
		}
		if (++i[1] == n) {
			i[1] = 0;
			if (++i[0] == n) {
				i[0] = 0;
				finished = true;
			}
		}
		if (sampled[0] * sampled[0] + sampled[1] * sampled[1] <= 1) {
			return true;
		}
	}
	return false;
}

