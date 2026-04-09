#include "gaussian_blur.hh"

#include "image.hh"
#include "image_io.hh"

using namespace tifo;
using namespace utils;
using namespace std;

namespace utils
{
    gray8_image* rgb_to_gray(rgb24_image& colored)
    {
        gray8_image* gray = new gray8_image(colored.sx, colored.sy);

        for (int i = 0; i < colored.sx * colored.sy; i++)
        {
            uint8_t r = colored.pixels[i * 3];
            uint8_t g = colored.pixels[i * 3 + 1];
            uint8_t b = colored.pixels[i * 3 + 2];
            gray->pixels[i] = 0.299 * r + 0.587 * g + 0.114 * b;
        }
        return gray;
    }

    rgb24_image* gray_to_rgb(gray8_image& gray)
    {
        rgb24_image* colored = new rgb24_image(gray.sx, gray.sy);

        for (int i = 0; i < gray.sx * gray.sy; i++)
        {
            colored->pixels[i * 3] = gray.pixels[i];
            colored->pixels[i * 3 + 1] = gray.pixels[i];
            colored->pixels[i * 3 + 2] = gray.pixels[i];
        }

        return colored;
    }

    void separate_canal(rgb24_image& base, gray8_image& res_r,
                        gray8_image& res_g, gray8_image& res_b)
    {
        for (int i = 0; i < base.sx * base.sy; i++)
        {
            res_r.pixels[i] = base.pixels[i * 3];
            res_g.pixels[i] = base.pixels[i * 3 + 1];
            res_b.pixels[i] = base.pixels[i * 3 + 2];
        }
    }

    rgb24_image* merge_canal(gray8_image& res_r, gray8_image& res_g,
                             gray8_image& res_b)
    {
        rgb24_image* merged = new rgb24_image(res_r.sx, res_r.sy);
        for (int i = 0; i < res_r.sx * res_r.sy; i++)
        {
            merged->pixels[i * 3] = res_r.pixels[i];
            merged->pixels[i * 3 + 1] = res_g.pixels[i];
            merged->pixels[i * 3 + 2] = res_b.pixels[i];
        }

        return merged;
    }

} // namespace utils

rgb24_image* gaussian_blur(rgb24_image& img)
{
    gray8_image r = gray8_image(img.sx, img.sy);
    gray8_image g = gray8_image(img.sx, img.sy);
    gray8_image b = gray8_image(img.sx, img.sy);
    separate_canal(img, r, g, b);

    gray8_image r_blur = gray8_image(img.sx, img.sy);
    gray8_image g_blur = gray8_image(img.sx, img.sy);
    gray8_image b_blur = gray8_image(img.sx, img.sy);

    for (int y = 0; y < img.sy; y++) // copie les pixels du bodrs
    {
        for (int x = 0; x < img.sx; x++)
        {
            int idx = y * img.sx + x;
            if (x == 0 || y == 0 || x == img.sx - 1 || y == img.sy - 1)
            {
                r_blur.pixels[idx] = r.pixels[idx];
                g_blur.pixels[idx] = g.pixels[idx];
                b_blur.pixels[idx] = b.pixels[idx];
            }
        }
    }

    for (int y = 1; y < img.sy - 1; y++) // floute les pixel centraux
    {
        for (int x = 1; x < img.sx - 1; x++)
        {
            int idx = y * img.sx + x;

            int up = y - 1;
            int down = y + 1;
            int left = x - 1;
            int right = x + 1;

            /*  noyau de convolution
                1/16 * [1 2 1
                        2 4 2
                        1 2 1]
            */
            int sum_r = r.pixels[up * img.sx + left] + // haut gauche
                2 * r.pixels[up * img.sx + x] + // haut
                r.pixels[up * img.sx + right] + // haut droite
                2 * r.pixels[y * img.sx + left] + // gauche
                4 * r.pixels[idx] + // centre
                2 * r.pixels[y * img.sx + right] + // droite
                r.pixels[down * img.sx + left] + // bas gauche
                2 * r.pixels[down * img.sx + x] + // bas
                r.pixels[down * img.sx + right]; // bas droite

            int sum_g = g.pixels[up * img.sx + left]
                + 2 * g.pixels[up * img.sx + x] + g.pixels[up * img.sx + right]
                + 2 * g.pixels[y * img.sx + left] + 4 * g.pixels[idx]
                + 2 * g.pixels[y * img.sx + right]
                + g.pixels[down * img.sx + left]
                + 2 * g.pixels[down * img.sx + x]
                + g.pixels[down * img.sx + right];

            int sum_b = b.pixels[up * img.sx + left]
                + 2 * b.pixels[up * img.sx + x] + b.pixels[up * img.sx + right]
                + 2 * b.pixels[y * img.sx + left] + 4 * b.pixels[idx]
                + 2 * b.pixels[y * img.sx + right]
                + b.pixels[down * img.sx + left]
                + 2 * b.pixels[down * img.sx + x]
                + b.pixels[down * img.sx + right];

            r_blur.pixels[idx] = static_cast<uint8_t>(sum_r / 16);
            g_blur.pixels[idx] = static_cast<uint8_t>(sum_g / 16);
            b_blur.pixels[idx] = static_cast<uint8_t>(sum_b / 16);
        }
    }

    return merge_canal(r_blur, g_blur, b_blur);
}