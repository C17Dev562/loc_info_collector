/*
 * @Author: C17_Dev lijialiangww@gmail.com
 * @Date: 2023-05-13 12:27:33
 * @LastEditors: C17_Dev lijialiangww@gmail.com
 * @LastEditTime: 2023-05-23 13:35:11
 * @FilePath: /OpenHarmony-v3.1-Release/applications/sample/wifi-iot/app/wb_app/app/config/wb_config.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#ifndef _WB_CONFIG_
#define _WB_CONFIG_

#define DEBUG_ON

#ifdef DEBUG_ON
#define DBprintf(...) printf(__VA_ARGS__)
#else
#define DBprintf(...)
#endif

#endif

#pragma endregion
