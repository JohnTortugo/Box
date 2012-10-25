//#define BX_SHOW_DEBUG 1

#define BX_PANIC(fmt) do { printf("[PANIC]: "); printf fmt; printf("\n"); exit(1);  } while(0)

#ifdef BX_SHOW_DEBUG
  #define BX_ERROR(fmt) do { printf("[ERROR]: "); printf fmt; printf("\n"); fflush(stdout); } while(0)
  #define BX_INFO(fmt)  do { printf("[INFO ]: "); printf fmt; printf("\n"); fflush(stdout); } while(0)
  #define BX_DEBUG(fmt) do { printf("[DEBUG]: "); printf fmt; printf("\n"); fflush(stdout); } while(0)
#else
  #define BX_ERROR(fmt)
  #define BX_INFO(fmt)
  #define BX_DEBUG(fmt)
#endif
