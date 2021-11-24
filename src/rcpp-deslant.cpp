#include <Rcpp.h>
// For opencv R package
#include "opencv_types.h"
#include "util.hpp"
// For deslanting
#include "DeslantImgCPU.hpp"


// [[Rcpp::export]]
XPtrMat textlinedetector_deslant(XPtrMat ptr, const int bgcolor=255, const float lower_bound=-1.0, const float upper_bound=1.0){
  cv::Mat image = get_mat(ptr);
  cv::Mat imageDeslanted;
  imageDeslanted = htr::deslantImg(image, bgcolor, lower_bound, upper_bound);
  return cvmat_xptr(imageDeslanted);
}