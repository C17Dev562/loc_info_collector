/*
 * @Author: C17_Dev cone7work98@outlook.com
 * @Date: 2023-04-14 14:29:05
 * @LastEditors: C17_Dev cone7work98@outlook.com
 * @LastEditTime: 2023-06-30 14:02:54
 * @FilePath: /OpenHarmony-v3.1-Release/applications/sample/wifi-iot/app/wb_app/app/AT_uart/wb_at.c
 * @Description:
 *
 *
 * Copyright (c) 2023 by C17_Dev cone7work98@outlook.com, All Rights Reserved.
 */
/*
 * Copyright (c) 2020 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "wb_at.h"
// #include "nmea/nmea.h"
#define MAX_GPS_FAIL_TIMES 100
// buff setup

static hi_u8 *g_buff = HI_NULL;
static hi_s32 buff_len = 0;

static hi_u8 *g_uart_buff = HI_NULL;
static hi_s32 uBuff_len = 0;

// buff end

static hi_u32 point = 0; // buff's point (num point)
// //nmea - start
// nmeaINFO* g_info = HI_NULL;
// nmeaPARSER* g_parser = HI_NULL;
// //nmea - end

static hi_u32 g_uart_task_id = 0;

static int loc_value_sign = 0; // locate info value sign 0:invalid 1:valid

bsta_info bsta_g = {1};          // base station list head node
bsta_info *bsta_g_end = &bsta_g; // base station end node needle

/// @brief 创建并添加一个结点到基站链表
/// @return 返回新创建的一个结点
static bsta_info *create_add_bstalist()
{
    bsta_info *end = &bsta_g;
    bsta_info *new = NULL;
    int index = 2;
    while (end->next != NULL)
    {
        end = end->next;
        index++;
    }
    new = malloc(sizeof(bsta_info));
    memset(new, 0, sizeof(bsta_info));
    new->id = index;
    end->next = new;
    bsta_g_end = new;
    return new;
}

/// @brief 获得基站链表结点个数
/// @return 基站列表的结点个数
static int num_of_bstalist()
{
    bsta_info *end = &bsta_g;
    bsta_info *new = NULL;
    int index = 1;
    while (end->next != NULL)
    {
        end = end->next;
        index++;
    }
    return index;
}
// static void clean_buff(hi_u8 *buff,hi_u32 buff_len){

// }

#define BSTA_STR_CAT_LEN 5
// AT+QCELL=?
int bsta_str_process(char *str, int str_len)
{
    char temp[15] = {0};                                 // 缓存变量
    int flag = 0;                                        // 0 - 头检测中;2 - 内容_跳过中;3 - 内容_检测到逗号;4 - 内容_正在处理
    int index = 0;                                       // 字符处理指针
    int value_index[BSTA_STR_CAT_LEN] = {2, 3, 4, 5, 7}; // 处理字段：mcc,mnc,lac,ci,rx_lev
    int cur_Vindex = -1;                                 // 目前的内容索引
    int cur_Vindex_add = 0;                              // 顺序访问到的处理字段索引

    bsta_info *cur_node = &bsta_g; // 基站链表处理结点
    int node_add_num = 0;

    for (; index < str_len; index++)
    {
        if (str[index] == '+')
        {
            cur_Vindex = -1;
            cur_Vindex_add = 0;
            if (cur_node == NULL)
                cur_node = create_add_bstalist(); // 结点为空，则创建

            // flag = 1;         // 识别到URC头标识
            int head_len = 0; // URC头长度
            for (int j = 0; j < URC_HEAD_MAX_LEN && index < str_len; j++, index++)
            { // 获取"+CREG"
                if (str[index] == ':')
                {
                    flag = 3; // URC头已获取,立刻标记为识别到内容开头
                    head_len = j + 1;
                    break;
                }
                temp[j] = str[index]; // 逐个拷贝URC头
            }
            // 此时 索引在':'
            if (strcmp(temp, "+QCELL") != 0)
            { // 不是想要的信息头，直接丢弃
                flag = -1;
                break;
            }
            memset(temp, 0, sizeof(char) * URC_HEAD_MAX_LEN); // 清空缓存
        }

        if (flag >= 2) // 头检测完成,开始内容处理
        {
            if (flag == 2)
            { // 被标记为跳过
                if (str[index] == ',')
                {
                    flag = 3;
                    goto CharJump;
                }
                else
                {
                    goto CharJump;
                }
            }

            if (flag == 3)
            { // 检测到开头
                cur_Vindex++;
                if (cur_Vindex == value_index[cur_Vindex_add]) //
                {
                    cur_Vindex_add++;
                    flag = 4; // 当前字段匹配，标记为正在处理
                }
                else
                {
                    flag = 2; // 不匹配，跳过
                    goto CharJump;
                }
            }

            if (flag == 4)
            { // 正在处理
                int content_len = 0;
                char *ptr = NULL;
                int value = 0;
                // if (cur_Vindex == 4)
                // {
                for (int j = 0; j < URC_CONT_MAX_LEN && index < str_len; j++, index++)
                { // 获取mcc字段信息
                    if (str[index] == ',' || str[index] == '\n')
                    {
                        flag = 3;
                        switch (cur_Vindex)
                        {
                        case 2:
                            value = (int)strtol(temp, &ptr, 10);
                            cur_node->mcc = value;
                            break;
                        case 3:
                            value = (int)strtol(temp, &ptr, 10);
                            cur_node->mnc = value;
                            break;
                        case 4:
                            value = (int)strtol(temp, &ptr, 16);
                            cur_node->lac = value;
                            break;
                        case 5:
                            value = (int)strtol(temp, &ptr, 16);
                            cur_node->ci = value;
                            break;
                        case 7:
                            value = (int)strtol(temp, &ptr, 10);
                            cur_node->rx_lev = value;
                            cur_node = cur_node->next; // 处理结点跳掉下一个
                            node_add_num++;
                            flag = 0; // 恢复初始状态
                            break;    // 添加结束标记
                        default:
                            flag = -2;
                            break;
                        }
                        memset(temp, 0, sizeof(char) * URC_HEAD_MAX_LEN); // 清空缓存
                        goto CharJump;
                    }
                    temp[content_len] = str[index];
                    content_len++;
                }
                flag = -1;

                // }
            }
        }

        if (str[index] == '+')
        {
            if (cur_node == NULL)
                cur_node = create_add_bstalist(); // 结点为空，则创建

            // flag = 1;         // 识别到URC头标识
            int head_len = 0; // URC头长度
            for (int j = 0; j < URC_HEAD_MAX_LEN && index < str_len; j++, index++)
            { // 获取"+CREG"
                if (str[index] == ':')
                {
                    flag = 3; // URC头已获取,立刻标记为识别到内容开头
                    head_len = j + 1;
                    break;
                }
                temp[j] = str[index]; // 逐个拷贝URC头
            }
            // 此时 索引在':'
            if (strcmp(temp, "+QCELL") != 0)
            { // 不是想要的信息头，直接丢弃
                flag = -1;
                break;
            }
            memset(temp, 0, sizeof(char) * URC_HEAD_MAX_LEN); // 清空缓存
        }

    CharJump:
        if (flag < 0)
        {
            break;
        }
    }
    return flag;
}

#define MIXLOCATE_STR_CAT_LEN 2
// AT+QENG="servingcell"
int mixlocate_str_process(char *str, int str_len, char *buff, int buff_len, int *write_len)
{
    char temp[15] = {0};                        // 缓存变量
    int flag = 0;                               // 0 - 头检测中;2 - 内容_跳过中;3 - 内容_检测到逗号;4 - 内容_正在处理
    int index = 0;                              // 字符处理指针
    int value_index[BSTA_STR_CAT_LEN] = {3, 4}; //
    int cur_Vindex = -1;                        // 目前的内容索引
    int cur_Vindex_add = 0;                     // 顺序访问到的字段索引

    int payload_len = 0;

    int node_add_num = 0;

    for (; index < str_len; index++)
    {
        if (str[index] == '+')
        {
            cur_Vindex = -1;
            cur_Vindex_add = 0;
            // if (cur_node == NULL)
            //     cur_node = create_add_bstalist(); // 结点为空，则创建

            // flag = 1;         // 识别到URC头标识
            int head_len = 0; // URC头长度
            for (int j = 0; j < URC_HEAD_MAX_LEN && index < str_len; j++, index++)
            { // 获取"+CREG"
                if (str[index] == ':')
                {
                    flag = 3; // URC头已获取,立刻标记为识别到内容开头
                    head_len = j + 1;
                    break;
                }
                temp[j] = str[index]; // 逐个拷贝URC头
            }
            // 此时 索引在':'
            if (strcmp(temp, "+QMTRECV") != 0)
            { // 不是想要的信息头，直接丢弃
                flag = -1;
                break;
            }
            memset(temp, 0, sizeof(char) * URC_HEAD_MAX_LEN); // 清空缓存
        }

        if (flag >= 2) // 头检测完成,开始内容处理
        {
            if (flag == 2)
            { // 被标记为跳过
                if (str[index] == ',')
                {
                    flag = 3;
                    goto CharJump;
                }
                else
                {
                    goto CharJump;
                }
            }

            if (flag == 3)
            { // 检测到开头
                cur_Vindex++;
                if (cur_Vindex == value_index[cur_Vindex_add]) //
                {
                    cur_Vindex_add++;
                    flag = 4; // 当前字段匹配，标记为正在处理
                }
                else
                {
                    flag = 2; // 不匹配，跳过
                    goto CharJump;
                }
            }

            if (flag == 4)
            { // 正在处理
                int content_len = 0;
                char *ptr = NULL;
                int value = 0;
                // if (cur_Vindex == 4)
                // {
                for (int j = 0; j < URC_CONT_MAX_LEN && index < str_len; j++, index++)
                { // 获取mcc字段信息
                    if (cur_Vindex == 3)
                    {
                        if (str[index] == ',' || str[index] == '\n')
                        {
                            flag = 3;
                            payload_len = (int)strtol(temp, &ptr, 10);
                            memset(temp, 0, sizeof(char) * URC_HEAD_MAX_LEN); // 清空缓存
                            goto CharJump;
                        }
                        temp[content_len] = str[index];
                        content_len++;
                    }
                    if (cur_Vindex == 4)
                    {
                        if (str[index] == '"')
                        {
                            memcpy(buff, &str[index + 1], payload_len);
                        }
                        else
                        {
                            memcpy(buff, &str[index], payload_len);
                        }
                        *write_len = payload_len;
                        flag = -2;
                        memset(temp, 0, sizeof(char) * URC_HEAD_MAX_LEN); // 清空缓存
                        goto CharJump;
                    }
                }

                temp[content_len] = str[index];
                content_len++;
            }
        }

    CharJump:
        if (flag < 0)
        {
            break;
        }
    }
    return flag;
}

/// @brief 对读取的AT指令进行OK确认
/// @param data 要分析的字符串
/// @param data_len 字符串长度
/// @return AT_CMD_OK - 指令正常操作 ; AT_CMD_FAIL - 指令执行异常
int at_cmd_check(hi_u8 *str, hi_u32 str_len)
{
    int flag = 0;
    DBprintf("rec char last:%d\n", str[str_len - 1]);
    // DBprintf("rec char :%.*s\n", str[str_len - 1]);
    // 判断ATV状态 - 消息尾部
    switch (lms_globe.at_ATV)
    {
    case AT_ATV_UNKNOW: // 未知状态 - 两种情况都判断
        if (str[str_len - 1] == '\r' && str[str_len - 2] == '0')
        {
            lms_globe.at_ATV = AT_ATV_0;
            return AT_CMD_OK;
        }
        if (str[str_len - 1] == '\n' && str[str_len - 2] == '\r' && str[str_len - 3] == 'K' && str[str_len - 4] == 'O')
        {
            lms_globe.at_ATV = AT_ATV_1;
            return AT_CMD_OK;
        }
        break;
    case AT_ATV_0: // 简单尾部
        if (str[str_len - 1] == '\r' && str[str_len - 2] == '0')
        {
            lms_globe.at_ATV = AT_ATV_0;
            return AT_CMD_OK;
        }
        break;
    case AT_ATV_1: // 复杂尾部
        if (str[str_len - 1] == '\n' && str[str_len - 2] == '\r' && str[str_len - 3] == 'K' && str[str_len - 4] == 'O')
        {
            lms_globe.at_ATV = AT_ATV_1;
            return AT_CMD_OK;
        }
        break;
    default:               // 特殊尾部
        if (str[0] == '>') // 等待数据传输
            return AT_CMD_WAIT_DATA;
        return AT_CMD_FALI;
        break;
    }
    return AT_CMD_FALI;
}

/// @brief AT指令回复接受
/// @param rec_len 需要返回的接受长度的指针
/// @return  返回的长度;AT_CMD_FALI - 命令回复检测失败;AT_CMD_ERR - 读取错误(UART错误);
hi_s32 at_rep_receive(hi_u32 *rec_len)
{
    hi_s32 read_len = 0;   // 实际读取长度
    hi_s32 check_flag = 0; // 命令检测标志

    check_flag = uart_read_nio(USING_AT_UART_PORT, g_uart_buff, AT_UART_BUFF_SIZE, &read_len);

    if (check_flag == HI_ERR_FAILURE)
        return AT_CMD_ERR;

    check_flag = at_cmd_check(g_uart_buff, read_len); // 返回结尾检测
    // DEBUG PRINTF
    DBprintf("AT_REP_REC | LEN:%d;%.*s;STATUS:%d;\n", read_len, read_len, g_uart_buff, check_flag);

    *rec_len = read_len;

    return check_flag;
}

/// @brief AT指令回复接受
/// @param rec_len 需要返回的接受长度的指针
/// @return  返回的长度;AT_CMD_FALI - 命令回复检测失败;AT_CMD_ERR - 读取错误(UART错误);
hi_s32 at_rep_WaitConfirm_receive(hi_u32 *rec_len)
{
    hi_s32 read_len = 0;   // 实际读取长度
    hi_s32 check_flag = 0; // 命令检测标志
    int loop = 0;
    while (read_len < AT_UART_BUFF_SIZE && loop < 5)
    {
        check_flag = uart_read_wait(USING_AT_UART_PORT, g_uart_buff + read_len, AT_UART_BUFF_SIZE - read_len, 100);

        if (check_flag < 0)
            return AT_CMD_ERR;

        read_len += check_flag; // 保存读取长度

        check_flag = at_cmd_check(g_uart_buff, read_len); // 返回结尾检测
        // DEBUG PRINTF
        DBprintf("AT_REP_REC | LEN:%d;%.*s;STATUS:%d;\n", read_len, read_len, g_uart_buff, check_flag);
        if (check_flag == AT_CMD_OK)
            break;
        loop++;
    }
    *rec_len = read_len;
    // DBprintf("AT_REP_REC | LEN:%d;%.*s;STATUS:%d;\n", read_len, read_len, g_uart_buff, check_flag);
    return check_flag;
}

/// @brief 封装函数 - AT指令发送和接收
/// @param cmd_srt 要输出的命令
/// @param cmd_srt_len 输出命令的长度
/// @param rec_len 接收的长度
/// @return
hi_s32 at_cmd_sendAndRec(const char *cmd_srt, hi_u32 cmd_srt_len, hi_u32 *rec_len)
{
    uart_send_nio(USING_AT_UART_PORT, cmd_srt, cmd_srt_len);
    wb_osDelay(AT_REP_WAIT_TIME_MS); // 等待模块处理和打印
    return at_rep_receive(rec_len);
}

/// @brief 封装函数 - AT指令发送和接收
/// @param cmd_srt 要输出的命令
/// @param cmd_srt_len 输出命令的长度
/// @param rec_len 接收的长度
/// @return
hi_s32 at_cmd_sendAndWaitConfirm(const char *cmd_srt, hi_u32 cmd_srt_len, hi_u32 *rec_len)
{
    uart_send_nio(USING_AT_UART_PORT, cmd_srt, cmd_srt_len);
    // wb_osDelay(AT_REP_WAIT_TIME_MS); // 等待模块处理和打印
    return at_rep_WaitConfirm_receive(rec_len);
}

/// @brief AT指令重置恢复 - 发多个AT并抛弃接受数据
/// @param cmd_srt 要输出的命令
/// @param cmd_srt_len 输出命令的长度
/// @param rec_len 接收的长度
/// @return AT_SYN_OK - 同步成功 ; AT_SYN_FAIL - 同步失败
hi_s32 at_cmd_RESYN()
{
    int rec_len = 0;
    for (int i = 0; i < AT_RESET_TIMES; i++)
    {
        if (at_cmd_sendAndRec("AT\r", strlen("AT\r"), &rec_len) == AT_CMD_OK)
            return AT_SYN_OK;
    }
    return AT_SYN_FAIL;
}

/// @brief init the uart port and memony , but uart processing not started
/// @return 0 - OK ; -1 - UART Init Fail ; -2 - MEMONY Init Fail
int at_init()
{
    hi_io_set_func(HI_IO_NAME_GPIO_0, HI_IO_FUNC_GPIO_0_UART1_TXD); /* uart1 tx */
    hi_io_set_func(HI_IO_NAME_GPIO_1, HI_IO_FUNC_GPIO_1_UART1_RXD); /* uart1 rx */

    hi_u32 ret;
    hi_uart_attribute uart_attr = {
        .baud_rate = 115200, /* baud_rate: 9600 */
        .data_bits = 8,      /* data_bits: 8bits */
        .stop_bits = 1,
        .parity = 0,
    };

    /* Initialize uart driver */
    ret = hi_uart_init(USING_AT_UART_PORT, &uart_attr, HI_NULL);
    if (ret != HI_ERR_SUCCESS)
    {
        printf("[sys_info] Failed to init uart! Err code = %d\n", ret);
        return -1;
    }

    // malloc the memony
    g_buff = hi_malloc(HI_MOD_ID_APP_COMMON, BUFF_MAX);
    g_uart_buff = hi_malloc(HI_MOD_ID_APP_COMMON, AT_UART_BUFF_SIZE);
    printf("at memony status: %d %d\n", g_buff, g_uart_buff);

    if (g_buff == HI_NULL || g_uart_buff == HI_NULL)
    {
        at_deinit();
        return -2;
    }

    return 0;
}

