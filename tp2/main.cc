#include <iostream>

#include <stdio.h>
#include "gaussian_blur.hh"
#include "nl_mean.hh"
#include "image.hh"
#include "image_io.hh"
#include "selective_gaussian.hh"

using namespace std;
using namespace tifo;
using namespace utils;

int main(void)
{
    tifo::rgb24_image* img =
        tifo::load_image("data/cropped.tga");
    if (img == nullptr)
    {
        return 1;
    }
    
    rgb24_image* denoised = selective_gaussian(*img);
    save_image(*denoised, "selective_gaussian.tga");

    delete denoised;
    delete img;

    return 0;
}