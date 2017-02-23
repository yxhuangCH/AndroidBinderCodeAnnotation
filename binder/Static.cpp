#include <private/binder/Static.h>
#include <binder/IPCThreadState.h>
#include <utils/Log.h>
namespace android {
// ------------ ProcessState.cpp
Mutex gProcessMutex;
sp<ProcessState> gProcess;
class LibUtilsIPCtStatics
{
public:
    LibUtilsIPCtStatics()
    {
    }
    
    ~LibUtilsIPCtStatics()
    {
        IPCThreadState::shutdown();
    }
};
static LibUtilsIPCtStatics gIPCStatics;
// ------------ ServiceManager.cpp
Mutex gDefaultServiceManagerLock;
sp<IServiceManager> gDefaultServiceManager;
sp<IPermissionController> gPermissionController;
}   // namespace android