/// @brief AT Module Init
/// @return AT_SYN_FAIL - SYN Fail ; AT_SYN_OK - SYN Ok
int at_module_init()
{
    // AT_MODLE_INIT_START
    // AT_SYN
    int flag = -1;
    int read_len = 0;
    for (int i = 0; i < AT_SYN_MAX_TIMES; i++)
    {
        for (int j = 0; j < 10; j++)
        { // Send The AT SYN ACK
            if (at_cmd_sendAndRec("AT\r", strlen("AT\r"), &read_len) == AT_CMD_OK)
            {
                flag = 1;
                break;
            }
        }
        if (flag == 1)
            break;
        wb_osDelay(AT_SYN_WAIT_TIME_MS);
    }

    // 4G MODULE INIT FAIL
    if (flag == -1)
    {
        DBprintf("THE 4G MODLE INIT FAIL\n");
        at_deinit();
        return AT_SYN_FAIL;
    }
    wb_osDelay(AT_SYN_WAIT_TIME_MS);

    // // ATI ack the module modle(not finish) - START
    // hi_uart_write(USING_AT_UART_PORT, "ATI\r", strlen("AT\r"));
    // wb_osDelay(500);
    // flag = uart_read_nio(USING_AT_UART_PORT, g_uart_buff , AT_UART_BUFF_SIZE);
    // if( flag == HI_ERR_FAILURE || at_cmd_check(g_uart_buff,flag) == AT_CMD_FALI )
    //     return -1;
    // // ATI ack the module modle(not finish) - END

    // ATV CONFIG - START
    if (lms_globe.at_ATV != AT_ATV_0)
    {
        lms_globe.at_ATV = AT_ATV_UNKNOW;
        if (at_cmd_sendAndRec("ATV0\r", strlen("ATV0\r"), &read_len) != AT_CMD_OK)
        {
            at_deinit();
            return AT_SYN_FAIL;
        }
    }
    wb_osDelay(AT_REP_WAIT_TIME_MS);
    // ATV CONFIG -END

    // ATE CONFIG -START
    if (lms_globe.at_ATE != AT_ATE_0)
    {
        if (at_cmd_sendAndRec("ATE0\r", strlen("ATE0\r"), &read_len) != AT_CMD_OK)
        {
            at_deinit();
            return AT_SYN_FAIL;
        }
        lms_globe.at_ATV = AT_ATE_0;
    }
    wb_osDelay(AT_REP_WAIT_TIME_MS);
    // ATE CONFIG -END

    // AT+CMEE CONFIG -START
    if (lms_globe.at_ATE != AT_ATE_0)
    {
        if (at_cmd_sendAndRec("AT+CMEE=1\r", strlen("AT+CMEE=1\r"), &read_len) != AT_CMD_OK)
        {
            at_deinit();
            return AT_SYN_FAIL;
        }
        lms_globe.at_ATV = AT_ATE_0;
    }
    // wb_osDelay(AT_REP_WAIT_TIME_MS);
    // AT+CMEE CONFIG -END

    return AT_SYN_OK;
}

