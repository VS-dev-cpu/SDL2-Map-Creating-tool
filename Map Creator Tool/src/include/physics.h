#pragma once

#include <iostream>
#include <cmath>

using namespace std;

int mapper(long x, long in_min, long in_max, long out_min, long out_max);

int random(int max);

bool isCollide(int x, int y, int xx, int yy, int size_x, int size_y);

int roundTo(int roundToNum, int mode, int in_num);