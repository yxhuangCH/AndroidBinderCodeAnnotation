#include <binder/IInterface.h>
namespace android {
// ---------------------------------------------------------------------------
IInterface::IInterface() 
    : RefBase() {
}
IInterface::~IInterface() {
}
sp<IBinder> IInterface::asBinder()
{
    return this ? onAsBinder() : NULL;
}
sp<const IBinder> IInterface::asBinder() const
{
    return this ? const_cast<IInterface*>(this)->onAsBinder() : NULL;
}
// ---------------------------------------------------------------------------
}; // namespace android