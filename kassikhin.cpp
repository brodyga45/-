#include "kassikhin.h"

cv::Mat big_white(cv::Mat &image) {
	cv::Mat start = image.clone();
	const int n = 800, m = 600;
	std::vector < std::vector <bool> > mark(n, std::vector <bool> (m, 0));
	std::vector < std::pair <int, int> > que;
	std::vector <char> null = {-1};
	std::vector <char> e = {0};

	for (int i = 0; i < n; i++) {
		if (get(start, i, 0) == e) {
			que.push_back(std::make_pair(i, 0));
		}
		if (get(start, i, m - 1) == e) {
			que.push_back(std::make_pair(i, m - 1));
		}
	}
	for (int i = 0; i < m; i++) {
		if (get(start, 0, i) == e) {
			que.push_back(std::make_pair(0, i));
		}
		if (get(start, n - 1, i) == e) {
			que.push_back(std::make_pair(n - 1, i));
		}
	}

	std::vector < std::pair <int, int> > comon = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

	while (que.size() != 0) {
		std::pair <int, int> nw = que.back();
		que.pop_back();
		int x, y;
		x = nw.first;
		y = nw.second;
		mark[x][y] = 1;
		
		for (int i = 0; i < comon.size(); i++) {
			int xa = comon[i].first;
			int ya = comon[i].second;
			if (x + xa < 0 || x + xa >= n) {
				continue;
			}
			if (y + ya < 0 || y + ya >= m) {
				continue;
			}
			if (get(start, x + xa, y + ya) == e && mark[x + xa][y + ya] == 0) {
				que.push_back(std::make_pair(x + xa, y + ya));
			}
		}
	}
	
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			if (!mark[i][j]) {
				set(start, i, j, null);
			}
		}
	}
	return start;
}

void dfs(int x, int y, cv::Mat &image, std::vector < std::vector <int> > &mr, int cl, std::vector< std::pair <int, int> > &comon, int n, int m, std::vector <char> &e) {
	if (mr[x][y] != 0) {
		return;
	}
	mr[x][y] = cl;
	for (int i = 0; i < comon.size(); i++) {
		int xa, ya;
		xa = comon[i].first;
		ya = comon[i].second;
		if (x + xa < 0 || x + xa >= n || y + ya < 0 || y + ya >= m) {
			continue;
		}
		if (get(image, x + xa, y + ya) == e) {
			continue;
		}
		dfs(x + xa, y + ya, image, mr, cl, comon, n, m, e);
	}
	return;
}

std::vector<cv::Mat> chop_image(cv::Mat &etalon, cv::Mat &image) {
	const int n = 800, m = 600;
	std::vector < std::pair <int, int> > comon = { { -1, 0 },{ 0, -1 },{ 1, 0 },{ 0, 1 } };
	std::vector < std::vector <int> > mr(n, std::vector <int>(m, 0));
	cv::Mat empty = image.clone();
	std::vector <char> null = { -1 };
	std::vector <char> e = { 0 };

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			set(empty, i, j, { 0, 0, 0 });
		}
	}

	int cl = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			if (get(etalon, i, j) == e || mr[i][j] != 0) {
				continue;
			}
			cl++;
			dfs(i, j, etalon, mr, cl, comon, n, m, e);
		}
	}
	std::vector < cv::Mat > ans(cl);


	for (int i = 0; i < cl; i++) {
		ans[i] = empty.clone();
	}

	std::vector<int> l(cl, n), r(cl, 0), u(cl, 0), d(cl, m);

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			if (mr[i][j] == 0) {
				continue;
			}
			l[mr[i][j] - 1] = std::min(l[mr[i][j] - 1], i);
			r[mr[i][j] - 1] = std::max(r[mr[i][j] - 1], i);



			u[mr[i][j] - 1] = std::max(u[mr[i][j] - 1], j);
			d[mr[i][j] - 1] = std::min(d[mr[i][j] - 1], j);


			set(ans[mr[i][j] - 1], i, j, get(image, i, j));
		}
	}

	for (int i = 0; i < cl; i++) {
		cv::Rect rect(l[i], d[i], r[i] - l[i], u[i] - d[i]);

		ans[i] = ans[i](rect);
	}

	return ans;
}