//#define LOG_NDEBUG 0
#define LOG_TAG "IMediaDeathNotifier"
#include <utils/Log.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <media/IMediaDeathNotifier.h>
namespace android {
// client singleton for binder interface to services
Mutex IMediaDeathNotifier::sServiceLock;
sp<IMediaPlayerService> IMediaDeathNotifier::sMediaPlayerService;
sp<IMediaDeathNotifier::DeathNotifier> IMediaDeathNotifier::sDeathNotifier;
SortedVector< wp<IMediaDeathNotifier> > IMediaDeathNotifier::sObitRecipients;
// establish binder interface to MediaPlayerService
/*static*/const sp<IMediaPlayerService>&
IMediaDeathNotifier::getMediaPlayerService()
{
    ALOGV("getMediaPlayerService");
    Mutex::Autolock _l(sServiceLock);
    if (sMediaPlayerService == 0) {
        // defaultServiceManager 是 IServiceManager
        sp<IServiceManager> sm = defaultServiceManager();
        sp<IBinder> binder;
        do {
            // 向 ServiceManager 查询对应服务的信息
            binder = sm->getService(String16("media.player"));
            if (binder != 0) {
                break;
            }
            ALOGW("Media player service not published, waiting...");
            usleep(500000); // 0.5 s
        } while (true);
        if (sDeathNotifier == NULL) {
        sDeathNotifier = new DeathNotifier();
    }
    binder->linkToDeath(sDeathNotifier);
    sMediaPlayerService = interface_cast<IMediaPlayerService>(binder);
    }
    ALOGE_IF(sMediaPlayerService == 0, "no media player service!?");
    return sMediaPlayerService;
}
/*static*/ void
IMediaDeathNotifier::addObitRecipient(const wp<IMediaDeathNotifier>& recipient)
{
    ALOGV("addObitRecipient");
    Mutex::Autolock _l(sServiceLock);
    sObitRecipients.add(recipient);
}
/*static*/ void
IMediaDeathNotifier::removeObitRecipient(const wp<IMediaDeathNotifier>& recipient)
{
    ALOGV("removeObitRecipient");
    Mutex::Autolock _l(sServiceLock);
    sObitRecipients.remove(recipient);
}
void
IMediaDeathNotifier::DeathNotifier::binderDied(const wp<IBinder>& who) {
    ALOGW("media server died");
    // Need to do this with the lock held
    SortedVector< wp<IMediaDeathNotifier> > list;
    {
        Mutex::Autolock _l(sServiceLock);
        sMediaPlayerService.clear();
        list = sObitRecipients;
    }
    // Notify application when media server dies.
    // Don't hold the static lock during callback in case app
    // makes a call that needs the lock.
    size_t count = list.size();
    for (size_t iter = 0; iter < count; ++iter) {
        sp<IMediaDeathNotifier> notifier = list[iter].promote();
        if (notifier != 0) {
            notifier->died();
        }
    }
}
IMediaDeathNotifier::DeathNotifier::~DeathNotifier()
{
    ALOGV("DeathNotifier::~DeathNotifier");
    Mutex::Autolock _l(sServiceLock);
    sObitRecipients.clear();
    if (sMediaPlayerService != 0) {
        sMediaPlayerService->asBinder()->unlinkToDeath(this);
    }
}
}; // namespace android