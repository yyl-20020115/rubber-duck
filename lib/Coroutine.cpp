//========================================================================
//  COROUTINE.CPP - implementation from
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//========================================================================

#include <cstring>
#include <cstdio>
#include <new>  // bad::alloc
#include "Coroutine.h"
#include "Task.h"  // Stig Kofoed's "Portable Multitasking" coroutine library

namespace rubber_duck {

#ifdef _WIN32

LPVOID Coroutine::lpMainFiber;

void Coroutine::init(unsigned totalStack, unsigned mainStack)
{
    lpMainFiber = ConvertThreadToFiber(0);
    if (!lpMainFiber) {
        DWORD err = GetLastError();
        if (err == ERROR_ALREADY_FIBER)
            lpMainFiber = GetCurrentFiber();
        if (!lpMainFiber){
			printf("Coroutine::init(): Cannot convert main thread to fiber\n");
			exit(0);
		}
    }
}

void Coroutine::switchTo(Coroutine *cor)
{
    SwitchToFiber(cor->lpFiber);
}

void Coroutine::switchToMain()
{
    SwitchToFiber(lpMainFiber);
}

Coroutine::Coroutine()
{
    lpFiber = 0;
}

Coroutine::~Coroutine()
{
    if (lpFiber != 0)
        DeleteFiber(lpFiber);
}

bool Coroutine::setup(CoroutineFnp fnp, void *arg, unsigned stkSize)
{
    // stack_size sets the *committed* stack size; *reserved* (=available)
    // stack size is 1MB by default; this allows ~2048 coroutines in a
    // 2GB address space
    stackSize = stkSize;

    // XXX: CreateFiberEx() does not seem to work any better than CreateFiber(),
    // it appears to have the same limit for the number of fibers that can be created.
    lpFiber = CreateFiberEx(stackSize, stackSize, 0, (LPFIBER_START_ROUTINE)fnp, arg);
    //lpFiber = CreateFiber(stkSize, (LPFIBER_START_ROUTINE)fnp, arg);
    return lpFiber != nullptr;
}

bool Coroutine::hasStackOverflow() const
{
    return false;
}

unsigned Coroutine::getStackSize() const
{
    return stackSize;
}

unsigned Coroutine::getStackUsage() const
{
    return 0;
}

void Coroutine::taskFinished(){
	SwitchToFiber(lpMainFiber);
}

#else
void Coroutine::init(unsigned totalStack, unsigned mainStack)
{
    task_init(totalStack, mainStack);
}

void Coroutine::switchTo(Coroutine *cor)
{
    task_switchto(((Coroutine *)cor)->task);
}

void Coroutine::switchToMain()
{
    task_switchto(&main_task);
}

Coroutine::Coroutine()
{
    task = nullptr;
}

void Coroutine::taskFinished(){
    task = nullptr;
}

Coroutine::~Coroutine()
{
    if (task)
        task_free(task);
}

bool Coroutine::setup(CoroutineFnp fnp, void *arg, unsigned stackSize)
{
    task = task_create(fnp, arg, stackSize);
    return task != nullptr;
}

bool Coroutine::hasStackOverflow() const
{
    return task == nullptr ? false : task_testoverflow(task);
}

unsigned Coroutine::getStackSize() const
{
    return task == nullptr ? 0 : task->size;
}

unsigned Coroutine::getStackUsage() const
{
    return task == nullptr ? 0 : task_stackusage(task);
}

#endif

}  // namespace rubber_duck

