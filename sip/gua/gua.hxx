
#ifndef __GUA_H_XXX__
#define __GUA_H_XXX__


//#ifndef CODE_OPTIMIZE
#if 0
#define DEBUG_MEM_USAGE(msg) debugMemUsage((msg), __FILE__, __LINE__)
#define INIT_DEBUG_MEM_USAGE system("rm -f gua_mem.txt")
#else
#define DEBUG_MEM_USAGE(msg) /* NOP */
#define INIT_DEBUG_MEM_USAGE /* NOP */
#endif

void debugMemUsage(const char* msg, const char* file, int line);


#endif
