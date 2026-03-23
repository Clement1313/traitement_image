//************************************************
//*                                              *
//*   TP 1&2    (c) 2017 J. FABRIZIO             *
//*                                              *
//*                               LRDE EPITA     *
//*                                              *
//************************************************

#include "histogram.hh"
using namespace std;

namespace tifo
{

    histogram_1d compute_histogram(const gray8_image& img)
    {
        histogram_1d h;
        memset(h.histogram, 0, sizeof(h.histogram));

        for (size_t i = 0; i < img.length; i++)
        {
            h.histogram[img.pixels[i]]++;
        }
        return h;
    }

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

    histogram_1d cumulated_histogram(histogram_1d hist_1d)
    {
        histogram_1d cumulated_histogram;
        memset(cumulated_histogram.histogram, 0,
               sizeof(cumulated_histogram.histogram));
        cumulated_histogram.histogram[0] = hist_1d.histogram[0];
        for (long unsigned int i = 1; i < IMAGE_NB_LEVELS; i++)
        {
            cumulated_histogram.histogram[i] =
                cumulated_histogram.histogram[i - 1] + hist_1d.histogram[i];
        }

        return cumulated_histogram;
    }

    gray8_image* equalize_histogram(gray8_image& img)
    {
        histogram_1d hist = compute_histogram(img);
        histogram_1d cumulate = cumulated_histogram(hist);

        unsigned int cdf_min = 0;
        bool found = false;
        for (long unsigned int i = 0; i < IMAGE_NB_LEVELS; i++)
        {
            if (cumulate.histogram[i] != 0)
            {
                cdf_min = cumulate.histogram[i];
                found = true;
                break;
            }
        }

        if (!found || img.length == cdf_min)
        {
            return &img;
        }

        for (long unsigned int x = 0; x < img.length; x++)
        {
            unsigned int cdf = cumulate.histogram[img.pixels[x]];
            img.pixels[x] = static_cast<uint8_t>(
                ((cdf - cdf_min) * (IMAGE_NB_LEVELS - 1)) / (img.length - cdf_min));
        }

        return &img;
    }

      void separate_canal(rgb24_image& base, gray8_image& res_r, gray8_image& res_g, gray8_image& res_b) {
        
        for (int i = 0; i < base.sx * base.sy; i++) {
            res_r.pixels[i] = base.pixels[i * 3];
            res_g.pixels[i] = base.pixels[i * 3 + 1];
            res_b.pixels[i] = base.pixels[i * 3 + 2];

        }
      }

    rgb24_image merge_canal(gray8_image& res_r, gray8_image& res_g, gray8_image& res_b){
        rgb24_image merged = rgb24_image(res_r.sx, res_r.sy);
        for (int i = 0; i < res_r.sx * res_r.sy; i++) {
            merged.pixels[i * 3] = res_r.pixels[i];
            merged.pixels[i * 3 + 1] = res_g.pixels[i];
            merged.pixels[i * 3 + 2] = res_b.pixels[i];
        }

        return merged;
    }

    rgb24_image rgb_histogram(rgb24_image& img) {
        gray8_image r = gray8_image(img.sx, img.sy);
        gray8_image g = gray8_image(img.sx, img.sy);
        gray8_image b = gray8_image(img.sx, img.sy);
        separate_canal(img, r, g, b);

        equalize_histogram(r);
        equalize_histogram(g);
        equalize_histogram(b);

        rgb24_image res = merge_canal(r, g, b);
        return res;
    }



} // namespace tifo
