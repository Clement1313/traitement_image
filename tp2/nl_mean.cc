#include "nl_mean.hh"

#include <algorithm>
#include <cmath>

namespace
{
    int clamp_int(int value, int low, int high)
    {
        return std::max(low, std::min(value, high));
    }

    uint8_t sample(const tifo::gray8_image& image, int x, int y)
    {
        x = clamp_int(x, 0, image.sx - 1);
        y = clamp_int(y, 0, image.sy - 1);
        return image.pixels[y * image.sx + x];
    }
    double patch_weight(int dx, int dy, int patch_radius)
    {
        const double sigma = std::max(1.0, patch_radius / 2.0);
        const double distance2 = static_cast<double>(dx * dx + dy * dy);
        return std::exp(-distance2 / (2.0 * sigma * sigma));
    }
} // namespace

rgb24_image* nl_mean(rgb24_image& img)
{
    const int patch_radius = 1;
    const int search_radius = 3;
    const double h = 10.0;
    const double noise_sigma = 0.0;

    gray8_image r = gray8_image(img.sx, img.sy);
    gray8_image g = gray8_image(img.sx, img.sy);
    gray8_image b = gray8_image(img.sx, img.sy);
    separate_canal(img, r, g, b);

    gray8_image r_blur = gray8_image(img.sx, img.sy);
    gray8_image g_blur = gray8_image(img.sx, img.sy);
    gray8_image b_blur = gray8_image(img.sx, img.sy);

    double patch_weights[2 * patch_radius + 1][2 * patch_radius + 1];
    for (int py = -patch_radius; py <= patch_radius; py++)
    {
        for (int px = -patch_radius; px <= patch_radius; px++)
        {
            patch_weights[py + patch_radius][px + patch_radius] =
                patch_weight(px, py, patch_radius);
        }
    }

    auto filter_channel = [&](gray8_image& src, gray8_image& dst) {
        for (int y = 0; y < img.sy; y++)
        {
            for (int x = 0; x < img.sx; x++)
            {
                double weighted_sum = 0.0;
                double weight_sum = 0.0;

                for (int sy = -search_radius; sy <= search_radius; sy++)
                {
                    for (int sx = -search_radius; sx <= search_radius; sx++)
                    {
                        const int qx = clamp_int(x + sx, 0, img.sx - 1);
                        const int qy = clamp_int(y + sy, 0, img.sy - 1);

                        double distance2 = 0.0;
                        for (int py = -patch_radius; py <= patch_radius; py++)
                        {
                            for (int px = -patch_radius; px <= patch_radius;
                                 px++)
                            {
                                const int p_x = x + px;
                                const int p_y = y + py;
                                const int q_x = qx + px;
                                const int q_y = qy + py;

                                const double diff =
                                    static_cast<double>(sample(src, p_x, p_y))
                                    - static_cast<double>(
                                        sample(src, q_x, q_y));
                                const double weight =
                                    patch_weights[py + patch_radius]
                                                 [px + patch_radius];
                                distance2 += weight * diff * diff;
                            }
                        }

                        const double bias = 2.0 * noise_sigma * noise_sigma;
                        const double weight = std::exp(
                            -std::max(distance2 - bias, 0.0) / (h * h));
                        weighted_sum += weight * sample(src, qx, qy);
                        weight_sum += weight;
                    }
                }

                dst.pixels[y * img.sx + x] =
                    static_cast<uint8_t>(std::lround(weight_sum / weight_sum));
            }
        }
    };

    filter_channel(r, r_blur);
    filter_channel(g, g_blur);
    filter_channel(b, b_blur);

    return merge_canal(r_blur, g_blur, b_blur);
}
