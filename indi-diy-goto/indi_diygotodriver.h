#pragma once

#include "libindi/defaultdevice.h"

class DIYGoToDriver : public INDI::DefaultDevice
{
public:
    DIYGoToDriver();
    virtual ~DIYGoToDriver() = default;

    // You must override this method in your class.
    virtual const char *getDefaultName() override;
};
