#include "wb_tools.h"

uint32_t kick_psec;

int wb_tools_init()
{
    kick_psec = osKernelGetTickFreq();
    return 0;
}

osStatus_t wb_osDelay(uint32_t ms)
{
    uint32_t kick_to_delay = (uint32_t)( (double) kick_psec * ( (double) ms / 1000.0 ) );
    return osDelay(kick_to_delay);
}

osStatus_t wb_osDelay_until(uint32_t ms)
{
    uint32_t kick_to_delay = (uint32_t)( (double) kick_psec * ( (double) ms / 1000.0));
    return osDelayUntil(kick_to_delay);
}
