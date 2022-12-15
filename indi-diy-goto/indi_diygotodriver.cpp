#include "config.h"
#include "indi_diygotodriver.h"

// We declare an auto pointer to DIYGoToDriver.
static std::unique_ptr<DIYGoToDriver> diygotodriver(new DIYGoToDriver());

DIYGoToDriver::DIYGoToDriver()
{
    setVersion(CDRIVER_VERSION_MAJOR, CDRIVER_VERSION_MINOR);
}

const char *DIYGoToDriver::getDefaultName()
{
    return "DIY Go-To Driver";
}
