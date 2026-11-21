/*
===========================================================================
Portions derived from WolfcamQL's cg_mem.c.
WolfcamQL is distributed under the GNU General Public License v2.
See COPYING.txt for the full license.
===========================================================================
*/

#include "cg_mem.h"

#include <stdlib.h>

void* CG_MallocMem(size_t size) {
    return malloc(size);
}

void* CG_CallocMem(size_t size) {
    return calloc(1, size);
}

void CG_FreeMem(void* ptr) {
    free(ptr);
}
