#ifndef BX_PANIC
 #define BX_PANIC(fmt) do { printf("[PANIC]: "); printf fmt; printf("\n"); exit(1);  } while(0)
#endif

#ifndef BX_ERROR
#define BX_ERROR(fmt) do { printf("[ERROR]: "); printf fmt; printf("\n"); fflush(stdout); } while(0)
#endif

#ifndef BX_INFO
#define BX_INFO(fmt)  do { printf("[INFO ]: "); printf fmt; printf("\n"); fflush(stdout); } while(0)
#endif

#ifndef BX_DEBUG
#define BX_DEBUG(fmt) do { printf("[DEBUG]: "); printf fmt; printf("\n"); fflush(stdout); } while(0)
#endif
