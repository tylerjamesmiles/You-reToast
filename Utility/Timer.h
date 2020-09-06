#pragma once

struct Timer
{
	float fTotalElapsedTime;
	float fFrameRate;
	int nTics;	// resets every time called
	int nFrame;	// total tics over time

	inline Timer()
		: fTotalElapsedTime(0.0f), fFrameRate(1.0f), nTics(0), nFrame(0) {}
	inline Timer(float frame_rate)
		: fTotalElapsedTime(0.0f), fFrameRate(frame_rate), nTics(0), nFrame(0) {}
	inline Timer(float offset, float frame_rate)
		: fTotalElapsedTime(offset), fFrameRate(frame_rate), nTics(0), nFrame(0) {}

	inline void Update(float fDelta) {
		nTics = 0;

		fTotalElapsedTime += fDelta;
		while (fTotalElapsedTime >= fFrameRate) {
			fTotalElapsedTime -= fFrameRate;
			nTics++;
			nFrame++;
		}
	}
};