#ifdef __vxworks

  #define __BYTE_ORDER _BYTE_ORDER
  #define __LITTLE_ENDIAN _LITTLE_ENDIAN
  #define __BIG_ENDIAN _BIG_ENDIAN

#else

  #ifdef __sgi
    #include <sys/endian.h>
  #else
    #ifdef __sparc
      #include <sys/isa_defs.h>

      #define __LITTLE_ENDIAN 1234
      #define __BIG_ENDIAN 4321

      #ifdef _LITTLE_ENDIAN
        #define __BYTE_ORDER __LITTLE_ENIDAN
      #endif

      #ifdef _BIG_ENDIAN
        #define __BYTE_ORDER __BIG_ENDIAN 
      #endif

    #else
      #ifdef __linux__
        #include <endian.h>
      #endif
      #ifdef __FreeBSD__
        #include <machine/endian.h>
      #endif
    #endif
  #endif
#endif