int at_deinit()
{
    if (g_buff == HI_NULL || g_uart_buff == HI_NULL)
    {
        if (g_buff != HI_NULL)
            hi_free(HI_MOD_ID_APP_COMMON, g_buff);
        if (g_uart_buff != HI_NULL)
            hi_free(HI_MOD_ID_APP_COMMON, g_uart_buff);
    }
}

int bstainfo_payload_jsonMake(char *buff, int buff_len)
{
    //-----------bsinfo_array------------

    bsta_info *bsta = &bsta_g;
    cJSON *bsinfo_list = NULL;
    bsinfo_list = cJSON_CreateArray();

    cJSON *temp = NULL;
    for (int i = 1; bsta != NULL; i++)
    {
        temp = cJSON_CreateObject();
        cJSON_AddNumberToObject(temp, "id", i);
        cJSON_AddNumberToObject(temp, "mcc", bsta->mcc);
        cJSON_AddNumberToObject(temp, "mnc", bsta->mnc);
        cJSON_AddNumberToObject(temp, "lac", bsta->lac);
        cJSON_AddNumberToObject(temp, "ci", bsta->ci);
        cJSON_AddNumberToObject(temp, "rx_lev", bsta->rx_lev);
        cJSON_AddItemToArray(bsinfo_list, temp);
        bsta = bsta->next;
    }

    //---------------------------

    cJSON *container = NULL;
    container = cJSON_CreateObject();
    cJSON_AddNumberToObject(container, "msgid", 1);
    cJSON_AddNumberToObject(container, "cmd", 2);
    cJSON_AddItemToObject(container, "bsinfo", bsinfo_list);

    char *str = cJSON_PrintUnformatted(container);
    int str_len = strlen(str);
    if (str_len > buff_len)
    {
        cJSON_Delete(container);
        free(str);
        return -1;
    }

    strcpy(buff, str);

    cJSON_Delete(container);
    return str_len;
}

