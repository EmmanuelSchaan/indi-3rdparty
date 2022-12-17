#pragma once
#ifndef CONFIG_H
#define CONFIG_H

/* Define INDI Data Dir */
#define INDI_DATA_DIR "/usr/share/indi"
/* Define Driver version */
#define STARBOOK_DRIVER_VERSION_MAJOR 0
#define STARBOOK_DRIVER_VERSION_MINOR 8


const char *DEFAULT_STARBOOK_ADDRESS = "169.254.1.1";
const int DEFAULT_STARBOOK_PORT = 80;
const char *STARBOOK_USERAGENT = "indi-starbook/0.8";

#endif // CONFIG_H
