#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "afin.h"
using namespace std;
bool check_for_black(vector <char> c) {
	return (int)(unsigned char)c[0] + (int)(unsigned char)c[1] + (int)(unsigned char)c[2] > 15;
}
vector < vector <long double> > preob(int vl, int vr) {
	vector < vector <long double> > ans = { {1, 0}, {0, 1} };
	ans[1][0] = -(long double)(vr - vl) / 99.0;
	long double k = 99.0 / (sqrt((long double)(vr - vl) * (long double)(vr - vl) + 99.0 * 99.0));
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			ans[i][j] *= k;
		}
	}
	swap(ans[1], ans[0]);
	return ans;
}

cv::Mat norm_image(cv::Mat im) {
	const int n = 100, m = 100;
	/*bool mr[100][100];
	for (int j = 0; j < m; j++){
	for (int i = 0; i < n; i++) 
			if (check_for_black(get(im, i, j)))
				cout << 0;
			else
				cout << 1;
		cout << endl;
	}*/

	int vl = 100, vr = 100;
	for (int i = 0; i <n - 2; i++) {
		if (check_for_black(get(im, 2, i)) && check_for_black(get(im, 2, i+1)) && check_for_black(get(im, 2, i+2))) {
			vl = min(vl, i);
		}
		if (check_for_black(get(im, n-3, i)) && check_for_black(get(im, n-3, i+1)) && check_for_black(get(im, n-3, i+2))) {
			vr = min(vr, i);
		}
	}
//	vector < vector <long double> > pr = preob(vl, vr);

	cv::Mat image = im.clone();

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			set(image, i, j, { 0, 0, 0 });
		}
	}
	//long double k = (sqrt((long double)(vr - vl) * (long double)(vr - vl) + 99.0 * 99.0)) / 99.0;
	
	long double mix = 100, mx = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0;j < m; j++)
			if(check_for_black(get(im,i,j))){
			long double xc, y, x, yc;
			xc = i;
			yc = j;
			y = yc;
			x = xc;
			long double vl1 = vl, vr1 = vr;
			y = y - (vr1 - vl1)/(96.0)*x*1.05;
			/*mix = min(mix, max((long double)0.0, y));
			mx = max(mx, min((long double)100.0, y));
			int xa, ya;
			xa = x + 0.0001;
			ya = y + 0.0001;
			xa = max(0, xa);
			xa = min(99, xa);
			ya = max(0, ya);
			ya = min(99, ya);
			if (xa == 0) {
				int ghghhg = 55;
			}
			*/
			int xa, ya;
			xa = x + 0.0001;
			ya = y + 0.0001;
			xa = max(0, xa);
			xa = min(99, xa);
			ya = max(0, ya);
			ya = min(99, ya);
			set(image, xa, ya, get(im, i, j));
		}
	}
	int miny = 100;
	int maxy = 0;
	for(int i = 0; i < n; i++)
		for(int j = 0; j < m; j++)
			if (check_for_black(get(image, i, j)))
			{
				miny = min(miny, j);
				maxy = max(maxy, j);
			}
	cv::Mat imcl = image.clone();
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			set(imcl, i, j, COLOR_BLACK);
	for (int i = 0; i < n; i++)
		for (int j = miny+3; j < m; j++)
			set(imcl, i, j-miny-3, get(image, i, j));
	for (int i = 0; i < n; i++) 
		for (int j = 0; j < m; j++)
			set(image, i, j, COLOR_BLACK);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++)
			if (check_for_black(get(imcl, i, j))) {
				long double xc, y, x, yc;
				xc = i;
				yc = j;
				y = yc;
				x = xc;
				y = y * 100 / (maxy - miny);
				/*mix = min(mix, max((long double)0.0, y));
				mx = max(mx, min((long double)100.0, y));
				int xa, ya;
				xa = x + 0.0001;
				ya = y + 0.0001;
				xa = max(0, xa);
				xa = min(99, xa);
				ya = max(0, ya);
				ya = min(99, ya);
				if (xa == 0) {
				int ghghhg = 55;
				}
				*/
				int xa, ya;
				xa = x + 0.0001;
				ya = y + 0.0001;
				xa = max(0, xa);
				xa = min(99, xa);
				ya = max(0, ya);
				ya = min(99, ya);
				set(image, xa, ya, get(imcl, i, j));
			}
	}
	return image;
}