int onenet_payload_jsonMake(hi_u8 *buff, int buff_len, double lon, double lat)
{
    //---------------------------

    cJSON *value = NULL;
    value = cJSON_CreateObject();
    cJSON_AddNumberToObject(value, "lon", gip_globe.lon);
    cJSON_AddNumberToObject(value, "lat", gip_globe.lat);

    //---------------------------

    cJSON *data_p_1_1 = NULL;
    data_p_1_1 = cJSON_CreateObject();
    cJSON_AddItemToObject(data_p_1_1, "value", value);

    cJSON *datapoints = NULL;
    datapoints = cJSON_CreateArray();
    cJSON_AddItemToArray(datapoints, data_p_1_1);

    //---------------------------

    cJSON *data_p_1 = NULL;
    data_p_1 = cJSON_CreateObject();
    cJSON_AddStringToObject(data_p_1, "id", "location");
    cJSON_AddItemToObject(data_p_1, "datapoints", datapoints);

    cJSON *datastreams = NULL;
    datastreams = cJSON_CreateArray();
    cJSON_AddItemToArray(datastreams, data_p_1);

    cJSON *cjson_p1 = NULL;
    cjson_p1 = cJSON_CreateObject();
    cJSON_AddItemToObject(cjson_p1, "datastreams", datastreams);

    char *str = cJSON_PrintUnformatted(cjson_p1);
    int str_len = strlen(str);
    if (str_len > buff_len)
    {
        cJSON_Delete(cjson_p1);
        free(str);
        return -1;
    }

    strcpy(buff, str);

    cJSON_Delete(cjson_p1);
    return str_len;
}

