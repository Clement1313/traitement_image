#include <iostream>

#include <stdio.h>
#include "gaussian_blur.hh"
#include "nl_mean.hh"
#include "image.hh"
#include "image_io.hh"

using namespace std;
using namespace tifo;
using namespace utils;

int main(void)
{
    tifo::rgb24_image* img =
        tifo::load_image("data/20140712_163729.tga");
    if (img == nullptr)
    {
        return 1;
    }
    
    rgb24_image* denoised = nl_mean(*img);
    save_image(*denoised, "nl_mean.tga");

    delete denoised;
    delete img;

    return 0;
}