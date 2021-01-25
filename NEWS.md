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
