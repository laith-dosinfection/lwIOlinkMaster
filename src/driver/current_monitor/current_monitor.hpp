
namespace lwIOLink::driver
{
class CurrentMonitor
{
public:
    virtual bool getCurrent(float * val) = 0;
};

} // namespace lwIOLink::driver