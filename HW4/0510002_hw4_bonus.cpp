// Student ID: 0510002
// Name      : YUAN YU-HSUN
// Date      : 2018/12/22
#include "bmpReader.h"
#include "bmpReader.cpp"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
using namespace std;



#define MYRED	2
#define MYGREEN 1
#define MYBLUE	0



int imgWidth, imgHeight;
int MEAN_FILTER_SIZE = 9;
int SOBEL_FILTER_SIZE;
int FILTER_SCALE;
int *filter_gx, *filter_gy;
int wsize = (int)sqrt((float)MEAN_FILTER_SIZE);
int halfwsize = wsize / 2;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition  = PTHREAD_COND_INITIALIZER;



const char *inputfile_name[5] = {
	"input1.bmp",
	"input2.bmp",
	"input3.bmp",
	"input4.bmp",
	"input5.bmp"
};
const char *outputMed_name[5] = {
	"output1.bmp",
	"output2.bmp",
	"output3.bmp",
	"output4.bmp",
	"output5.bmp"
};



unsigned char *pic_in, *pic_grey, *pic_mean, *pic_final;



unsigned char RGB2grey(int index) {
	int tmp = (
		pic_in[index + MYRED] +
		pic_in[index + MYGREEN] +
		pic_in[index + MYBLUE] )/3;

	if (tmp < 0) tmp = 0;
	else if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}



unsigned char MeanFilter(int w, int h) {
	int sum = 0, a, b;
	for (int j = 0; j < wsize; j++)
		for (int i = 0; i < wsize; i++)
		{
			a = w + i - halfwsize;
			b = h + j - halfwsize;

			// detect for borders of the image
			if (a < 0) continue;
			else if (b < 0) continue;
			else if (a >= imgWidth) continue;
			else if (b >= imgHeight) continue;

			sum += pic_grey[b*imgWidth + a];
		}

	int tmp = sum / MEAN_FILTER_SIZE;
	
	if (tmp < 0) tmp = 0;
	else if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}



unsigned char sobelFilter(int w, int h) {
	int tmpx = 0, tmpy = 0, a, b, index1, index2;
	for (int j = 0; j < wsize; j++)
		for (int i = 0; i < wsize; i++)
		{
			a = w + i - halfwsize;
			b = h + j - halfwsize;

			// detect for borders of the image
			if (a < 0) continue;
			else if (b < 0) continue;
			else if (a >= imgWidth) continue;
			else if (b >= imgHeight) continue;

			index1 = j*wsize + i;
			index2 = b*imgWidth + a;
			tmpx += filter_gx[index1] * pic_mean[index2];
			tmpy += filter_gy[index1] * pic_mean[index2];
		}

	if (tmpx < 0) tmpx = 0;
	else if (tmpx > 255) tmpx = 255;
	if (tmpy < 0) tmpy = 0;
	else if (tmpy > 255) tmpy = 255;

	int tmp = sqrt(tmpx*tmpx + tmpy*tmpy);
	if (tmp < 0) tmp = 0;
	else if (tmp > 255) tmp = 255;

	return (unsigned char)tmp;
}



void* child_RGB2grey(void* data) {
	pthread_mutex_lock(&mutex);
	pthread_mutex_unlock(&mutex);
	//convert RGB image to grey image
	int index;
	bool start = false;
	for (int j = 0; j < imgHeight; j++) {
		for (int i = 0; i < imgWidth; i++){
			index = j*imgWidth + i;
			pic_grey[index] = RGB2grey(3 * index);

			if (start) ;
			else if (!start) // row = 2
				if (j == 4) {
					start = true;
					pthread_cond_signal(&condition);
				}
		}
	}

	pthread_exit(NULL);
}



void* child_MeanFilter(void* data) {
	//apply the Mean filter to the image
	bool start = false;
	for (int j = 0; j < imgHeight; j++) {
		for (int i = 0; i < imgWidth; i++){
			pic_mean[j*imgWidth + i] = MeanFilter(i, j);

			if (start) ;
			else if (!start) // row = 1, col = 2
				if (j == 4) {
					start = true;
					pthread_cond_signal(&condition);
				}
		}
	}

	pthread_exit(NULL);
}



void * child_SobelFilter(void* data) {
	//apply the sobel filter to the image and extend the size
	unsigned char tmp;
	int index;
	for (int j = 0; j < imgHeight; j++) {
		for (int i = 0; i < imgWidth; i++){
			tmp = sobelFilter(i, j);
			index = 3 * (j*imgWidth + i);
			pic_final[index + MYRED] = tmp;
			pic_final[index + MYGREEN] = tmp;
			pic_final[index + MYBLUE] = tmp;
		}
	}

    pthread_exit(NULL);
}



int main()
{
	// read mask file
	FILE* mask;

	mask = fopen("mask_Sobel.txt", "r");
	fscanf(mask, "%d", &SOBEL_FILTER_SIZE);

	filter_gx = new int[SOBEL_FILTER_SIZE];
	filter_gy = new int[SOBEL_FILTER_SIZE];

	for (int i = 0; i < SOBEL_FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_gx[i]);

	for (int i = 0; i < SOBEL_FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_gy[i]);

	fclose(mask);
	
	BmpReader* bmpReader = new BmpReader();
	int size;
	for (int k = 0; k < 5; k++){
		// read input BMP file
		pic_in = bmpReader->ReadBMP(inputfile_name[k], &imgWidth, &imgHeight);
		// allocate space for output image
		size = imgWidth*imgHeight*sizeof(unsigned char);
		pic_grey = (unsigned char*)malloc(size);
		pic_mean = (unsigned char*)malloc(size);
		pic_final = (unsigned char*)malloc(3 * size);

		pthread_t rgb, mean, sobel;

		pthread_mutex_lock(&mutex);
		pthread_create(&rgb, NULL, child_RGB2grey, NULL);
		pthread_cond_wait(&condition, &mutex);

		pthread_create(&mean, NULL, child_MeanFilter, NULL);
		pthread_cond_wait(&condition, &mutex);
		pthread_mutex_unlock(&mutex);

		pthread_create(&sobel, NULL, child_SobelFilter, NULL);

		pthread_join(rgb, NULL);
		pthread_join(mean, NULL);
		pthread_join(sobel, NULL);

		bmpReader->WriteBMP(outputMed_name[k], imgWidth, imgHeight, pic_final);

		//free memory space
		free(pic_in);
		free(pic_grey);
		free(pic_mean);
		free(pic_final);
	}

	return 0;
}
