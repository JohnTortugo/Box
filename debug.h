#define DEBUG 1

#define BX_PANIC(fmt) do { printf("[PANIC]: "); printf fmt; printf("\n"); exit(1);  } while(0)
#define BX_INFO(fmt)  do { printf("[INFO ]: "); printf fmt; printf("\n"); } while(0)
#define BX_DEBUG(fmt) do { printf("[DEBUG]: "); printf fmt; printf("\n"); } while(0)
#define BX_ERROR(fmt) do { printf("[ERROR]: "); printf fmt; printf("\n"); } while(0)
