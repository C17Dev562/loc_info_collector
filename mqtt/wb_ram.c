#include "wb_ram.h"

#define HI_MOD_ID_APP_COMMON 812 /* 32C */

gps_info_pak gip_globe = {0};
lte_m_info lmi_globe = {0};
lte_m_status lms_globe = {
    AT_SYN_FAIL,
    AT_ATV_UNKNOW,
    AT_ATE_UNKNOW
};

void wb_ram_init(){
    gip_globe.lat = 0;
    gip_globe.lon = 0;
    gip_globe.status = GPS_STATUS_ERROR;

    lmi_globe.brand = hi_malloc(HI_MOD_ID_APP_COMMON,LMI_STR_LEN);
    lmi_globe.brand = hi_malloc(HI_MOD_ID_APP_COMMON,LMI_STR_LEN);
}

