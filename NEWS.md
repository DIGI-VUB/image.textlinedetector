### CHANGES IN image.textlinedetector VERSION 0.1.1

- Updated SystemRequirements (OpenCV 3 or newer)
- Get rid of cvmat functions which are already part of R package opencv + only link to highgui,imgproc,core (drop linking to the other opencv modules as they were not needed)
- Fix LineSegmentation::sieve as it gave undefined behaviour giving incorrect prime numbers

### CHANGES IN image.textlinedetector VERSION 0.1.0

- Initial version