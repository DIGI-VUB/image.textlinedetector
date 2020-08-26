#include <Rcpp.h>
// For opencv R package
#include "opencv_types.h"
#include "util.hpp"
// For line localization
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline Mat distance_transform (Mat input) {

	Mat dmat = input.clone();
	for (int i = 0; i < input.cols; i++) {
		Mat column = input(Rect(i, 0, 1, input.rows));
		Mat dcol;
#if (defined(CV_VERSION_EPOCH) && CV_VERSION_EPOCH == 2)
		distanceTransform(column, dcol, CV_DIST_L2, 5);
#else
		distanceTransform(column, dcol, DIST_L2, 5);
#endif		
		dcol.copyTo(dmat.col(i));
	}

	return dmat;
}
template<typename Node>
inline Mat segment_line (Mat& input, vector<Node> path){

	Mat output = input.clone();
	for (auto node: path) {
		int row, col;
		tie(row, col) = node;
		for (int i = row; i < input.rows; i++) {
			output.at<uchar>(i, col) = (uchar) 255;
			if (col < output.cols) {
				output.at<uchar>(i, col + 1) = (uchar) 255;
			}
		}
	}
	return output;
}
template<typename Node>
inline void draw_path (Mat& graph, vector<Node>& path) {

	for (auto node : path) {
		int row, col;
		tie (row, col) = node;
		graph.at<uchar>(row, col) = (uchar) 0;
		if (col < graph.cols) {
			graph.at<uchar>(row, col + 1) = (uchar) 0;
		}
	}
	//imwrite("map.jpg", graph*255);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <opencv2/opencv.hpp>
#include "persistence1d.hpp"
#include <algorithm>

using namespace cv;
using namespace std;
using namespace p1d;


inline void invert (Mat& im, Mat& output) {
	output = abs(255 - im) / 255;
}

inline void enhance (Mat& im, Mat& output) {
	Mat element = getStructuringElement( MORPH_RECT, Size(7, 7), Point(3, 3));
	morphologyEx(im, output, 2, element);
}

inline vector<int> detect_peaks (Mat& hist, double delta) {

	vector<float> data;
	for (unsigned int i = 0; i < hist.total(); i++) {
		data.push_back((float) hist.at<double>(i, 0));
	}

	Persistence1D detector;
	detector.RunPersistence(data);

	vector<TPairedExtrema> Extrema;
	detector.GetPairedExtrema(Extrema, delta);

	vector<int> lines;
	for(vector< TPairedExtrema >::iterator it = Extrema.begin(); it != Extrema.end(); it++) {
		lines.push_back((*it).MaxIndex);
	}

	return lines;
}

inline vector<int> projection_analysis (Mat& im) {

	im.convertTo(im, CV_64F);
	Mat hist = Mat(im.rows, 1, CV_64F);
	reduce(im, hist, 1, 0, CV_64F);

	Scalar _mean, _std;
	meanStdDev(hist, _mean, _std);
	double hist_mean = _mean.val[0];
	double hist_std = _std.val[0];

	double min, max;
	minMaxLoc(hist, &min, &max);

	hist = hist / max;
	double delta = hist_mean / max + 0.6*(hist_std / max); //0.6 is to try to detect lines that are short
	// double epsilon = 0.015; //to compensate error in peak detection for some cases
	return detect_peaks (hist, delta);
}

inline vector<int> localize (Mat& input) {

	Mat im;
	enhance(input, im);
	invert(im, im);
	vector<int> peaks = projection_analysis(im);
	sort(peaks.begin(), peaks.end());  
	vector<int> lines;
	if(peaks.size() < 1){
	  return lines;
	}
	int dist, valley;
	for (unsigned int i = 0; i < (peaks.size() - 1); i++) {
		dist = (peaks[i + 1] - peaks[i]) / 2;
		valley = peaks[i] + dist;
		lines.push_back(valley);
	}
	return lines;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "opencv2/opencv.hpp"
#include <queue>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <limits>

using namespace cv;
using namespace std;


struct Map {

	typedef tuple<int, int> Node;
	Mat grid;
	Mat dmat;
	Node directions[8] = {Node{-1, -1}, Node{-1, 0}, Node{-1, 1},
						  Node{0, -1}, Node{0, 1},
						  Node{1, -1}, Node{1, 0}, Node{1, 1}};

	inline bool in_bounds (Node node) const {
		int row, col;
		tie (row, col) = node;
		return 0 <= row and row < grid.rows and 0 <= col and col < grid.cols;
	}

	inline bool is_wall (Node node) const {
		int row, col;
		tie (row, col) = node;
		return (int) grid.at<uchar>(row, col) == 0;
	}

	inline int closest_vertical_obstacle (Node node) const {
		int row, col, dist;
		tie (row, col) = node;
		dist = (int) dmat.at<uchar>(row, col);
		if (dist < 255) {
			return dist;
		} else {
			//return INFINITY;
      return std::numeric_limits<int>::max();
		}
	}

	vector<Node> neighbors(Node node, int step) const {
		int row, col, dr, dc;
		tie (row, col) = node;
		vector<Node> neighbors;

		for (auto dir : directions) {
			tie (dr, dc) = dir;
			Node neighbor(row + step*dr, col + step*dc);
			if (in_bounds(neighbor)) {
				neighbors.push_back(neighbor);
			}
		}
		return neighbors;
	}

};

template<typename T, typename Priority = double>
struct PriorityQueue {

	typedef pair<Priority, T> Element;
	priority_queue<Element, vector<Element>, greater<Element>> elements;

	inline bool empty () {
		return elements.empty();
	}

	inline void put (T element, Priority priority) {
		elements.emplace(priority, element);
	}

	inline T get () {
		T element = elements.top().second;
		elements.pop();
		return element;
	}

};

template<typename Node>
inline double heuristic (Node start, Node end, int mfactor) {
	int r1, r2, c1, c2;
	tie (r1, c1) = start;
	tie (r2, c2) = end;
	double a = pow((r1 - r2), 2);
	double b = pow((c1 - c2), 2);

	return mfactor*sqrt(a + b);
}

template<typename Node>
inline double V (Node node, Node start) {
	int row, col, st_row, st_col;
	tie (row, col) = node;
	tie (st_row, st_col) = start;
	return abs(row - st_row);
}

template<typename Node>
inline double N (Node current, Node neighbor) {
	int crow, ccol, nrow, ncol;
	tie(crow, ccol) = current;
	tie (nrow, ncol) = neighbor;
	if (crow == nrow or ccol == ncol) {
		return (double) 10;
	} else {
		return (double) 14;
	}
}

template<typename Graph>
inline double M (const Graph& graph, typename Graph::Node node) {
	if (graph.is_wall(node)) {
		return (double) 1;
	} else {
		return (double) 0;
	}
}

template<typename Graph>
inline tuple<double, double> D (const Graph& graph, typename Graph::Node node) {
	double min = (double) graph.closest_vertical_obstacle(node);
	tuple<double, double> ds{1 / (1 + min), 1 / (1 + pow(min, 2))};
	return ds;
}

template<typename Graph>
inline double compute_cost (const Graph& graph, typename Graph::Node current, typename Graph::Node neighbor, typename Graph::Node start, string dataset) {
	double v = V(neighbor, start);
	double n = N(current, neighbor);
	double m = M(graph, neighbor);
	tuple<double, double> ds = D(graph, neighbor);
	double d, d2;
	tie (d, d2) = ds;

	if (strcmp(dataset.c_str(), "MLS") == 0) {
		return 2.5*v + 1*n + 50*m + 130*d + 0*d2;
	} else {
		return 3*v + 1*n + 50*m + 150*d + 50*d2;
	}
}

namespace std {
  template <>
  struct hash<tuple<int,int> > {
    inline size_t operator() (const tuple<int,int>& node) const {
      int x, y;
      tie (x, y) = node;
      return x * 1812433253 + y;
    }
  };
}

template<typename Node>
inline vector<Node> reconstruct_path (Node start, Node goal, unordered_map<Node, Node>& parents) {
	vector<Node> path;
	Node current = goal;
	path.push_back(current);
	while (current != start) {
		current = parents[current];
		path.push_back(current);
	}

	reverse(path.begin(), path.end());
	return path;
}

template<typename Graph>
inline void astar_search (const Graph& graph, typename Graph::Node start, typename Graph::Node goal,
				   unordered_map<typename Graph::Node, typename Graph::Node>& parents, string dataset_name, int step, int mfactor) {

	typedef typename Graph::Node Node;
	unordered_map<Node, double> gscore;
	unordered_set<Node> closedSet;
	PriorityQueue<Node> openSet;
	openSet.put(start, 0);
	gscore[start] = 0;

	while (not openSet.empty()) {

		auto current = openSet.get();

		if (current == goal) {
			break;
		}

		for (auto neighbor : graph.neighbors(current, step)) {

			if (closedSet.count(neighbor)) {
				continue;
			}

			double new_gscore = gscore[current] + compute_cost(graph, current, neighbor, start, dataset_name); //heuristic(current, neighbor);
			if (!gscore.count(neighbor) or new_gscore < gscore[neighbor]) {
				gscore[neighbor] = new_gscore;
				parents[neighbor] = current;
				double fscore = new_gscore + heuristic(neighbor, goal, mfactor);
				openSet.put(neighbor, fscore);
			}
		}
	}

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// [[Rcpp::export]]
Rcpp::List textlinedetector_astarpath(XPtrMat ptr, bool morph = true, int step = 2, int mfactor = 5, bool trace = true){
  cv::Mat img = get_mat(ptr);
  cv::Mat imbw = img.clone();
  if(morph){
    Mat element = getStructuringElement( MORPH_RECT, Size(5, 5), Point(2, 2));
    morphologyEx(img, imbw, 2, element );
  }
  if(trace) Rcpp::Rcout << "- Detecting lines location..";
  vector<int> lines = localize(imbw);
  if(trace) Rcpp::Rcout << " ==> " << lines.size() + 1 << " areas found." << endl;
  if(trace) Rcpp::Rcout << "- A* path planning algorithm.." << endl;
  Map map;
  map.grid = imbw / 255;
  map.dmat = distance_transform(map.grid);
  typedef Map::Node Node;
	vector<vector<Node>> paths;
	Mat image_path = map.grid.clone();
  Rcpp::List segmented_boxes(lines.size() + 1);
  int previous_y = 0;
  int i = 1;
  if(lines.size() == 0){
    i = 0;
  }
  std::vector<int> from_x;
  std::vector<int> to_x;
  std::vector<int> from_y;
  std::vector<int> to_y;
	for (vector<int>::iterator itr = lines.begin(); itr != lines.end(); itr++) {
    i = distance(lines.begin(), itr) + 1;
    int end;
    if ((map.grid.cols - 1) % 2 == 0) {
      end = map.grid.cols - 1;
		} else {
      end = map.grid.cols - 2;
		}
    Node start{*itr, 0};
    Node goal{*itr, end};
    from_x.push_back(get<0>(start));    
    from_y.push_back(get<1>(start));
    to_x.push_back(get<0>(goal));
    to_y.push_back(get<1>(goal));
		if(trace) Rcpp::Rcout << "\tline #" << to_string(i) + " - from [" << get<0>(start) << ", " << get<1>(start) << "]" << " to [" << get<0>(goal) << ", " << get<1>(goal) << "]" << endl;
    int current_y = get<0>(start);
    int width  = get<1>(goal) - get<1>(start);
    int height = current_y - previous_y;
    //Rcpp::Rcout << current_y << " " << previous_y << " " << width << " " << height << "::" << img.rows << "x" << img.cols << "\n";
    cv::Mat cropped = img(cv::Rect(0, previous_y, width, height));    
    segmented_boxes[i-1] = cvmat_xptr(cropped);
    previous_y = current_y;

		unordered_map<Node, Node> parents;
    astar_search(map, start, goal, parents, "NULL", step, mfactor);
    vector<Node> path = reconstruct_path(start, goal, parents);
    draw_path(image_path, path);
    paths.push_back(path);
  }
  cv::Mat cropped = img(cv::Rect(0, previous_y, img.cols, img.rows - previous_y));
  segmented_boxes[i] = cvmat_xptr(cropped);

  Rcpp::List pathlines(paths.size());
  for (unsigned int i=0; i<paths.size(); i++) {
    auto path = paths[i];
    std::vector<int> x;
    std::vector<int> y;
    for (auto node : path) {
      int row, col;
      tie (row, col) = node;
      x.push_back(col);
      y.push_back(row);
    }
    pathlines[i] = Rcpp::DataFrame::create(Rcpp::Named("x") = x,
                                           Rcpp::Named("y") = y);
    //segmented_images[i] = cvmat_xptr(segment_line(img, path));
  }
  

  return Rcpp::List::create(Rcpp::Named("n") = paths.size(),
                            Rcpp::Named("overview") = cvmat_xptr(image_path*255), 
                            Rcpp::Named("paths") = pathlines,
                            Rcpp::Named("lines") = Rcpp::DataFrame::create(Rcpp::Named("x_from") = from_x, Rcpp::Named("x_to") = to_x,
                                                                           Rcpp::Named("y_from") = from_y, Rcpp::Named("y_to") = to_y),
                            Rcpp::Named("textlines") = segmented_boxes);
}


