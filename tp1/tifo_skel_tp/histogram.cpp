//************************************************
//*                                              *
//*   TP 1&2    (c) 2017 J. FABRIZIO             *
//*                                              *
//*                               LRDE EPITA     *
//*                                              *
//************************************************

#include "histogram.hh"
using namespace std;

namespace tifo {

  histogram_1d compute_histogram(const gray8_image& img) {
    histogram_1d h;
    memset(h.histogram, 0, sizeof(h.histogram));
    
    for (size_t i=0; i < img.length; i++){
            h.histogram[img.pixels[i]]++;
    }
    return h;
  }

  gray8_image* rgb_to_gray(rgb24_image& colored) {
    gray8_image* gray = new gray8_image(colored.sx, colored.sy);

    for (int i=0; i < colored.sx * colored.sy; i++) {
       uint8_t r = colored.pixels[i * 3];
       uint8_t g = colored.pixels[i * 3 + 1];
       uint8_t b = colored.pixels[i * 3 + 2];
       gray->pixels[i] = 0.299 * r + 0.587 * g + 0.114 * b; 
    }
    return gray;
}


rgb24_image* gray_to_rgb(gray8_image& gray) {
    rgb24_image* colored = new rgb24_image(gray.sx, gray.sy);

    for (int i=0; i < gray.sx * gray.sy; i++) {
        colored->pixels[i * 3] = gray.pixels[i];
        colored->pixels[i * 3 + 1] = gray.pixels[i];
        colored->pixels[i * 3 + 2] = gray.pixels[i];
    }

    return colored;
}

histogram_1d cumulated_histogram(histogram_1d hist_1d) {
    histogram_1d cumulated_histogram;
    memset(cumulated_histogram.histogram, 0, sizeof(cumulated_histogram.histogram));
    cumulated_histogram.histogram[0] = hist_1d.histogram[0];
    for (long unsigned int i = 1; i < IMAGE_NB_LEVELS; i++) {
        cumulated_histogram.histogram[i] = cumulated_histogram.histogram[i-1] + hist_1d.histogram[i];
    }
    
    return cumulated_histogram;
}


gray8_image* equalize_histogram(gray8_image& img) {
    cout << "1";
    histogram_1d hist = compute_histogram(img);
    histogram_1d cumulate = cumulated_histogram(hist);
    cout << "2";
    uint8_t b_sup = hist.histogram[0];
    for (long unsigned int i = 1; i < IMAGE_NB_LEVELS; i++) 
    {
        if (b_sup < hist.histogram[i]) {
            b_sup = hist.histogram[i];
        }
    }


    for (long unsigned int x=0; x < img.length; x++) {
        img.pixels[x] = b_sup * cumulate.histogram[img.pixels[x]] / img.length;
    }

    return &img;
}


}
