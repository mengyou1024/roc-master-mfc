#include "pch.h"

#include "Thread.h"

Thread::Thread() :
_pthread(nullptr),
m_bWorking(false),
_hevent(INVALID_HANDLE_VALUE) {
}

Thread::~Thread() {
    Close();
}

bool Thread::InitEvent() {
    if (_hevent != INVALID_HANDLE_VALUE)
        return true;

    _hevent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (_hevent == INVALID_HANDLE_VALUE)
        return false;

    return true;
}

bool Thread::WaitEvent(INT iTime) {
    if (WaitForSingleObject(_hevent, iTime) == WAIT_TIMEOUT)
        return false;

    return true;
}

bool Thread::Event(bool bEnable) {
    if (_hevent == INVALID_HANDLE_VALUE)
        return false;

    if (bEnable)
        SetEvent(_hevent);
    else
        ResetEvent(_hevent);

    return true;
}

void Thread::Close() {
    m_bWorking = false;

    if (_pthread) {
        _pthread->join();
        delete _pthread;
        _pthread = nullptr;
    }

    if (_hevent != INVALID_HANDLE_VALUE) {
        CloseHandle(_hevent);
        _hevent = INVALID_HANDLE_VALUE;
    }
}