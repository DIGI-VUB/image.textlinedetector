#' @title Text Line Segmentation based on valley finding in projection profiles
#' @description Text Line Segmentation based on valley finding in projection profiles
#' @param x an object of class magick-image
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
#' path   <- system.file(package = "image.textlinedetector", "extdata", "example.png")
#' img    <- image_read(path)
#' img    <- image_resize(img, "1000x")
#' areas  <- image_textlines_flor(img, light = TRUE, type = "sauvola")
#' areas  <- lines(areas, img)
#' areas$n
#' areas$overview
#' combined <- lapply(areas$textlines, FUN=function(x) image_read(ocv_bitmap(x)))
#' combined <- do.call(c, combined)
#' combined
#' image_append(combined, stack = TRUE)
image_textlines_flor <- function(x, light = TRUE, type = c("none", "niblack", "sauvola", "wolf")){
  stopifnot(inherits(x, "magick-image"))
  type <- match.arg(type)
  type <- switch(type, none = 1L, niblack = 2L, sauvola = 3L, wolf = 4L)

  width  <- image_info(x)$width
  height <- image_info(x)$height
  x   <- image_data(x, channels = "bgr")
  img <- cvmat_bgr(x, width = width, height = height)
  if(!missing(type)){
    img <- textlinedetector_binarization(img, light = light, type = type)
  }
  out <- textlinedetector_linesegmentation(img)
  class(out) <- c("textlines", "flor")
  out <- lines.textlines(out, img)
  out
}


#' @title Crop an image to extract only the region containing text
#' @description Applies a sequence of image operations to obtain a region which contains relevant texts
#' by cropping white space on the borders of the image.
#' This is done in the following steps:
#' morphological opening, morphological closing, blurring, canny edge detection, convex hull contours of the edges, 
#' keep only contours above the mean contour area, find approximated contour lines of the convex hull contours of these, 
#' dilation and thresholding.
#' @param x an object of class magick-image
#' @return an object of class magick-image
#' @export 
#' @examples 
#' library(opencv)
#' library(magick)
#' library(image.textlinedetector)
#' path  <- system.file(package = "image.textlinedetector", "extdata", "example.png")
#' img   <- image_read(path)
#' image_info(img)
#' img   <- image_textlines_crop(img)
#' image_info(img)
image_textlines_crop <- function(x){
  stopifnot(inherits(x, "magick-image"))
  width  <- image_info(x)$width
  height <- image_info(x)$height
  x   <- image_data(x, channels = "bgr")
  img <- cvmat_bgr(x, width = width, height = height)
  img <- textlinedetector_crop(img)
  image_read(cvmat_bitmap(img))
}


