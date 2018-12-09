// The srand2_mix() function is by Jonathan Wright:
// http://stackoverflow.com/a/323302/623622

// The random() function is by Ryan Reich:
// http://stackoverflow.com/a/6852396/623622
// The code was modified a bit.

// The random() fallback is by Mark B:
// http://stackoverflow.com/a/5009006/623622

#include "random.h"
#include <time.h>
#include <windows.h>

unsigned long srand2_mix(unsigned long a, unsigned long b, unsigned long c)
{
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
}

void srand2() {
    unsigned long seed = srand2_mix(
            clock(),
            (unsigned long)time(NULL),
            GetProcessId(GetModuleHandle(NULL)));
    srand(seed);
}

int random(unsigned int min, unsigned int max, int recursion_level)
{
    /* Returns a semi-open interval [min, max) */
    static bool srand_initialized = false;
    if (!srand_initialized) {
        srand_initialized = true;
        srand2();
    }
    if (recursion_level > 100) {
        // A fallback.
        int ret = min + (rand() % (int)(max - min));
        return ret;
    }
    int base_random = rand(); /* in [0, RAND_MAX] */
    if (RAND_MAX == base_random) 
        return random(min, max, recursion_level + 1);
    /* now guaranteed to be in [0, RAND_MAX) */
    int range     = max - min,
        remainder = RAND_MAX % range,
        bucket    = RAND_MAX / range;
    /* There are range buckets, plus one smaller interval
        within remainder of RAND_MAX */
    if (base_random < RAND_MAX - remainder) {
        return min + base_random/bucket;
    } else {
        return random(min, max, recursion_level + 1);
    }
}
