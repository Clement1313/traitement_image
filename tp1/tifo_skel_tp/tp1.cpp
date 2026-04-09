#include <iostream>
#include <string>

#include "histogram.hh"
#include "image.hh"
#include "image_io.hh"

using namespace std;
using namespace tifo;
int main()
{
    tifo::rgb24_image* img =
        tifo::load_image("../tifo_tp1_data/tp1/20140326_124555.tga");

    // // gray histogram
    // tifo::gray8_image* gray = rgb_to_gray(*img);
    // // tifo::histogram_1d hist = compute_histogram(*gray);
    // tifo::rgb24_image* colored_gray = gray_to_rgb(*gray);
    // tifo::save_image(*colored_gray, "gray.tga");

    // tifo::gray8_image* equalized = tifo::equalize_histogram(*gray);
    // tifo::rgb24_image* colored_gray2 = gray_to_rgb(*equalized);
    // tifo::save_image(*colored_gray2, "equalized.tga");

    // rgb histogram
    rgb24_image rgb_histo = rgb_histogram(*img);
    save_image(rgb_histo, "rgb_histo.tga");

    // // hsv histogram (equalization on V channel)
    // rgb24_image hsv_histo = hsv_histogram(*img);
    // save_image(hsv_histo, "hsv_histo.tga");
}