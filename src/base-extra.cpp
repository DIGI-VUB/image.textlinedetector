#include "opencv_types.h"
#include "util.hpp"

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