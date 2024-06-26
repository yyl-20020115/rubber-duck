//==========================================================================
//   COROUTINE.H  -  header from
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

#ifndef COROUTINE_H_
#define COROUTINE_H_

#include "platdefs.h"

#ifdef _WIN32
	#include <windows.h>
#endif 

namespace rubber_duck {
	
#ifndef _WIN32
	struct _Task;
#endif

/**
 * @brief Prototype for functions that can be used with cCoroutine objects as
 * coroutine bodies.

 * @ingroup SimSupport
 */
typedef void (*CoroutineFnp)( void * );

//--------------------------------------------------------------------------

/**
 * @brief Low-level coroutine library. Coroutines are used by cSimpleModule.
 *
 * cCoroutine has platform-dependent implementation:
 *
 * On Windows, it uses the Win32 Fiber API.
 *
 * On Unix-like systems, it uses POSIX coroutines (setcontext()/switchcontext())
 * if they are available.
 *
 * Otherwise, it uses a portable coroutine library first described
 * by Stig Kofoed ("Portable coroutines", see the Manual for a better
 * reference). It creates all coroutine stacks within the main stack,
 * and uses setjmp()/longjmp() for context switching. This
 * implies that the maximum stack space allowed by the operating system
 * for the \opp process must be sufficiently high (several,
 * maybe several hundred megabytes), otherwise a segmentation fault
 * will occur.
 *
 * @ingroup SimSupport
 */
class Coroutine
{
  protected:
#ifdef _WIN32
    LPVOID lpFiber;
    static LPVOID lpMainFiber;
    unsigned stackSize;
#else
    _Task *task;
#endif 
  public:
    /** @name Coroutine control */
    //@{

    /**
     * Initializes the coroutine library. This function has to be called
     * exactly once in a program, possibly at the top of main().
     */
    static void init(unsigned totalStack, unsigned mainStack);

    /**
     * Switch to another coroutine. The execution of the current coroutine
     * is suspended and the other coroutine is resumed from the point it
     * last left off.
     */
    static void switchTo(Coroutine *cor);

    /**
     * Switch to the main coroutine (the one main() runs in).
     */
    static void switchToMain();
    //@}

    /** @name Constructor, destructor */
    //@{

    /**
     * Sets up a coroutine. The arguments are the function that should be
     * run in the coroutine, a pointer that is passed to the coroutine
     * function, and the stack size.
     */
    bool setup(CoroutineFnp fnp, void *arg, unsigned stackSize);

    void taskFinished();
    /**
     * Constructor.
     */
    Coroutine();

    /**
     * Destructor.
     */
    virtual ~Coroutine();
    //@}

    /** @name Coroutine statistics */
    //@{

    /**
     * Returns true if there was a stack overflow during execution of the
     * coroutine.
     *
     * Windows/Fiber API, POSIX coroutines: Not implemented: always returns false.
     *
     * Portable coroutines: it checks the intactness of a predefined byte pattern
     * (0xdeadbeef) at the stack boundary, and report stack overflow
     * if it was overwritten. The mechanism usually works fine, but occasionally
     * it can be fooled by large uninitialized local variables
     * (e.g. char buffer[256]): if the byte pattern happens to fall in the
     * middle of such a local variable, it may be preserved intact and
     * stack violation is not detected.
     */
    virtual bool hasStackOverflow() const;

    /**
     * Returns the stack size of the coroutine.
     */
    virtual unsigned getStackSize() const;

    /**
     * Returns the amount of stack actually used by the coroutine.
     *
     * Windows/Fiber API, POSIX coroutines: Not implemented, always returns 0.
     *
     * Portable coroutines: It works by checking the intactness of
     * predefined byte patterns (0xdeadbeef) placed in the stack.
     */
    virtual unsigned getStackUsage() const;
    //@}
};

}  // namespace rubber_duck


#endif


