#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

#include <stack>
#include <iostream>
#include <cstring>
#include <cstdio>

using namespace std;

void reset_buffer(char *, int);
int get_screen_size(int *, int *);
int defragment_json(const char *, char [10][1024]);