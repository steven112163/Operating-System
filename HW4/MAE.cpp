#include "bmpReader.h"
#include "bmpReader.cpp"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <math.h>
using namespace std;

double MAE(char const *name1, char const *name2)
{
	int imgWidth_1, imgHeight_1, imgWidth_2, imgHeight_2;
	unsigned char *pic_1, *pic_2;
	int W, H, a;
	double sum;

	BmpReader* bmpReader = new BmpReader();

	pic_1 = bmpReader->ReadBMP(name1, &imgWidth_1, &imgHeight_1);
	pic_2 = bmpReader->ReadBMP(name2, &imgWidth_2, &imgHeight_2);

	if(pic_1==0 || pic_2==0)
		return -1;

	if (imgWidth_1!=imgWidth_2 || imgHeight_1!=imgHeight_2)
	{
		cout << " Warning: The size of 2 images is not the same!\n";
		W = min(imgWidth_1,imgWidth_2);
		H = min(imgHeight_1,imgHeight_2);
	}
	else
	{
		W = imgWidth_1;
		H = imgHeight_1;
	}
	
	sum = 0;
	for (int j = 0; j<H; j++) {
		for (int i = 0; i<W; i++){
			for (int k = 0; k<3; k++){
				a = abs(pic_1[3*(j*imgWidth_1 + i) + k] - pic_2[3*(j*imgWidth_2 + i) + k]);
				sum += a;
			}
		}
	}
	sum = sum/(W*H*3);

	free(pic_1);
	free(pic_2);
	return sum;
}

int main(int argc, char **argv)
{
	char *name1 = argv[1];
	char *name2 = argv[2];
	//cout << "argc = "<<argc<<", name1 = " <<name1<<", name2 = "<<name2<<endl;

	int i, j, k, sum;
	double temp;

	temp = MAE(name1,name2);
	cout << "MAE = " << temp << endl;


	return 0;
}