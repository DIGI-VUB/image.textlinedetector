
#' @title Deslant images by putting cursive text upright
#' @description This algorithm sets handwritten text in images upright by removing cursive writing style. 
#' One can use it as a preprocessing step for handwritten text recognition.
#' @param x an object of class opencv-image with pixel values between 0 and 255
#' @param bgcolor integer value with the background color to use to fill the gaps of the sheared image that is returned. Defaults to white: 255
#' @param lower_bound lower bound of shear values. Defaults to -1
#' @param upper_bound upper bound of shear values. Defaults to 1
#' @export 
#' @return an object of class opencv-image with the deslanted image
#' @examples 
#' \dontshow{
#' if(require(opencv)) \{
#' }
#' library(opencv)
#' library(image.textlinedetector)
#' path <- system.file(package = "image.textlinedetector", "extdata", "cursive.png")
#' img  <- ocv_read(path)
#' img  <- ocv_grayscale(img)
#' img
#' up   <- ocv_deslant(img)
#' up
#' \dontshow{
#' \}
#' # End of main if statement running only if the required packages are installed
#' }
ocv_deslant <- function(x, bgcolor = 255, lower_bound = -1, upper_bound = 1){
  stopifnot(inherits(x, "opencv-image"))
  bgcolor <- as.integer(bgcolor)
  textlinedetector_deslant(x, bgcolor, lower_bound, upper_bound)
}
