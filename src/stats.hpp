
#ifndef _STATS_H_
#define _STATS_H_


#include <cmath>



double stats_mean(const double data[], const size_t size);
double stats_variance(const double data[], const size_t size, const double mean);
double stats_sd(const double variance);
double stats_kurtosis(
    const double data[], const size_t size, const double mean, const double sd
);
double stats_skew(
    const double data[], const size_t size, const double mean, const double sd
);
void stats_min_max(
    double &min, double &max, const double data[], const size_t size
);


#endif

