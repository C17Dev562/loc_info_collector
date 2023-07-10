#include <stdio.h>
#include <cmsis_os2.h>


int wb_tools_init();

osStatus_t wb_osDelay(uint32_t ms);

osStatus_t wb_osDelay_until(uint32_t ms);