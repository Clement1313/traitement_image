//************************************************
//*                                              *
//*   TP 1&2    (c) 2017 J. FABRIZIO             *
//*                                              *
//*                               LRDE EPITA     *
//*                                              *
//************************************************

#ifndef HISTOGRAM_HH
#define	HISTOGRAM_HH

#include "image.hh"

#include <bits/stdc++.h>
#include "image_io.hh"


namespace tifo {
  //https://stackoverflow.com/questions/10747810/what-is-the-difference-between-typedef-and-using
  typedef struct { unsigned int histogram[IMAGE_NB_LEVELS]; } histogram_1d;

  histogram_1d compute_histogram(const gray8_image& img);
  void equalize_histogram(histogram_1d hist);
  gray8_image* rgb_to_gray(rgb24_image& colored);
  rgb24_image* gray_to_rgb(gray8_image& gray);
  gray8_image* equalize_histogram(gray8_image& img);
}

#endif
