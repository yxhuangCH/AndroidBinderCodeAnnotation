package com.android.internal.os;
import android.os.Binder;
import android.os.IBinder;
import android.os.SystemClock;
import android.util.EventLog;
import android.util.Log;
import java.io.FileDescriptor;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.lang.ref.WeakReference;
import java.lang.reflect.Modifier;
/**
 * Private and debugging Binder APIs.
 * 
 * @see IBinder
 */
public class BinderInternal {
    static WeakReference<GcWatcher> mGcWatcher
            = new WeakReference<GcWatcher>(new GcWatcher());
    static long mLastGcTime;
    
    static final class GcWatcher {
        @Override
        protected void finalize() throws Throwable {
            handleGc();
            mLastGcTime = SystemClock.uptimeMillis();
            mGcWatcher = new WeakReference<GcWatcher>(new GcWatcher());
        }
    }
    
    /**
     * Add the calling thread to the IPC thread pool.  This function does
     * not return until the current process is exiting.
     */
    public static final native void joinThreadPool();
    
    /**
     * Return the system time (as reported by {@link SystemClock#uptimeMillis
     * SystemClock.uptimeMillis()}) that the last garbage collection occurred
     * in this process.  This is not for general application use, and the
     * meaning of "when a garbage collection occurred" will change as the
     * garbage collector evolves.
     * 
     * @return Returns the time as per {@link SystemClock#uptimeMillis
     * SystemClock.uptimeMillis()} of the last garbage collection.
     */
    public static long getLastGcTime() {
        return mLastGcTime;
    }
    /**
     * Return the global "context object" of the system.  This is usually
     * an implementation of IServiceManager, which you can use to find
     * other services.
     */
    public static final native IBinder getContextObject();
    
    /**
     * Special for system process to not allow incoming calls to run at
     * background scheduling priority.
     * @hide
     */
    public static final native void disableBackgroundScheduling(boolean disable);
    
    static native final void handleGc();
    
    public static void forceGc(String reason) {
        EventLog.writeEvent(2741, reason);
        Runtime.getRuntime().gc();
    }
    
    static void forceBinderGc() {
        forceGc("Binder");
    }
}