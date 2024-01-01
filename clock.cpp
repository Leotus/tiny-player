#include "clock.h"

double get_clock(Clock *c)
{
    double time = av_gettime_relative() / 1000000.0;
    return c->pts_drift + time;
}

void set_clock_at(Clock *c, double pts, double time)
{
    c->pts = pts;
    c->pts_drift = c->pts - time;
}

void set_clock(Clock* c, double pts)
{
    double time = av_gettime_relative() / 1000000.0;
    set_clock_at(c, pts, time);
}

void init_clock(Clock* c)
{
    set_clock(c, NAN);
}
