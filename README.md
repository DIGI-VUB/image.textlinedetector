# image.textlinedetector

The  **image.textlinedetector** R package detects text lines in digital images and segments these into words.

Objective of the package is to more easily plug the text lines in Handwritten Text Recognition modelling frameworks like the one explained [in this document](http://www.jpuigcerver.net/pubs/jpuigcerver_icdar2017.pdf) 

The algorithm in this R package follows the following techniques:

1. __An Implementation of a Novel A* Path Planning Algorithm for Line Segmentation of Handwritten Documents__ [paper link](https://github.com/smeucci/LineSegm/blob/master/c%2B%2B/linesegm/docs/relazione.pdf)
2. __A Statistical approach to line segmentation in handwritten documents__ [paper link](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.88.5806&rep=rep1&type=pdf)

More descriptions of technique 2 can be found [in this document](https://github.com/arthurflor23/text-segmentation/blob/master/doc/Text%20Segmentation.pdf)

### Installation

- For installing the development version of this package and to execute the example 

```
install.packages("opencv")
install.packages("magick")
install.packages("image.binarization")
remotes::install_github("DIGI-VUB/image.textlinedetector")
```

Look to the documentation of the functions

```
help(package = "image.textlinedetector")
```

## Example

### Based on the paper `An Implementation of a Novel A* Path Planning Algorithm for Line Segmentation of Handwritten Documents`

![](https://raw.githubusercontent.com/DIGI-VUB/image.textlinedetector/master/inst/extdata/example-result-astar.png)

```
library(opencv)
library(magick)
library(image.binarization)
library(image.textlinedetector)
#path <- "C:/Users/Jan/Desktop/OCR-HTR/RABrugge_TBO119_693_088.jpg"
path  <- system.file(package = "image.textlinedetector", "extdata", "example.png")
img   <- image_read(path)
img   <- image_binarization(img, type = "su")
areas <- image_textlines_astar(img, morph = TRUE, step = 2, mfactor = 5)
areas <- lines(areas, img, channels = "bgr")
areas$n
areas$overview
areas$lines
areas$textlines[[2]]
areas$textlines[[4]]
combined <- lapply(areas$textlines, FUN = function(x) image_read(ocv_bitmap(x)))
combined <- do.call(c, combined)
combined
```

### Based on the paper `A Statistical approach to line segmentation in handwritten documents`

![](https://raw.githubusercontent.com/DIGI-VUB/image.textlinedetector/master/inst/extdata/example-result.png)


```{r}
library(opencv)
library(magick)
library(image.binarization)
library(image.textlinedetector)
path   <- system.file(package = "image.textlinedetector", "extdata", "example.png")
img    <- image_read(path)
img
img_bw <- image_binarization(img, type = "isauvola")
areas  <- image_textlines_flor(img, light = TRUE, type = "sauvola")
areas$overview
areas$textlines[[6]]
areas  <- lines(areas, img_bw, channels = "gray")
textwords <- image_wordsegmentation(areas$textlines[[10]])
textwords$n
textwords$overview
textwords$words[[2]]
textwords$words[[3]]
combined <- lapply(textwords$words, FUN = function(x) image_read(ocv_bitmap(x)))
combined <- do.call(c, combined)
combined
```

### DIGI

By DIGI: Brussels Platform for Digital Humanities: https://digi.research.vub.be

![](tools/logo.png)