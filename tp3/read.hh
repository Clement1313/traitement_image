#ifndef READ_HH
#define READ_HH

#include <stdio.h>
#include <vector>

using namespace std;

namespace raw
{
    struct pixel
    {
        double r;
        double g;
        double b;

        pixel(double r, double g, double b)
            : r(r)
            , g(g)
            , b(b)
        {}
    };

    class image_raw
    {
    public:
        int sx;
        int sy;
        size_t length;
        vector<double> value;
        vector<pixel> pixels;
        
        image_raw(char* filename);
        pixel find_black();
        void sub_min(pixel mini);
        void demosaicing();
    };

} // namespace raw

#endif // READ_HH