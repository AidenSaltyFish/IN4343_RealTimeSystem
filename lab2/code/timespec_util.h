#pragma once

const long NS_PER_SECOND = 1000000000L;

struct timespec timespec_sub(const struct timespec t1, const struct timespec t2)
{
    struct timespec t;

    t.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    t.tv_sec  = t2.tv_sec - t1.tv_sec;

    if (t.tv_nsec < 0) {
        t.tv_nsec += NS_PER_SECOND;
        t.tv_sec--;
    }
    return t;
}

struct timespec timespec_add(const struct timespec t1, const struct timespec t2)
{
    struct timespec t = { t1.tv_sec + t2.tv_sec, t1.tv_nsec + t2.tv_nsec };
    if (t.tv_nsec >= NS_PER_SECOND) {
        t.tv_nsec -= NS_PER_SECOND;
        t.tv_sec++;
    }
    return t;
}

struct timespec timespec_divide(struct timespec t, const int n)
{
    time_t remainder_secs = t.tv_sec % n;
    t.tv_sec /= n;
    t.tv_nsec /= n;
    t.tv_nsec +=
            remainder_secs * (NS_PER_SECOND / n) +
            remainder_secs * (NS_PER_SECOND % n) / n;

    while (t.tv_nsec >= NS_PER_SECOND) {
        t.tv_nsec -= NS_PER_SECOND;
        t.tv_sec++;
    }
    return t;
}