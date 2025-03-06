### CHANGES IN text.alignment VERSION 0.2.4

- Windows: use opencv from Rtools if available on R > 4.5

### CHANGES IN text.alignment VERSION 0.2.3

- Windows: update to opencv 4.8.1 with support for arm64.
- Wrap examples in donttest to avoid timing issues with magick on cran debian

### CHANGES IN text.alignment VERSION 0.2.2

- Compile with CXX17 when available to fix compiler warnings
  
### CHANGES IN text.alignment VERSION 0.2.1

- Change of maintainer email address

### CHANGES IN image.textlinedetector VERSION 0.2.0

- Added ocv_deslant / image_deslant

### CHANGES IN image.textlinedetector VERSION 0.1.5

- Makevars fix for Windows UCRT toolchain

### CHANGES IN image.textlinedetector VERSION 0.1.4

- Remove LazyData from DESCRIPTION as there is no data to be lazy about
- Put example with image_draw in donttest block

### CHANGES IN image.textlinedetector VERSION 0.1.3

- Update configure script, using new autobrew location https://autobrew.github.io/scripts/opencv, using sh instead of bash
- Windows uses now opencv 4.4.0
- Fix for R CMD check valgrind message

### CHANGES IN image.textlinedetector VERSION 0.1.2

- Use C++11 instead of C++17 in Makevars.win

### CHANGES IN image.textlinedetector VERSION 0.1.1

- Updated SystemRequirements (OpenCV 3 or newer)
- Get rid of cvmat functions which are already part of R package opencv + only link to highgui,imgproc,core (drop linking to the other opencv modules as they were not needed)
- Fix LineSegmentation::sieve as it gave undefined behaviour giving incorrect prime numbers
- Fix signed integer overflow in rcpp-linelocalization.cpp:296 during hashing of 2 integers (x/y locations)

### CHANGES IN image.textlinedetector VERSION 0.1.0

- Initial version
