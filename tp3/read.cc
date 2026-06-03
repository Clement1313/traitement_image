#include "read.hh"

#include <bits/stdc++.h>
#include <limits>
#include <vector>

using namespace raw;

namespace raw
{
    image_raw::image_raw(char* filename)
    {
        sx = 3280;
        sy = 2464;

        std::ifstream input(filename, std::ios::binary);

        std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input),
                                          {});

        for (size_t i = 0; i + 4 < buffer.size(); i += 5)
        {
            unsigned char byte1 = buffer[i];
            unsigned char byte2 = buffer[i + 1];
            unsigned char byte3 = buffer[i + 2];
            unsigned char byte4 = buffer[i + 3];
            unsigned char byte5 = buffer[i + 4];

            uint16_t p1 = (byte1 << 2) | ((byte5 >> 6) & 0x03);
            uint16_t p2 = (byte2 << 2) | ((byte5 >> 4) & 0x03);
            uint16_t p3 = (byte3 << 2) | ((byte5 >> 2) & 0x03);
            uint16_t p4 = (byte4 << 2) | (byte5 & 0x03);

            value.push_back(p1);
            value.push_back(p2);
            value.push_back(p3);
            value.push_back(p4);
        }
    }

    pixel image_raw::find_black()
    {
        double min_r = numeric_limits<double>::max();
        double min_g = numeric_limits<double>::max();
        double min_b = numeric_limits<double>::max();
        for (size_t y = 0; y < sy; y++)
        {
            for (size_t x = 0; x < sx; x++)
            {
                if (x % 2 == 0)
                {
                    if (y % 2 == 0 && min_b > value[y * sx + x])
                    {
                        min_b = value[y * sx + x];
                    }
                    else if (y % 2 == 1 && min_g > value[y * sx + x])
                    {
                        min_g = value[y * sx + x];
                    }
                }
                if (x % 2 == 1)
                {
                    if (y % 2 == 0 && min_g > value[y * sx + x])
                    {
                        min_g = value[y * sx + x];
                    }
                    else if (y % 2 == 1 && min_r > value[y * sx + x])
                    {
                        min_r = value[y * sx + x];
                    }
                }
            }
        }

        return pixel(min_r, min_g, min_b);
    }

    void image_raw::sub_min(pixel mini)
    {
        for (size_t y = 0; y < sy; y++)
        {
            for (size_t x = 0; x < sx; x++)
            {
                if (x % 2 == 0)
                {
                    if (y % 2 == 0)
                    {
                        value[x * sx + y] -= mini.b;
                    }
                    else if (y % 2 == 1)
                    {
                        value[x * sx + y] -= mini.g;
                    }
                }
                if (x % 2 == 1)
                {
                    if (y % 2 == 0)
                    {
                        value[x * sx + y] -= mini.g;
                    }
                    else if (y % 2 == 1)
                    {
                        value[x * sx + y] -= mini.r;
                    }
                }
            }
        }
    }

    void image_raw::demosaicing()
    {
        pixels.resize((size_t)sx * (size_t)sy);
        for (size_t y = 0; y < sy; y++)
        {
            for (size_t x = 0; x < sx; x++)
            {
                if (x % 2 == 0)
                {
                    if (y % 2 == 0) // b
                    {
                        size_t idx = y * sx + x;
                        double b = value[idx];

                        double r_sum = 0;
                        int r_cnt = 0;
                        if (x > 0 && y > 0)
                        {
                            r_sum += value[(y - 1) * sx + (x - 1)];
                            ++r_cnt;
                        }
                        if (x + 1 < (size_t)sx && y > 0)
                        {
                            r_sum += value[(y - 1) * sx + (x + 1)];
                            ++r_cnt;
                        }
                        if (x > 0 && y + 1 < (size_t)sy)
                        {
                            r_sum += value[(y + 1) * sx + (x - 1)];
                            ++r_cnt;
                        }
                        if (x + 1 < (size_t)sx && y + 1 < (size_t)sy)
                        {
                            r_sum += value[(y + 1) * sx + (x + 1)];
                            ++r_cnt;
                        }
                        double r = r_cnt ? (r_sum / r_cnt) : 0.0;

                        double g_sum = 0;
                        int g_cnt = 0;
                        if (y > 0)
                        {
                            g_sum += value[(y - 1) * sx + x];
                            ++g_cnt;
                        }
                        if (y + 1 < (size_t)sy)
                        {
                            g_sum += value[(y + 1) * sx + x];
                            ++g_cnt;
                        }
                        if (x > 0)
                        {
                            g_sum += value[y * sx + (x - 1)];
                            ++g_cnt;
                        }
                        if (x + 1 < (size_t)sx)
                        {
                            g_sum += value[y * sx + (x + 1)];
                            ++g_cnt;
                        }
                        double g = g_cnt ? (g_sum / g_cnt) : 0.0;

                        pixels[idx] = pixel(r, g, b);
                    }
                    else if (y % 2 == 1) // g
                    {
                        size_t idx = y * sx + x;
                        double g = value[idx];

                        double r_sum = 0;
                        int r_cnt = 0;
                        if (x > 0)
                        {
                            r_sum += value[y * sx + (x - 1)];
                            ++r_cnt;
                        }
                        if (x + 1 < (size_t)sx)
                        {
                            r_sum += value[y * sx + (x + 1)];
                            ++r_cnt;
                        }
                        double r = r_cnt ? (r_sum / r_cnt) : 0.0;

                        double b_sum = 0;
                        int b_cnt = 0;
                        if (y > 0)
                        {
                            b_sum += value[(y - 1) * sx + x];
                            ++b_cnt;
                        }
                        if (y + 1 < (size_t)sy)
                        {
                            b_sum += value[(y + 1) * sx + x];
                            ++b_cnt;
                        }
                        double b = b_cnt ? (b_sum / b_cnt) : 0.0;

                        pixels[idx] = pixel(r, g, b);
                    }
                }
                if (x % 2 == 1)
                {
                    if (y % 2 == 0) // g
                    {
                        size_t idx = y * sx + x;
                        double g = value[idx];

                        double b_sum = 0;
                        int b_cnt = 0;
                        if (x > 0)
                        {
                            b_sum += value[y * sx + (x - 1)];
                            ++b_cnt;
                        }
                        if (x + 1 < (size_t)sx)
                        {
                            b_sum += value[y * sx + (x + 1)];
                            ++b_cnt;
                        }
                        double b = b_cnt ? (b_sum / b_cnt) : 0.0;

                        double r_sum = 0;
                        int r_cnt = 0;
                        if (y > 0)
                        {
                            r_sum += value[(y - 1) * sx + x];
                            ++r_cnt;
                        }
                        if (y + 1 < (size_t)sy)
                        {
                            r_sum += value[(y + 1) * sx + x];
                            ++r_cnt;
                        }
                        double r = r_cnt ? (r_sum / r_cnt) : 0.0;

                        pixels[idx] = pixel(r, g, b);
                    }
                    else if (y % 2 == 1) // r
                    {
                        size_t idx = y * sx + x;
                        double r = value[idx];

                        double g_sum = 0;
                        int g_cnt = 0;
                        if (y > 0)
                        {
                            g_sum += value[(y - 1) * sx + x];
                            ++g_cnt;
                        }
                        if (y + 1 < (size_t)sy)
                        {
                            g_sum += value[(y + 1) * sx + x];
                            ++g_cnt;
                        }
                        if (x > 0)
                        {
                            g_sum += value[y * sx + (x - 1)];
                            ++g_cnt;
                        }
                        if (x + 1 < (size_t)sx)
                        {
                            g_sum += value[y * sx + (x + 1)];
                            ++g_cnt;
                        }
                        double g = g_cnt ? (g_sum / g_cnt) : 0.0;

                        double b_sum = 0;
                        int b_cnt = 0;
                        if (x > 0 && y > 0)
                        {
                            b_sum += value[(y - 1) * sx + (x - 1)];
                            ++b_cnt;
                        }
                        if (x + 1 < (size_t)sx && y > 0)
                        {
                            b_sum += value[(y - 1) * sx + (x + 1)];
                            ++b_cnt;
                        }
                        if (x > 0 && y + 1 < (size_t)sy)
                        {
                            b_sum += value[(y + 1) * sx + (x - 1)];
                            ++b_cnt;
                        }
                        if (x + 1 < (size_t)sx && y + 1 < (size_t)sy)
                        {
                            b_sum += value[(y + 1) * sx + (x + 1)];
                            ++b_cnt;
                        }
                        double b = b_cnt ? (b_sum / b_cnt) : 0.0;

                        pixels[idx] = pixel(r, g, b);
                    }
                }
            }
        }
    }

} // namespace raw