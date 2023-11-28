#pragma once

namespace lwIOLink::driver{
// Interface for the high-side driver
class HighSide {
public:
    virtual bool setDO(bool val) = 0;
};
} // namespace lwIOLink::driver

