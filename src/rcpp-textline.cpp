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

// [[Rcpp::export]]
XPtrMat textlinedetector_crop(XPtrMat ptr){
  cv::Mat image = get_mat(ptr);
  
  Scanner *scanner = new Scanner();
  cv::Mat imageCropped;
  scanner->process(image, imageCropped);
  return cvmat_xptr(imageCropped);
}

// [[Rcpp::export]]
XPtrMat textlinedetector_resize(XPtrMat ptr, int width = 1280){
  int newW = width;
  cv::Mat imageCropped = get_mat(ptr);
  
  int newH = ((newW * imageCropped.rows) / imageCropped.cols);
  resize(imageCropped, imageCropped, Size(newW, newH));  
  return cvmat_xptr(imageCropped);
}

// [[Rcpp::export]]
XPtrMat textlinedetector_binarization(XPtrMat ptr, bool light = true, int type = 3){
  cv::Mat imageCropped = get_mat(ptr);
  
  Binarization *threshold = new Binarization();
  cv::Mat imageBinary;
  threshold->binarize(imageCropped, imageBinary, light, type);
  return cvmat_xptr(imageBinary);
}


// [[Rcpp::export]]
Rcpp::List textlinedetector_linesegmentation(XPtrMat ptr, int chunksNumber = 8, int chunksProcess = 4, int kernelSize = 11, int sigma = 11, int theta = 7){
  cv::Mat imageBinary = get_mat(ptr);
  
  LineSegmentation *line = new LineSegmentation();
  std::vector<cv::Mat> lines;
  cv::Mat imageLines = imageBinary.clone();
  line->segment(imageLines, lines, chunksNumber, chunksProcess);

  Rcpp::List textlines(lines.size());
  for (unsigned int i=0; i<lines.size(); i++) {
    textlines[i] = cvmat_xptr(lines[i]);
  }
  /*
  // Get region x/y locations
  unsigned int regions_nr = line->lineRegions.size();
  Rcpp::List coords(regions_nr);
  for (unsigned int i=0; i<regions_nr; i++) {
    std::vector<int> top_x;
    std::vector<int> top_y;
    std::vector<int> bottom_x;
    std::vector<int> bottom_y;    

    auto region = line->lineRegions[i];
    Line top    = *region->top;
    Line bottom = *region->bottom;
    
    for (auto point : region->top->points) {
      top_x.push_back(point->x);
      top_y.push_back(point->y);
    }

    for (auto point : region->bottom->points) {
      bottom_x.push_back(point->x);
      bottom_y.push_back(point->y);  
    }
    coords[i] = Rcpp::List::create(Rcpp::Named("regionID") = region->regionID,  
    Rcpp::Named("test") = top.points.size(),
                                   Rcpp::Named("height") = region->height//,  
                                   //Rcpp::Named("top_x")    = top_x,
                                   //Rcpp::Named("top_y")    = top_y,
                                   //Rcpp::Named("bottom_x") = bottom_x,
                                   //Rcpp::Named("bottom_y") = bottom_y
                                   );
  }
  */
  return Rcpp::List::create(Rcpp::Named("n") = lines.size(),
                            Rcpp::Named("overview") = cvmat_xptr(imageLines),
                            Rcpp::Named("textlines") = textlines//,
                            //Rcpp::Named("coords") = coords
                            );
}

// [[Rcpp::export]]
Rcpp::List textlinedetector_wordsegmentation(XPtrMat ptr, int kernelSize = 11, int sigma = 11, int theta = 7){
  cv::Mat textlineimg = get_mat(ptr);
  
  WordSegmentation *word = new WordSegmentation();
  cv::Mat summary;
  word->setKernel(kernelSize, sigma, theta);
  
  std::vector<cv::Mat> words;
  word->segment(textlineimg, words);
    
  Rcpp::List tokens(words.size()-1);
  for (unsigned int i=1; i<words.size(); i++) {
    tokens[i-1] = cvmat_xptr(words[i]);
  }
  
  return Rcpp::List::create(Rcpp::Named("n") = words.size() - 1,
                            Rcpp::Named("overview") = cvmat_xptr(words[0]),
                            Rcpp::Named("words") = tokens);
}