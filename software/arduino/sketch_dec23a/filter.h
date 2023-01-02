#ifndef _FILTER_H_
#define _FILTER_H_

/*
 * Simple Kalman filter.
 */
struct Filter
{
  float err_m;
  float err_e;
  float q;

  float curr_e;
  float last_e;
  float gain;
};

/*
 * Create filter.
 */
static inline struct Filter
FilterCreate(float m, float e, float q)
{
  Filter filter;

  filter.err_m = m;
  filter.err_e = e;
  filter.q = q;

  return filter;
}

/*
 * Update filter with new measurement.
 */
static inline float
FilterUpdate(Filter *filter, float m)
{
  filter->gain = filter->err_e / (filter->err_e + filter->err_m);
  filter->curr_e = filter->last_e + filter->gain * (m - filter->last_e);
  filter->err_e = (1.0f - filter->gain) * filter->err_e + fabs(filter->last_e - filter->curr_e) * filter->q;
  filter->last_e = filter->curr_e;

  return filter->curr_e;
}

#endif /* _FILTER_H_ */
