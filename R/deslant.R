
#' @title Deslant images by putting cursive text upright
#' @description This algorithm sets handwritten text in images upright by removing cursive writing style. 
#' One can use it as a preprocessing step for handwritten text recognition.\cr
#' \itemize{
#' \item{\code{image_deslant} expects a magick-image and performs grayscaling before doing deslanting}
#' \item{\code{ocv_deslant} expects a ocv-image and does not perform grayscaling before doing deslanting}
#' }
#' @param image an object of class opencv-image (for \code{ocv_deslant}) with pixel values between 0 and 255 or a magick-image (for \code{image_deslant})
#' @param bgcolor integer value with the background color to use to fill the gaps of the sheared image that is returned. Defaults to white: 255
#' @param lower_bound lower bound of shear values. Defaults to -1
#' @param upper_bound upper bound of shear values. Defaults to 1
#' @export 
#' @return an object of class opencv-image or magick-image with the deslanted image
#' @examples 
#' \dontshow{
#' if(require(opencv)) \{
#' }
#' library(magick)
#' library(opencv)
#' library(image.textlinedetector)
#' path <- system.file(package = "image.textlinedetector", "extdata", "cursive.png")
#' img  <- ocv_read(path)
#' img  <- ocv_grayscale(img)
#' img
#' up   <- ocv_deslant(img)
#' up
#' 
#' img  <- image_read(path)
#' img
#' image_deslant(img)
#' \dontshow{
#' \}
#' # End of main if statement running only if the required packages are installed
#' }
ocv_deslant <- function(image, bgcolor = 255, lower_bound = -1, upper_bound = 1){
  stopifnot(inherits(image, "opencv-image"))
  bgcolor <- as.integer(bgcolor)
  textlinedetector_deslant(image, bgcolor, lower_bound, upper_bound)
}

#' @export
#' @rdname ocv_deslant
image_deslant <- function(image, bgcolor = 255, lower_bound = -1, upper_bound = 1){
  stopifnot(inherits(image, "magick-image"))
  img <- magick_to_opencv(image)
  img <- opencv::ocv_grayscale(img)
  deslanted <- ocv_deslant(img, bgcolor, lower_bound, upper_bound)
  opencv_to_magick(deslanted)
}



magick_to_opencv <- function(img, ...){
  p <- tempfile()
  on.exit({
    if(file.exists(p)){
      file.remove(p) 
    }
  })
  magick::image_write(img, path = p, ...)
  image <- opencv::ocv_read(p)
  image
}

opencv_to_magick <- function(img){
  magick::image_read(opencv::ocv_bitmap(img))
}