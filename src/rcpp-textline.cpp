#include <Rcpp.h>
// For opencv R package
#include "opencv_types.h"
#include "util.hpp"
// For line segmentation
#include "Binarization.hpp"
#include "Scanner.hpp"
#include "LineSegmentation.hpp"
#include "WordSegmentation.hpp"
#include <string>

//' @export
// [[Rcpp::export]]
XPtrMat textlinedetector_crop(XPtrMat ptr){
  cv::Mat image = get_mat(ptr);
  
  Scanner *scanner = new Scanner();
  cv::Mat imageCropped;
  scanner->process(image, imageCropped);
  return cvmat_xptr(imageCropped);
}

//' @export
// [[Rcpp::export]]
XPtrMat textlinedetector_resize(XPtrMat ptr, int newW = 1280){
  cv::Mat imageCropped = get_mat(ptr);
  
  int newH = ((newW * imageCropped.rows) / imageCropped.cols);
  resize(imageCropped, imageCropped, Size(newW, newH));  
  return cvmat_xptr(imageCropped);
}

//' @export
// [[Rcpp::export]]
XPtrMat textlinedetector_binarization(XPtrMat ptr, bool light = true, int type = 3){
  cv::Mat imageCropped = get_mat(ptr);
  
  Binarization *threshold = new Binarization();
  cv::Mat imageBinary;
  threshold->binarize(imageCropped, imageBinary, light, type);
  return cvmat_xptr(imageBinary);
}


//' @export
// [[Rcpp::export]]
Rcpp::List textlinedetector_linesegmentation(XPtrMat ptr, int chunksNumber = 8, int chunksProcess = 4, int kernelSize = 11, int sigma = 11, int theta = 7){
  cv::Mat imageBinary = get_mat(ptr);
  
  LineSegmentation *line = new LineSegmentation();
  std::vector<cv::Mat> lines;
  cv::Mat imageLines = imageBinary.clone();
  line->segment(imageLines, lines, chunksNumber, chunksProcess);

  Rcpp::List textlines(lines.size());
  for (int i=0; i<lines.size(); i++) {
    textlines[i] = cvmat_xptr(lines[i]);
  }
  
  return Rcpp::List::create(Rcpp::Named("n") = lines.size(),
                            Rcpp::Named("imagelines") = cvmat_xptr(imageLines),
                            Rcpp::Named("textlines") = textlines,
                            Rcpp::Named("example") = cvmat_xptr(lines[0]));
}

//' @export
// [[Rcpp::export]]
Rcpp::List textlinedetector_wordsegmentation(XPtrMat ptr, int kernelSize = 11, int sigma = 11, int theta = 7){
  cv::Mat textlineimg = get_mat(ptr);
  
  // START Step 4: word segmentation //
  WordSegmentation *word = new WordSegmentation();
  cv::Mat summary;
  word->setKernel(kernelSize, sigma, theta);
  
  std::vector<cv::Mat> words;
  word->segment(textlineimg, words);
    
  //summary.push_back(words[0]);
  //words.erase(words.begin());
  Rcpp::List tokens(words.size());
  for (int i=0; i<words.size(); i++) {
    tokens[i] = cvmat_xptr(words[i]);
  }
  
  return Rcpp::List::create(Rcpp::Named("n") = words.size(),
                            Rcpp::Named("words") = tokens,
                            Rcpp::Named("example") = cvmat_xptr(words[0]));
}