int mixlocate_payload_jsonParse(hi_u8 *payload)
{
    cJSON *cjson_test = NULL;
    cjson_test = cJSON_Parse(payload);
    cJSON *lat_json = cJSON_GetObjectItem(cjson_test, "lat");
    cJSON *lon_json = cJSON_GetObjectItem(cjson_test, "lon");
    double lat = cJSON_GetNumberValue(lat_json);
    double lon = cJSON_GetNumberValue(lon_json);
    DBprintf("Mixlocate lat:%f,lon:%f\n", lat, lon);
    gip_globe.lat = lat;
    gip_globe.lon = lon;
    cJSON_Delete(cjson_test);
}

static hi_void at_mqtt_init()
{
    int flag = -1;
    int read_len = 0;

    at_cmd_sendAndRec("AT+CREG=2\r", strlen("AT+CREG=2\r"), &read_len);
    wb_osDelay(300);

    // mqtt init 0 onenet

    at_cmd_sendAndRec("AT+QMTCFG=\"version\",0,4\r", strlen("AT+QMTCFG=\"version\",0,4\r"), &read_len);
    wb_osDelay(300);
    at_cmd_sendAndRec("AT+QMTCFG=\"recv/mode\",0,0,1\r", strlen("AT+QMTCFG=\"recv/mode\",0,0,1\r"), &read_len);
    // wb_osDelay(300);
    // at_cmd_sendAndRec("AT+QMTCFG=\"onenet\",0,\"324618\",\"m/vvj7ceL96yuuBcF8g6YbzVoeQD0GsG4MnZIsivtGY=\"\r", strlen("AT+QMTCFG=\"onenet\",0,\"324618\",\"m/vvj7ceL96yuuBcF8g6YbzVoeQD0GsG4MnZIsivtGY=\"\r"), &read_len);
    wb_osDelay(300);
    at_cmd_sendAndRec("AT+QMTOPEN=0,\"mqtt.heclouds.com\",6002\r", strlen("AT+QMTOPEN=0,\"mqtt.heclouds.com\",6002\r"), &read_len);
    wb_osDelay(300);
    at_cmd_sendAndRec("AT+QMTCONN=0,\"1086796029\",\"606063\",\"231231\"\r", strlen("AT+QMTCONN=0,\"1086796029\",\"606063\",\"231231\"\r"), &read_len);
    wb_osDelay(300);

    // mqtt init 1 bsta

    at_cmd_sendAndRec("AT+QMTCFG=\"version\",1,4\r", strlen("AT+QMTCFG=\"version\",1,4\r"), &read_len);
    wb_osDelay(300);
    at_cmd_sendAndRec("AT+QMTCFG=\"recv/mode\",1,1,1\r", strlen("AT+QMTCFG=\"recv/mode\",1,1,1\r"), &read_len);
    // wb_osDelay(300);
    // at_cmd_sendAndRec("AT+QMTCFG=\"onenet\",0,\"324618\",\"m/vvj7ceL96yuuBcF8g6YbzVoeQD0GsG4MnZIsivtGY=\"\r", strlen("AT+QMTCFG=\"onenet\",0,\"324618\",\"m/vvj7ceL96yuuBcF8g6YbzVoeQD0GsG4MnZIsivtGY=\"\r"), &read_len);
    wb_osDelay(300);
    at_cmd_sendAndRec("AT+QMTOPEN=1,\"c17.top\",2883\r", strlen("AT+QMTOPEN=1,\"c17.top\",2883\r"), &read_len);
    wb_osDelay(300);
    at_cmd_sendAndRec("AT+QMTCONN=1,\"zhsx_01_01\",\"zhsx_01\",\"NH8bg7820hjn5\"\r", strlen("AT+QMTCONN=1,\"zhsx_01_01\",\"zhsx_01\",\"NH8bg7820hjn5\"\r"), &read_len);
    wb_osDelay(300);
    at_cmd_sendAndRec("AT+QMTSUB=1,1,\"zhsx/bsp/mixlocate\",0\r", strlen("AT+QMTSUB=1,1,\"zhsx/bsp/mixlocate\",0\r"), &read_len);
    wb_osDelay(300);
}

