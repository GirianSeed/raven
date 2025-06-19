#ifndef COMMON_H
#define COMMON_H

#define MIN(x, a)      (((x) < (a)) ? (x) : (a))
#define MAX(x, a)      (((x) > (a)) ? (x) : (a))
#define CLAMP(x, a, b) (MIN(MAX(x, a), b))

#endif /* COMMON_H */
