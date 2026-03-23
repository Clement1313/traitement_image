#include "image.hh"
#include "image_io.hh"
#include "histogram.hh"
#include "histogram.hh"

#include <iostream>
#include <string>

using namespace std;

int main ()
{
    tifo::rgb24_image* img = tifo::load_image("../tifo_tp1_data/tp1/20140326_124555.tga");
    tifo::gray8_image* gray = rgb_to_gray(*img);
    // tifo::histogram_1d hist = compute_histogram(*gray);
    // for (int i=0; i < 255; i++)
    // {
    //     cout << hist.histogram[i] << ";";
    // }
    tifo::rgb24_image* colored_gray = gray_to_rgb(*gray);
    tifo::save_image(*colored_gray, "gray.tga");

    tifo::gray8_image* equalized = tifo::equalize_histogram(*gray);
    tifo::rgb24_image* colored_gray2 = gray_to_rgb(*equalized);
    
    tifo::save_image(*colored_gray2, "equalized.tga");
}