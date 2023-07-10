#include "oled.h"

const char* OLED_GPS_STATUS_LIST[4] = {
    "FAIL",
    "Good",
    "GPS_TIMEOUT",
    "GPS_UNKNOW",
};

char* gpsInfo_lat_str = NULL;
char* gpsInfo_lon_str = NULL;
char* gpsInfo_status_str = NULL;
int gpsInfo_lat_str_len = 0;
int gpsInfo_lon_str_len = 0;
int gpsInfo_status_srt_len = 0;

int oled_init(){

    IoTGpioInit(HI_IO_NAME_GPIO_13);
    IoTGpioInit(HI_IO_NAME_GPIO_14);

    hi_io_set_func(HI_IO_NAME_GPIO_13, HI_IO_FUNC_GPIO_13_I2C0_SDA);
    hi_io_set_func(HI_IO_NAME_GPIO_14, HI_IO_FUNC_GPIO_14_I2C0_SCL);

    IoTI2cInit(0, OLED_I2C_BAUDRATE);

    ssd1306_Init();

    gpsInfo_lat_str = hi_malloc(812,STRING_CACHE_MAXLEN);
    gpsInfo_lon_str = hi_malloc(812,STRING_CACHE_MAXLEN);
    gpsInfo_status_str = hi_malloc(812,STRING_CACHE_MAXLEN);

    if( gpsInfo_lat_str == HI_NULL || gpsInfo_lon_str == HI_NULL ){
        if( gpsInfo_lat_str != HI_NULL )
            hi_free(812,gpsInfo_lat_str);
        if( gpsInfo_lon_str != HI_NULL )
            hi_free(812,gpsInfo_lon_str);
        if( gpsInfo_status_str != HI_NULL )
            hi_free(812,gpsInfo_status_str);
        return -1;
    }
    return 0;
}

int oled_page_change();







void oled_task(void* arg)
{
    (void) arg;

    oled_init();

    int flag = 0;

    ssd1306_Fill(Black);

    // usleep(20*1000);
    // ssd1306_Init();
    // ssd1306_Fill(Black);
    // ssd1306_SetCursor(0, 0);
    // ssd1306_DrawString("Hello HarmonyOS!", Font_7x10, White);
    // ssd1306_SetCursor(0, 12);
    // ssd1306_DrawString("Hello HarmonyOS!", Font_7x10, White);
    // ssd1306_SetCursor(0, 24);
    // ssd1306_DrawString("Hello HarmonyOS!", Font_7x10, White);
    // ssd1306_UpdateScreen();

    int f_times = 0;

    while(1){
        // ssd1306_Fill(Black);

        osDelay( osKernelGetTickFreq()/2 );

        gpsInfo_status_srt_len = OLED_GPS_STATUS_UNKNOW;
        flag = gps_mqtt_event_wait(2000);
        DBprintf("OLED_FLAG_GET:%x\n",flag);
        switch(flag){
            case GPS_INFO_Fail : gpsInfo_status_srt_len = OLED_GPS_STATUS_FAIL;break;
            case GPS_INFO_SUCESS : gpsInfo_status_srt_len = OLED_GPS_STATUS_SUCCESS;break;
            case GPS_INFO_TIMEOUT : gpsInfo_status_srt_len = OLED_GPS_STATUS_TIMEOUT;break;
            default : gpsInfo_status_srt_len = OLED_GPS_STATUS_UNKNOW;break;
        }

        gpsInfo_status_srt_len = snprintf(gpsInfo_status_str,STRING_CACHE_MAXLEN-1,"GPS:%s T:%d",OLED_GPS_STATUS_LIST[gpsInfo_status_srt_len],f_times);
        gpsInfo_lat_str_len = snprintf(gpsInfo_lat_str,STRING_CACHE_MAXLEN-1,"LAT:%.05f",gip_globe.lat);
        gpsInfo_lon_str_len = snprintf(gpsInfo_lon_str,STRING_CACHE_MAXLEN-1,"LON:%.05f",gip_globe.lon);
        ssd1306_SetCursor(0, 0);
        ssd1306_DrawString(gpsInfo_status_str, Font_7x10, White);
        ssd1306_SetCursor(0, 12);
        ssd1306_DrawString(gpsInfo_lat_str, Font_7x10, White);
        ssd1306_SetCursor(0, 24);
        ssd1306_DrawString(gpsInfo_lon_str, Font_7x10, White);
        ssd1306_UpdateScreen();
        f_times++;
    }

}

int oled_entry()
{
    osThreadAttr_t attr;

    attr.name = "oled_entry";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = OLED_TASK_STAK_SIZE;
    attr.priority = OLED_TASK_PRIORITY;

    if (osThreadNew(oled_task, NULL, &attr) == NULL) {
        printf("[oled_entry] Falied to create Ssd1306TestTask!\n");
        return -1;
    }

    return 0;
}








// --------- TEMP ------------

        // if( flag != GPS_INFO_SUCESS )
        // {
        //     DBprintf("GET_GPS_INFO_FAIL\n");
        //     // osDelay( 1000 );
        //     // flag = 0;
        //     // continue;

        // }
