#include "selective_gaussian.hh"

#include <algorithm>
#include <cmath>

namespace
{
    int clamp_int(int value, int low, int high)
    {
        return std::max(low, std::min(value, high));
    }

    uint8_t sample(const gray8_image& image, int x, int y)
    {
        x = clamp_int(x, 0, image.sx - 1);
        y = clamp_int(y, 0, image.sy - 1);
        return image.pixels[y * image.sx + x];
    }
} // namespace

rgb24_image* selective_gaussian(rgb24_image& img)
{
    const double sigma_spatial = 2.0;
    const double sigma_range = 25.0;
    const int radius = static_cast<int>(std::ceil(3.0 * sigma_spatial));

    gray8_image r = gray8_image(img.sx, img.sy);
    gray8_image g = gray8_image(img.sx, img.sy);
    gray8_image b = gray8_image(img.sx, img.sy);
    separate_canal(img, r, g, b);

    gray8_image r_blur = gray8_image(img.sx, img.sy);
    gray8_image g_blur = gray8_image(img.sx, img.sy);
    gray8_image b_blur = gray8_image(img.sx, img.sy);

    auto filter_channel = [&](const gray8_image& src, gray8_image& dst) {
        for (int y = 0; y < img.sy; y++)
        {
            for (int x = 0; x < img.sx; x++)
            {
                const double center = static_cast<double>(src.pixels[y * img.sx + x]);

                double weighted_sum = 0.0;
                double weight_sum = 0.0;

                for (int dy = -radius; dy <= radius; dy++)
                {
                    for (int dx = -radius; dx <= radius; dx++)
                    {
                        const int nx = x + dx;
                        const int ny = y + dy;
                        const double neighbor = static_cast<double>(sample(src, nx, ny));

                        const double spatial2 = static_cast<double>(dx * dx + dy * dy);
                        const double range = neighbor - center;

                        const double spatial_weight = std::exp(
                            -spatial2 / (2.0 * sigma_spatial * sigma_spatial));
                        const double range_weight = std::exp(
                            -(range * range) / (2.0 * sigma_range * sigma_range));
                        const double weight = spatial_weight * range_weight;

                        weighted_sum += weight * neighbor;
                        weight_sum += weight;
                    }
                }

                const double filtered =
                    (weight_sum > 0.0) ? (weighted_sum / weight_sum) : center;
                dst.pixels[y * img.sx + x] =
                    static_cast<uint8_t>(std::lround(filtered));
            }
        }
    };

    filter_channel(r, r_blur);
    filter_channel(g, g_blur);
    filter_channel(b, b_blur);

    return merge_canal(r_blur, g_blur, b_blur);
}
