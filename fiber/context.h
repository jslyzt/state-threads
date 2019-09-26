#pragma once

#include "fiber.h"
#include <WinSock2.h>
#include <Windows.h>
#include <algorithm>

#if defined(_MSC_VER) && _MSC_VER < 1900
# define thread_local __declspec(thread)
#endif

class Context;
struct FiberScopedGuard {
    FiberScopedGuard::FiberScopedGuard() {
        GetTlsContext() = ConvertThreadToFiber(nullptr);
    }
    FiberScopedGuard::~FiberScopedGuard() {
        ConvertFiberToThread();
        GetTlsContext() = nullptr;
    }
    static void*& GetTlsContext() {
        static thread_local void* native = nullptr;
        return native;
    }
    static Context*& CurrentContext() {
        static thread_local Context* current = nullptr;
        return current;
    }
};

typedef FiberScopedGuard FiberSG;

class Context {
public:
    Context(fn_t fn, intptr_t vp, std::size_t stackSize) {
        SIZE_T commit_size = 4 * 1024;
        ctx_ = CreateFiberEx(commit_size, std::max<std::size_t>(stackSize, commit_size),
            FIBER_FLAG_FLOAT_SWITCH, (LPFIBER_START_ROUTINE)fn, (LPVOID)vp);
    }

    ~Context() {
        DeleteFiber(ctx_);
    }

    inline void SwapIn() {
        FiberSG::CurrentContext() = this;
        SwitchToFiber(ctx_);
    }

    inline void SwapOut() {
        FiberSG::CurrentContext() = nullptr;
        SwitchToFiber(FiberSG::GetTlsContext());
    }

    inline void* getCtx() {
        return ctx_;
    }

private:
    void* ctx_;
};
