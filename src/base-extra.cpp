#include "opencv_types.h"
#include "util.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

// [[Rcpp::export]]
XPtrMat cvmat_bgr(Rcpp::RawVector img, int width = 0, int height = 0){
  std::vector<uchar> x = Rcpp::as<std::vector<uchar>>(img);
  cv::Mat output(height, width, CV_8UC3, x.data());
  return cvmat_xptr(output);
}


// [[Rcpp::export]]
XPtrMat cvmat_bw(Rcpp::RawVector img, int width = 0, int height = 0){
  std::vector<uchar> x = Rcpp::as<std::vector<uchar>>(img);
  cv::Mat output(height, width, CV_8U, x.data());
  return cvmat_xptr(output);
}


// [[Rcpp::export]]
XPtrMat cvmat_rect(XPtrMat ptr, int x, int y, int width = 0, int height = 0){
  cv::Mat img = get_mat(ptr);
  cv::Rect roi = cv::Rect(x, y, width, height);
  cv::Mat output = img(roi);
  return cvmat_xptr(output);
}


std::vector<cv::Point> as_points(Rcpp::List pts){
  std::vector<cv::Point> points;
  std::vector<int> x = Rcpp::as<std::vector<int>>(pts["x"]);
  std::vector<int> y = Rcpp::as<std::vector<int>>(pts["y"]);
  for (size_t i = 0; i < x.size(); ++i){
    cv::Point2i pt(x[i], y[i]);
    points.push_back(pt);
  }
  return points;
}

// [[Rcpp::export]]
XPtrMat cvmat_polygon(XPtrMat ptr, Rcpp::List pts, bool convex = false, bool crop = false, int color = 255){
  auto points = as_points(pts);
  cv::Mat img = get_mat(ptr);
  cv::Mat mask = cv::Mat::zeros(img.rows, img.cols, CV_8U);
  if(convex){
    cv::fillConvexPoly(mask, points, cv::Scalar(255, 255, 255));
  }else{
    std::vector<std::vector<cv::Point>> polygons;
    polygons.push_back(points);
    cv::fillPoly(mask, polygons, cv::Scalar(255, 255, 255));
  }
  cv::Mat area(img.rows, img.cols, img.type());
  area.setTo(cv::Scalar(color, color, color));
  img.copyTo(area, mask);
  if(crop){
    cv::Rect roi = cv::boundingRect(points);
    return cvmat_xptr(area(roi));
  }
  return cvmat_xptr(area);
}