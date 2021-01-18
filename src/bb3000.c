#include <ctype.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "spa.h"

#define TAI_LESS_UTC ((double)37.0) // Defined in set7.dat
#define TIME_CONSTANT ((double)32.184) // Not sure where this comes from.
#define DELTA_T ((double) (TIME_CONSTANT + TAI_LESS_UTC))

#define DEFAULT_PRESSURE ((double) 1013.25)
#define DEFAULT_ELEVATION ((double) 0.0)
#define DEFAULT_TEMPURATURE ((double) 20.0)
#define DEFAULT_SLOPE ((double) 0.0)
#define DEFAULT_REFRACT ((double) 0.567)

struct bb3000_args {
    double norm_azm_rotation;
    double latitude;
    double longitude;
    double elevation;
    double pressure;
    double temperature;
    double slope;
    double refract;
};

static int get_double(const char *arg_name, const char *str, double *d);
static int get_gmt_offset();
static void parse_args(struct bb3000_args *args, int argc, char *argv[]);
static void usage(int exit_code, const char *name);

int main(int argc, char *argv[])
{
    struct bb3000_args bb_args;
    struct tm time_date;
    int spa_result = 0;

    spa_data spa_args;
    memset(&spa_args, 0, sizeof spa_args);

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

    parse_args(&bb_args, argc, argv);

    printf("Timezone: %d:%d:%d\n", spa_args.hour, spa_args.minute, spa_args.second);

    spa_args.longitude = bb_args.longitude;
    spa_args.latitude = bb_args.latitude;
    spa_args.elevation = bb_args.elevation;
    spa_args.pressure = bb_args.pressure;
    spa_args.temperature = bb_args.temperature;
    spa_args.slope = bb_args.slope;
    spa_args.azm_rotation = ((int)(bb_args.norm_azm_rotation + 180.0)) % 360;
    spa_args.atmos_refract = bb_args.refract;
    spa_args.function = SPA_ZA;

    if ((spa_result = spa_calculate(&spa_args)))
    {
        fprintf(stderr, "SPA Failed: %d\n", spa_result);
    }
    else
    {
        // Zenith is angle from the ground.
        // Azimuth is the angle from north.
        printf("Zenith: %f\nAzimuth: %f\n", spa_args.zenith, spa_args.azimuth);
    }

    return 0;
}

static int get_gmt_offset()
{
    time_t epoch_plus_11h = 60 * 60 * 11;

    struct tm time_local;
    struct tm time_gmt;

    localtime_r(&epoch_plus_11h, &time_local);
    gmtime_r(&epoch_plus_11h, &time_gmt);

    return time_local.tm_hour - time_gmt.tm_hour;
}

static int get_double(const char *arg_name, const char *str, double *d)
{
    errno = 0;
    *d = strtod(str, NULL);

    if (errno) {
        perror(arg_name);
        return 1;
    }

    return 0;
}

#define OPT_SPEC "a:u:g:e:p:t:s:r:h"
static void parse_args(struct bb3000_args *args, int argc, char *argv[])
{
    int c;
    extern char *optarg;
    extern int optind, optopt;

    errno = 0;

    while ((c = getopt(argc, argv, OPT_SPEC)) != -1)
    {
        switch(c) {
        case 'h':
            usage(0, *argv); // No return
            break;
        case 'a':
            if (get_double("Azimuth Rotation", optarg, &args->norm_azm_rotation))
            {
                exit(1);
            }
            break;
        case 'u':
            if (get_double("Latitude", optarg, &args->latitude))
            {
                exit(2);
            }
            break;
        case 'g':
            if (get_double("Longitude", optarg, &args->longitude))
            {
                exit(3);
            }
            break;
        case 'e':
            if (get_double("Elevation", optarg, &args->elevation))
            {
                exit(4);
            }
            break;
        case 'p':
            if (get_double("Pressure", optarg, &args->pressure))
            {
                exit(5);
            }
            break;
        case 't':
            if (get_double("Temperature", optarg, &args->temperature))
            {
                exit(6);
            }
            break;
        case 's':
            if (get_double("Slope", optarg, &args->slope))
            {
                exit(7);
            }
            break;
        case 'r':
            if (get_double("Atmospheric Refraction", optarg, &args->refract))
            {
                exit(8);
            }
        case ':':
            // Check for remaining optionals
            switch(optopt) {
            case 'e':
                args->elevation = DEFAULT_ELEVATION;
                break;
            case 'p':
                args->pressure = DEFAULT_PRESSURE;
                break;
            case 't':
                args->temperature = DEFAULT_TEMPURATURE;
                break;
            case 's':
                args->slope = DEFAULT_SLOPE;
                break;
            case 'r':
                args->refract = DEFAULT_REFRACT;
                break;
            default:
                if (isprint(optopt))
                {
                    fprintf(stderr, "-%c requires operand\n", optopt);
                }
                else
                {
                    fprintf(stderr, "Flag requires operand\n");
                }
                exit(9);
            }
            break;
        default:
            usage(15, *argv); // No return.
        }

    }

}

static void usage(int exit_code, const char *name)
{
    fprintf(
        stderr,
        "Usage: %s -a <rotation from north> -u <latitude> -g <longitude> [-e <elevation>]\n"
        "    [-p <pressure>] [-t <temperature>] [-s <slope>] [-h]\n"
        "Options:\n"
        "\t-a <rotation>   \tDirection facing. North is 0 degrees, positive is east.\n"
        "\t-u <latitude>   \tLatitude of observer\n"
        "\t-g <longitude>  \tLongitude of observer\n"
        "\t-e <elevation>  \tElevation of observer in meters\n"
        "\t-p <pressure>   \tAtmospheric pressure in millibars\n"
        "\t-t <temperature>\tTemperature in celcius\n"
        "\t-s <slope>      \tSlope of group\n"
        "\t-h              \tPrints this message\n",
        name);

    exit(exit_code);
}
