#ifndef SYSTEM_H
#define SYSTEM_H

#include <sys/resource.h>
#include <iostream>

namespace Fondue {

    static void alloc_stack(const rlim_t stackSize)
    {
        struct rlimit rl;
        int result;

        result = getrlimit(RLIMIT_STACK, &rl);
        if (result == 0)
        {
            if (rl.rlim_cur < stackSize)
            {
                rl.rlim_cur = stackSize;
                result = setrlimit(RLIMIT_STACK, &rl);
                if (result != 0)
                {
                    fprintf(stderr, "setrlimit returned result = %d\n", result);
                }
            }
        }
    }

}

#endif // SYSTEM_H
