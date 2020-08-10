#' @title Text Line Segmentation based on valley finding in projection profiles
#' @description Text Line Segmentation based on valley finding in projection profiles
#' @param x an object of class magick-image
#' @param crop logical indicating to crop white space on the borders of the image
#' @param resize_width resize after cropping to this specified width
#' @param light logical indicating to remove light effects due to scanning
#' @param type which type of binarisation to perform before doing line segmentation
#' @export 
#' @return a list with elements
#' \itemize{
#' \item{n: the number of lines found}
#' \item{overview: an opencv-image of the detected areas}
#' \item{textlines: a list of opencv-image's, one for each text line area}
#' }
#' @examples 
#' library(opencv)
#' library(magick)
#' library(image.textlinedetector)
#' path  <- system.file(package = "image.textlinedetector", "extdata", "example.png")
#' img   <- image_read(path)
#' areas <- image_textlines_flor(img, crop = TRUE, light = TRUE, type = "sauvola")
#' areas$n
#' areas$overview
#' combined <- lapply(areas$textlines, FUN=function(x) image_read(ocv_bitmap(x)))
#' combined <- do.call(c, combined)
#' combined
image_textlines_flor <- function(x, crop = TRUE, resize_width = 1280, light = TRUE, type = c("none", "niblack", "sauvola", "wolf")){
  stopifnot(inherits(x, "magick-image"))
  type <- match.arg(type)
  type <- switch(type, none = 1L, niblack = 2L, sauvola = 3L, wolf = 4L)

  width  <- image_info(x)$width
  height <- image_info(x)$height
  x   <- image_data(x, channels = "bgr")
  img <- cvmat_bgr(x, width = width, height = height)
  if(crop){
    img <- textlinedetector_crop(img)
  }
  if(!missing(resize_width)){
    img <- textlinedetector_resize(img, width = resize_width)
  }
  if(!missing(type)){
    img <- textlinedetector_binarization(img, light = light, type = type)
  }
  out <- textlinedetector_linesegmentation(img)
  out
}


#' @title Text Line Segmentation based on the A* Path Planning Algorithm
#' @description Text Line Segmentation based on the A* Path Planning Algorithm
#' @param x an object of class magick-image
#' @param morph logical indicating to apply a morphological 5x5 filter
#' @param step step size of A-star
#' @param mfactor multiplication factor in the cost heuristic of the A-star algorithm
#' @param trace logical indicating to show the evolution of the line detection
#' @export 
#' @return a list with elements
#' \itemize{
#' \item{n: the number of lines found}
#' \item{overview: an opencv-image of the detected areas}
#' \item{textlines: a list of opencv-image's, one for each text line area}
#' \item{lines: a data.frame with the x/y positions of the detected lines}
#' }
#' @examples 
#' library(opencv)
#' library(magick)
#' library(image.binarization)
#' library(image.textlinedetector)
#' path  <- system.file(package = "image.textlinedetector", "extdata", "example.png")
#' path  <- system.file("extdata", "doxa-example.png", package = "image.binarization")
#' img   <- image_read(path)
#' img   <- image_binarization(img, type = "su")
#' areas <- image_textlines_astar(img, morph = TRUE, step = 2, mfactor = 5, trace = TRUE)
#' areas$n
#' areas$overview
#' areas$lines
#' areas$textlines[[2]]
#' areas$textlines[[4]]
#' combined <- lapply(areas$textlines, FUN=function(x) image_read(ocv_bitmap(x)))
#' combined <- do.call(c, combined)
#' combined
image_textlines_astar <- function(x, morph = FALSE, step = 2, mfactor = 5, trace = FALSE){
  stopifnot(inherits(x, "magick-image"))
  width  <- image_info(x)$width
  height <- image_info(x)$height
  x   <- image_data(x, channels = "gray")
  img <- cvmat_bw(x, width = width, height = height)
  out <- textlinedetector_astarpath(img, morph = morph, step = step, mfactor = mfactor, trace = trace)
  out
}


#' @title Find Words by Connected Components Labelling
#' @description Filter the image using the kernel and components which are connected are considered as words
#' @param x an object of class opencv-image
#' @param kernelSize size of the kernel
#' @param sigma sigma of the kernel
#' @param theta theta of the kernel
#' @export 
#' @return a list with elements
#' \itemize{
#' \item{n: the number of lines found}
#' \item{overview: an opencv-image of the detected areas}
#' \item{words: a list of opencv-image's, one for each word area}
#' }
#' @examples 
#' library(opencv)
#' library(magick)
#' library(image.textlinedetector)
#' path  <- system.file(package = "image.textlinedetector", "extdata", "example.png")
#' img   <- image_read(path)
#' areas <- image_textlines_flor(img, crop = TRUE, light = TRUE, type = "sauvola")
#' areas$overview
#' areas$textlines[[6]]
#' textwords <- image_wordsegmentation(areas$textlines[[6]])
#' textwords$n
#' textwords$overview
#' textwords$words[[2]]
#' textwords$words[[3]]
#' combined <- lapply(textwords$words, FUN=function(x) image_read(ocv_bitmap(x)))
#' combined <- do.call(c, combined)
#' combined
image_wordsegmentation <- function(x, kernelSize = 11L, sigma = 11L, theta = 7L){
  out <- textlinedetector_wordsegmentation(x, kernelSize = kernelSize, sigma = sigma, theta = theta)
  out
}
