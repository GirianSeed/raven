#ifndef _COMMON_H_
#define _COMMON_H_

#define MIN(x, a)       (((x) < (a)) ? (x) : (a))
#define MAX(x, a)       (((x) > (a)) ? (x) : (a))
#define CLAMP(x, a, b)  (MIN(MAX(x, a), b))

#define STEP_SIZE       448
#define SAMPLE_RATE     44100

#endif /* _COMMON_H_ */
