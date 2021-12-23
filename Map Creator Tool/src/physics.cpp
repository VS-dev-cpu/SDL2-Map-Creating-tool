#include <iostream>
#include <cmath>

using namespace std;

#ifdef __EMSCRIPTEN__
#include "physics.h"
#else
#include "include/physics.h"
#endif

int mapper(long x, long in_min, long in_max, long out_min, long out_max)
{
    return ((x - in_min) * (out_max - out_min) + 0.1) / (in_max - in_min) + out_min;
}

bool isCollide(int x, int y, int xx, int yy, int size_x, int size_y)
{

    if (yy + size_y > y and y > yy)
    {
        if (xx + size_x > x and x > xx)
        {
            return true;
        }
    }
    return false;
}

int roundTo(int roundToNum, int mode, int in_num)
{
    switch (mode)
    {
    case 0:
        float num, dec, numcpy;
        int n, last;

        num = in_num;

        numcpy = num;
        dec = num - floor(num);
        num = numcpy;
        n = floor(num);

        if (n % roundToNum < roundToNum / 2)
        {
            n = (n / roundToNum) * roundToNum;
        }
        else if (n % roundToNum == roundToNum / 2)
        {
            if (dec > 0)
                n = (((n + roundToNum) / roundToNum) * roundToNum);
            else
                n = (n / roundToNum) * roundToNum;
        }
        else
        {
            n = (((n + roundToNum) / roundToNum) * roundToNum);
        }

        return n;
        break;

    case 1:
        int current = in_num;
        while (!(current < roundToNum))
            current -= roundToNum;
        return in_num - current + roundToNum;
        break;
    }
}