#' @title Find Words by Connected Components Labelling
#' @description Filter the image using the gaussian kernel 
#' and extract components which are connected which are to be considered as words.
#' @param x an object of class opencv-image containing black/white binary data (type CV_8U1)
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
#' img   <- image_resize(img, "x1000")
#' areas <- image_textlines_flor(img, light = TRUE, type = "sauvola")
#' areas$overview
#' areas$textlines[[6]]
#' textwords <- image_wordsegmentation(areas$textlines[[6]])
#' textwords$n
#' textwords$overview
#' textwords$words[[2]]
#' textwords$words[[3]]
image_wordsegmentation <- function(x, kernelSize = 11L, sigma = 11L, theta = 7L){
  out <- textlinedetector_wordsegmentation(x, kernelSize = kernelSize, sigma = sigma, theta = theta)
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
#' \item{paths: a list of data.frame's with the x/y location of the baseline paths}
#' \item{textlines: a list of opencv-image's, one for each rectangular text line area}
#' \item{lines: a data.frame with the x/y positions of the detected lines}
#' }
#' @examples 
#' library(opencv)
#' library(magick)
#' library(image.textlinedetector)
#' path   <- system.file(package = "image.textlinedetector", "extdata", "example.png")
#' img    <- image_read(path)
#' img    <- image_resize(img, "x1000")
#' areas  <- image_textlines_astar(img, morph = TRUE, step = 2, mfactor = 5, trace = TRUE)
#' areas  <- lines(areas, img)
#' areas$n
#' areas$overview
#' areas$lines
#' areas$textlines[[2]]
#' areas$textlines[[4]]
#' combined <- lapply(areas$textlines, FUN=function(x) image_read(ocv_bitmap(x)))
#' combined <- do.call(c, combined)
#' combined
#' image_append(combined, stack = TRUE)
#' 
#' plt <- image_draw(img)
#' lapply(areas$paths, FUN=function(line){
#'   lines(x = line$x, y = line$y, col = "red")  
#' })
#' dev.off()
#' plt
image_textlines_astar <- function(x, morph = FALSE, step = 2, mfactor = 5, trace = FALSE){
  stopifnot(inherits(x, "magick-image"))
  width  <- image_info(x)$width
  height <- image_info(x)$height
  x <- image_data(x, channels = "gray")
  x <- cvmat_bw(x, width = width, height = height)
  out <- textlinedetector_astarpath(x, morph = morph, step = step, mfactor = mfactor, trace = trace)
  class(out) <- c("textlines", "astarpath")
  out <- lines.textlines(out, x)
  out
}


#' @title Extract the polygons of the textlines
#' @description Extract the polygons of the textlines as a cropped rectangular image containing the image content of the line segmented polygon
#' @param x an object of class \code{textlines} as returned by \code{\link{image_textlines_astar}} or \code{\link{image_textlines_flor}} 
#' @param image an object of class magick-image
#' @param crop extract only the bounding box of the polygon of the text lines
#' @param channels either 'bgr' or 'gray' to work on the colored data or on binary greyscale data
#' @param ... further arguments passed on 
#' @return the object \code{x} where element \code{textlines} is replaced with the extracted polygons of text lines
#' @export
#' @examples 
#' ## See the examples in ?image_textlines_astar or ?image_textlines_flor
lines.textlines <- function(x, image, crop = TRUE, channels = c("bgr", "gray"), ...){
  channels <- match.arg(channels)
  stopifnot(inherits(image, "magick-image") || inherits(image, "opencv-image"))
  if(inherits(image, "magick-image")){
    width   <- image_info(image)$width
    height  <- image_info(image)$height
    if(channels == "bgr"){
      img_bgr <- image_data(image, channels = "bgr")
      img_bgr <- cvmat_bgr(img_bgr, width = width, height = height)  
    }else if(channels == "gray"){
      img_bgr <- image_data(image, channels = "gray")
      img_bgr <- cvmat_bw(img_bgr, width = width, height = height)  
    } 
  }else{
    img_bgr <- image
  }
  if(x$n > 0){
    pts <- c(list(data.frame(ocv_points(img_bgr, type = c("topleft", "topright"))[c("x", "y")])), x$paths,
             list(data.frame(ocv_points(img_bgr, type = c("bottomleft", "bottomright"))[c("x", "y")])))
    for(i in seq_len(length(pts) - 1)){
      a <- pts[[i]]
      b <- pts[[i+1]]
      a <- a[order(a$x, decreasing = FALSE), ]
      b <- b[order(b$x, decreasing = TRUE), ]
      combi <- rbind(a, b)
      combi <- unique(combi)
      x$textlines[[i]] <- cvmat_polygon(img_bgr, pts = combi, crop = crop, convex = FALSE, ...)
    }
  }
  x
}


ocv_points <- function(image, type = c("topleft", "topright", "bottomleft", "bottomright")){
  info <- cvmat_info(image)
  pts <- list(type = c("topleft", "topright", "bottomleft", "bottomright"),
              x = c(0L, info$width-1L, 0L, info$width-1),
              y = c(0L, 0L, info$height-1, info$height-1L))
  idx <- which(pts$type %in% type)
  pts <- list(type = pts$type[idx], x = pts$x[idx], y = pts$y[idx])
  pts
}



