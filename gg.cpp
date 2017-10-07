#include "gg.h"

std::vector<int> count_threshold(cv::Mat &image, int type)
{
 
	auto pixel0 = get(image, 0, 0);
	int min = (unsigned char)pixel0[type], max = (unsigned char)pixel0[type];
	int temp, temp1;
	int histSize;

	int alpha, beta, threshold=0;
	double sigma, maxSigma = -1;
	double w1,a;
	int hist[256];

	for (int i = 0; i < 256; i++) hist[i] = 0;

	for (int i = 0; i < image.cols; i++)
		for (int j = 0; j < image.rows; ++j)
		{
			auto pixel = get(image, i, j);
			temp = (unsigned char)pixel[type];
			hist[temp]++;
			if (temp < min)   min = temp;
			if (temp > max)   max = temp;
		}

   histSize = max - min + 1;
   
   temp = temp1 = 0;
   alpha = beta = 0;

   for (int i = 0; i <= 255; i++)
   {
      temp += i*hist[i];
      temp1 += hist[i];
   }

   for (int i = 0; i < 256; i++)
   {
      alpha += i * hist[i];
      beta += hist[i];

      w1 = (double)beta / temp1;
      a = (double)alpha / beta - (double)(temp - alpha) / (temp1 - beta);
      sigma = w1 * (1 - w1) * a * a;
    
      if(sigma > maxSigma)
      {
         maxSigma = sigma;
         threshold = i;
      }
   }

   int histSum = 0;
   for (int i = 0; i < 256; i++) histSum += hist[i];

   min = 0;
   int sum = 0;
   while (sum * 10 < histSum && min < 255) sum += hist[min++];

   max = 255;
   sum = 0;
   while (sum * 10 < histSum && max > 0) sum += hist[max--];

   return { threshold, min, max };
}