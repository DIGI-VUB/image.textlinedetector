# image.textlinedetector

The  **image.textlinedetector** R package detects text lines in digital images and segments these into words.

Objective of the package is to more easily plug the text lines in Handwritten Text Recognition modelling frameworks like the one explained [in this document](http://www.jpuigcerver.net/pubs/jpuigcerver_icdar2017.pdf) 

The algorithm in this R package follows the following techniques:

- All techniques are explained [in this document](https://github.com/arthurflor23/text-segmentation/blob/master/doc/Text%20Segmentation.pdf)
- The technique for line segmentation is explained in more detail [in this paper](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.88.5806&rep=rep1&type=pdf)


### Installation

- For installing the development version of this package: `remotes::install_github("DIGIT-VUB/image.textlinedetector")`

Look to the documentation of the functions

```
help(package = "image.textlinedetector")
```

## Example

![](https://raw.githubusercontent.com/DIGIT-VUB/image.textlinedetector/master/inst/extdata/example-result.png?raw=true)

```{r}
library(opencv)
library(magick)
library(image.textlinedetector)
path <- system.file(package = "image.textlinedetector", "extdata", "example.png")
x <- image_read(path)

width  <- image_info(x)$width
height <- image_info(x)$height
x   <- image_data(x, channels = "bgr")
img <- image.textlinedetector:::cvmat_bgr(x, width = width, height = height)
img <- textlinedetector_crop(img)
img <- textlinedetector_resize(img)
img <- textlinedetector_binarization(img)
textlines <- textlinedetector_linesegmentation(img)
textlines$n
textlines$imagelines
textwords <- textlinedetector_wordsegmentation(textlines$textlines[[1]])
textwords$n
textwords$words[[1]]
```

### DIGI

By DIGI: Brussels Platform for Digital Humanities: https://digi.research.vub.be

![](tools/logo.png)