static hi_void at_mqtt_deinit()
{
    int flag = -1;
    int read_len = 0;
    at_cmd_sendAndRec("AT+QMTCLOSE=0\r", strlen("AT+QMTCLOSE=0\r"), &read_len);
    wb_osDelay(300);
    at_cmd_sendAndRec("AT+QMTCLOSE=1\r", strlen("AT+QMTCLOSE=1\r"), &read_len);
    wb_osDelay(300);
    at_cmd_sendAndRec("AT+QMTCLOSE=2\r", strlen("AT+QMTCLOSE=2\r"), &read_len);
    wb_osDelay(300);
}

static hi_void *at_task(hi_void *param)
{
    hi_u8 *uart_buff = g_uart_buff;
    hi_u8 *buff = g_buff;

    hi_unref_param(param);

    printf("4G init starting...\n");

    hi_u8 *mqtt_buff = NULL;
    mqtt_buff = hi_malloc(HI_MOD_ID_APP_COMMON, STR_PRO_BUFF_MAX);
    hi_u8 *mqtt_cmd_buff = NULL;
    mqtt_cmd_buff = hi_malloc(HI_MOD_ID_APP_COMMON, STR_PRO_BUFF_MAX);
    int mqtt_cmd_buff_len = 0;

    at_mqtt_init();

    int str_len = 0;
    int read_len = 0;
    int flag_cmd_end = 0;
    DBprintf("mqtt init finish\n");

    int loop = 0;
    while (1)
    {
        if (loop == 5)
        {
            loop = 0;
            if (gip_globe.status != GPS_INFO_SUCESS)
            {
                // bsta info get START
                at_rep_receive(mqtt_buff); // reflash the uart buff
                mqtt_cmd_buff_len = snprintf(mqtt_cmd_buff, STR_PRO_BUFF_MAX - 1, "AT+QCELL?\r");
                str_len = at_cmd_sendAndWaitConfirm(mqtt_cmd_buff, mqtt_cmd_buff_len, &read_len);
                if (str_len == AT_CMD_OK)
                {
                    bsta_str_process(uart_buff, read_len);
                    DBprintf("BSta_Str: %.*s\n", read_len, uart_buff);
                    DBprintf("BSta_Info_GET: mcc:%d mnc:%d ci:%d lac:%d\n", bsta_g.mcc, bsta_g.mnc, bsta_g.ci, bsta_g.lac);
                    str_len = bstainfo_payload_jsonMake(mqtt_buff, STR_PRO_BUFF_MAX);

                    mqtt_cmd_buff_len = snprintf(mqtt_cmd_buff, STR_PRO_BUFF_MAX - 1, "AT+QMTPUBEX=1,0,0,0,\"zhsx/bsp/bsinfo\",%d\r", str_len);
                    at_cmd_sendAndRec(mqtt_cmd_buff, mqtt_cmd_buff_len, &read_len);
                    wb_osDelay(200);
                    uart_send_nio(USING_AT_UART_PORT, mqtt_buff, str_len);
                    wb_osDelay(200);

                    at_rep_receive(mqtt_buff); // reflash the uart buff
                    mqtt_cmd_buff_len = snprintf(mqtt_cmd_buff, STR_PRO_BUFF_MAX - 1, "AT+QMTRECV=1\r");
                    at_cmd_sendAndRec(mqtt_cmd_buff, mqtt_cmd_buff_len, &read_len);
                    DBprintf("mixlocate payload str:%.*s;\n", read_len, uart_buff);
                    mixlocate_str_process(uart_buff, read_len, mqtt_cmd_buff, STR_PRO_BUFF_MAX, &str_len);
                    mqtt_cmd_buff[str_len] = '\0';
                    DBprintf("mixlocate json str:%.*s;\n", str_len, mqtt_cmd_buff);
                    mixlocate_payload_jsonParse(mqtt_cmd_buff);
                }
                // bsta info get END
            }
        }
        else
        {
            loop++;
        }

        // onenet_location_data_send_START

        // 生成要传递的onenet数据
        str_len = onenet_payload_jsonMake(mqtt_buff + 3, STR_PRO_BUFF_MAX - 3, gip_globe.lat, gip_globe.lon);
        DBprintf("mqtt payload str:%.*s;len:%d;[1]:%d;[2]:%d\n", str_len, mqtt_buff + 3, str_len, str_len / 0x100, str_len % 0x100);
        if (str_len == -1)
            break;
        mqtt_buff[0] = 0x01;
        mqtt_buff[1] = str_len / 0x100;
        mqtt_buff[2] = str_len % 0x100;

        mqtt_cmd_buff_len = snprintf(mqtt_cmd_buff, STR_PRO_BUFF_MAX - 1, "AT+QMTPUBEX=0,0,0,0,\"$dp\",%d\r", str_len + 3);
        flag_cmd_end = at_cmd_sendAndRec(mqtt_cmd_buff, mqtt_cmd_buff_len, &read_len);

        // if( flag_cmd_end == AT_CMD_FALI ){
        //     at_mqtt_deinit();
        //     at_mqtt_init();
        // }else{
            wb_osDelay(200);
            uart_send_nio(USING_AT_UART_PORT, mqtt_buff, str_len + 3);
            DBprintf("mqtt send x1\n");
        // }



        // onenet_location_data_send_END

        wb_osDelay(2000);
    }

    at_deinit();
    hi_task_delete(g_uart_task_id);
    g_uart_task_id = 0;

    return HI_NULL;
}

/// @brief uart task entry ; create a thread for UART
/// @param
void at_entry(void)
{

    osThreadAttr_t attr;

    attr.name = "uart_entry";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = AT_FUN_TASK_STAK_SIZE;
    attr.priority = AT_FUN_TASK_PRIORITY;

    if (osThreadNew((osThreadFunc_t)at_task, HI_NULL, &attr) == NULL)
    {
        printf("[sys_info] Falied to create uart_task!\n");
    }
}

// SYS_RUN(uart_entry);

// UART FOR GPS MODLE END
