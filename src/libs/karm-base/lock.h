#pragma once

#include <karm-meta/nocopy.h>

#include "_embed.h"

#include "atomic.h"
#include "string.h"

namespace Karm {

struct CriticalScope :
    Meta::Static {

    CriticalScope() {
        _Embed::enterCritical();
    }

    ~CriticalScope() {
        _Embed::leaveCritical();
    }
};

struct Lock :
    Meta::Static {

    Atomic<bool> _lock{};

    bool _tryAcquire() {
        bool result = _lock.cmpxchg(false, true);
        memoryBarier();

        if (not result) {
            _Embed::leaveCritical();
        }

        return result;
    }

    bool tryAcquire() {
        _Embed::enterCritical();
        return _tryAcquire();
    }

    void acquire() {
        _Embed::enterCritical();

        while (not _tryAcquire()) {
            _Embed::relaxe();
        }
    }

    void release() {
        memoryBarier();
        _lock.store(false);
        _Embed::leaveCritical();
    }
};

struct NoLock : Meta::Static {
    bool tryAcquire() {
        return true;
    }

    void acquire() {}

    void release() {}
};

template <typename T>
concept Lockable =
    requires(T &lockable) {
        lockable.tryAcquire();
        lockable.acquire();
        lockable.release();
    };

struct LockScope :
    Meta::Static {
    Lock &_lock;

    LockScope(Lock &lock)
        : _lock(lock) {
        _lock.acquire();
    }

    ~LockScope() {
        _lock.release();
    }
};

template <typename T>
struct LockProtected {
    Lock _lock;
    T _value;

    LockProtected() = default;

    template <typename... Args>
    LockProtected(Args &&...args)
        : _value(std::forward<Args>(args)...) {}

    auto with(auto f) {
        LockScope scope(_lock);
        return f(_value);
    }
};

struct RwLock : Meta::Static {
    Lock _lock;
    Atomic<isize> _pendings{};
    isize _readers{};
    isize _writers{};

    void acquireRead() {
        _Embed::enterCritical();

        while (not tryAcquireRead()) {
            _Embed::relaxe();
            memoryBarier();
        }
    }

    bool tryAcquireRead() {
        LockScope scope(_lock);

        if (_pendings.load())
            return false;

        if (_writers)
            return false;

        ++_readers;

        _Embed::enterCritical();
        return true;
    }

    void releaseRead() {
        LockScope scope(_lock);
        --_readers;
        _Embed::leaveCritical();
    }

    void acquireWrite() {
        _Embed::enterCritical();

        _pendings.inc();

        while (not tryAcquireWrite()) {
            _Embed::relaxe();
            memoryBarier();
        }

        _pendings.dec();
    }

    bool tryAcquireWrite() {
        LockScope scope(_lock);

        if (_readers)
            return false;

        if (_writers)
            return false;

        ++_writers;
        _Embed::enterCritical();
        return true;
    }

    void releaseWrite() {
        LockScope scope(_lock);
        --_writers;
        _Embed::leaveCritical();
    }
};

struct ReadLockScope : Meta::Static {
    RwLock &_lock;

    ReadLockScope(RwLock &lock)
        : _lock(lock) {
        _lock.acquireRead();
    }

    ~ReadLockScope() {
        _lock.releaseRead();
    }
};

struct WriteLockScope : Meta::Static {
    RwLock &_lock;

    WriteLockScope(RwLock &lock)
        : _lock(lock) {
        _lock.acquireWrite();
    }

    ~WriteLockScope() {
        _lock.releaseWrite();
    }
};

} // namespace Karm
