// include/FreeRTOSConfig.h

#include_next "freertos/FreeRTOSConfig.h"  // lấy toàn bộ macro gốc

// Ghi đè vài macro cụ thể
#undef configUSE_PREEMPTION  
#define configUSE_PREEMPTION   1