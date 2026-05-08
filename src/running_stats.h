/* class RunningStats
   Get mean and variance in one pass using Welford's online algorithm
   (see https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance)
   Also tracks the min, max, sum and count.
   Chris Toney <chris.toney at usda.gov> */

#ifndef RUNNING_STATS_H_
#define RUNNING_STATS_H_

#include <Rcpp.h>

#include <cstdint>

class RunningStats {
 public:
    RunningStats();
    explicit RunningStats(bool na_rm);

    // read/write field exposed to R
    bool returnCountAsInteger64 {false};

    // public methods exported to R
    void update(const Rcpp::NumericVector& newvalues);
    void reset();
    // NumericVector for count to carry the optional bit64::integer64 payload
    Rcpp::NumericVector get_count() const;
    double get_mean() const;
    double get_min() const;
    double get_max() const;
    double get_sum() const;
    double get_var() const;
    double get_sd() const;

    void show() const;

 private:
    bool m_na_rm;
    // count uses signed int64 for optional return as bit64::integer64
    int64_t m_count;
    double m_mean, m_min, m_max, m_sum;
    double m_M2;
};

// cppcheck-suppress unknownMacro
RCPP_EXPOSED_CLASS(RunningStats)

#endif  // RUNNING_STATS_H_
