#include <time.h>

#include "spa.h"

#define TAI_LESS_UTC ((double)37.0) // Defined in set7.dat
#define TIME_CONSTANT ((double)32.184) // Not sure where this comes from.
#define DELTA_T ((double) (TIME_CONSTANT + TAI_LESS_UTC))

int get_gmt_offset()
{
    time_t epoch_plus_11h = 60 * 60 * 11;

    struct tm time_local;
    struct tm time_gmt;

    localtime_r(&epoch_plus_11h, &time_local);
    gmtime_r(&epoch_plus_11h, &time_gmt);

    return time_local.tm_hour - time_gmt.tm_hour;
}

int main()
{
    struct tm time_date;
    // TODO arguments (angle from north -deg west)
    int north_azm_rotation;

    spa_data spa_args;

    time_t now = time(NULL);

    localtime_r(&now, &time_date);

    spa_args.year   = time_date.tm_year + 1900;
    spa_args.month  = time_date.tm_mon + 1; // tm is 0 indexed, spa_data is not.
    spa_args.day    = time_date.tm_mday;
    spa_args.hour   = time_date.tm_hour;
    spa_args.minute = time_date.tm_min;
    spa_args.second = time_date.tm_sec;

    spa_args.delta_t = DELTA_T; // See ser7.dat

    spa_args.timezone = (double)get_gmt_offset();

    spa_args.azm_rotation = (north_azm_rotation + 180) % 360;


    return 0;
}
