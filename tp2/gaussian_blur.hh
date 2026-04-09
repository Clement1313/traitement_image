#pragma once

#include "image.hh"

using namespace tifo;

namespace utils
{
    gray8_image* rgb_to_gray(rgb24_image& colored);
    rgb24_image* gray_to_rgb(gray8_image& gray);
    void separate_canal(rgb24_image& base, gray8_image& res_r,
                        gray8_image& res_g, gray8_image& res_b);
    rgb24_image* merge_canal(gray8_image& res_r, gray8_image& res_g,
                             gray8_image& res_b);
} // namespace utils

rgb24_image* gaussian_blur(rgb24_image& img);