//************************************************
//*                                              *
//*   TP 1&2    (c) 2017 J. FABRIZIO             *
//*                                              *
//*                               LRDE EPITA     *
//*                                              *
//************************************************

#include "histogram.hh"

#include <cmath>

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
            img.pixels[x] =
                static_cast<uint8_t>(((cdf - cdf_min) * (IMAGE_NB_LEVELS - 1))
                                     / (img.length - cdf_min));
        }

        return &img;
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

    rgb24_image merge_canal(gray8_image& res_r, gray8_image& res_g,
                            gray8_image& res_b)
    {
        rgb24_image merged = rgb24_image(res_r.sx, res_r.sy);
        for (int i = 0; i < res_r.sx * res_r.sy; i++)
        {
            merged.pixels[i * 3] = res_r.pixels[i];
            merged.pixels[i * 3 + 1] = res_g.pixels[i];
            merged.pixels[i * 3 + 2] = res_b.pixels[i];
        }

        return merged;
    }

    rgb24_image rgb_histogram(rgb24_image& img)
    {
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

    hsv rgb_to_hsv(uint8_t r, uint8_t g, uint8_t b)
    {
        const double rf = static_cast<double>(r) / 255.0;
        const double gf = static_cast<double>(g) / 255.0;
        const double bf = static_cast<double>(b) / 255.0;

        const double cmax = std::max({ rf, gf, bf });
        const double cmin = std::min({ rf, gf, bf });
        const double delta = cmax - cmin;

        double h_deg = 0.0;
        if (delta != 0.0)
        {
            if (cmax == rf)
                h_deg = 60.0 * std::fmod(((gf - bf) / delta), 6.0);
            else if (cmax == gf)
                h_deg = 60.0 * (((bf - rf) / delta) + 2.0);
            else
                h_deg = 60.0 * (((rf - gf) / delta) + 4.0);
        }
        if (h_deg < 0.0)
            h_deg += 360.0;

        const double s = (cmax == 0.0) ? 0.0 : (delta / cmax);
        const double v = cmax;

        const uint8_t h8 = static_cast<uint8_t>(std::round((h_deg / 360.0) * 255.0));
        const uint8_t s8 = static_cast<uint8_t>(std::round(s * 255.0));
        const uint8_t v8 = static_cast<uint8_t>(std::round(v * 255.0));
        return hsv(h8, s8, v8);
    }

    void hsv_to_rgb(uint8_t h8, uint8_t s8, uint8_t v8, uint8_t& r, uint8_t& g,
                    uint8_t& b)
    {
        const double h_deg = (static_cast<double>(h8) / 255.0) * 360.0;
        const double s = static_cast<double>(s8) / 255.0;
        const double v = static_cast<double>(v8) / 255.0;

        const double c = v * s;
        const double h_sector = h_deg / 60.0;
        const double x = c * (1.0 - std::abs(std::fmod(h_sector, 2.0) - 1.0));
        const double m = v - c;

        double rp = 0.0;
        double gp = 0.0;
        double bp = 0.0;

        if (h_sector >= 0.0 && h_sector < 1.0)
        {
            rp = c;
            gp = x;
        }
        else if (h_sector < 2.0)
        {
            rp = x;
            gp = c;
        }
        else if (h_sector < 3.0)
        {
            gp = c;
            bp = x;
        }
        else if (h_sector < 4.0)
        {
            gp = x;
            bp = c;
        }
        else if (h_sector < 5.0)
        {
            rp = x;
            bp = c;
        }
        else
        {
            rp = c;
            bp = x;
        }

        r = static_cast<uint8_t>(std::round((rp + m) * 255.0));
        g = static_cast<uint8_t>(std::round((gp + m) * 255.0));
        b = static_cast<uint8_t>(std::round((bp + m) * 255.0));
    }

    rgb24_image hsv_histogram(rgb24_image& img)
    {
        gray8_image h = gray8_image(img.sx, img.sy);
        gray8_image s = gray8_image(img.sx, img.sy);
        gray8_image v = gray8_image(img.sx, img.sy);

        for (int i = 0; i < img.sx * img.sy; i++)
        {
            hsv hsv_value = rgb_to_hsv(img.pixels[i * 3], img.pixels[i * 3 + 1],
                                       img.pixels[i * 3 + 2]);
            h.pixels[i] = hsv_value.h;
            s.pixels[i] = hsv_value.s;
            v.pixels[i] = hsv_value.v;
        }

        equalize_histogram(v);

        rgb24_image res = rgb24_image(img.sx, img.sy);
        for (int i = 0; i < img.sx * img.sy; i++)
        {
            uint8_t r, g, b;
            hsv_to_rgb(h.pixels[i], s.pixels[i], v.pixels[i], r, g, b);
            res.pixels[i * 3] = r;
            res.pixels[i * 3 + 1] = g;
            res.pixels[i * 3 + 2] = b;
        }

        return res;
    }
} // namespace tifo
