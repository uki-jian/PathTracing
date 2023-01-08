#pragma once
#include "super.h"

class Frame_counter {
public:
	unsigned int frame_counts;
	unsigned int buffer_size;
	clock_t time_buffer[10];
	float fps;

	Frame_counter() :frame_counts(0), buffer_size(10) {};
	~Frame_counter() {};
	void calculate() {
		cout << "Rendering Counts: " << ++frame_counts << " ";
		int pos = frame_counts % buffer_size;
		time_buffer[pos] = clock();
		if (frame_counts >= 10) {
			fps = (float)buffer_size / (time_buffer[pos] - time_buffer[(pos + 1) % buffer_size]) * CLOCKS_PER_SEC;
			cout << "FPS: " << fps << endl;
		}
		else cout << endl;
	}
};
