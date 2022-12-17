#ifndef CONFIG_H
#define CONFIG_H

/* Define INDI Data Dir */
#define INDI_DATA_DIR "/usr/share/indi"

/* Define Driver version */
#define INDI_GPHOTO_VERSION_MAJOR 3
#define INDI_GPHOTO_VERSION_MINOR 2

/* Define LibRaw temperature struct members available */
/* #undef LIBRAW_CAMERA_TEMPERATURE */
/* #undef LIBRAW_SENSOR_TEMPERATURE */

#define LIBRAW_CAMERA_TEMPERATURE2 1
#define LIBRAW_SENSOR_TEMPERATURE2 1

#endif // CONFIG_H
