#ifndef _FIFO_H
#define _FIFO_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define FIFO_PATH "/tmp/mpd.fifo"
#define FPS 25
#define SAMPLES 44100/FPS

static int fd = -1;

int16_t* update();
void fifo_open();

void fifo_open()
{
	if(fd < 0 && (fd = open(FIFO_PATH, O_RDONLY | O_NONBLOCK)) < 0)
		printf("Error opening '%s' (%s)", FIFO_PATH, strerror(errno));
}

//TODO separate file access and array gen, implement sound waves
int32_t *fifo_get_spectrum(int sample_size, int height)
{
	if(fd < 0)
		return NULL;

	int16_t buf[SAMPLES];

	ssize_t data = read(fd, buf, sizeof(buf));

	if(data < 0){
		return NULL;
	}

	const ssize_t samples_read = data/sizeof(int16_t);

	int32_t *spectrum = malloc(sizeof(int32_t) * sample_size);

	const int samples_per_col = samples_read/sample_size;

	for(int i=0; i < sample_size; ++i) {
		*(spectrum+i) = 0.0;
		for(int j=0; j < samples_per_col; ++j) {
			*(spectrum+i) += buf[i * samples_per_col + j];
		}
		*(spectrum+i) /= samples_per_col;
		*(spectrum+i) *= height / 65536.0;
	}

	return spectrum;
}

#endif
