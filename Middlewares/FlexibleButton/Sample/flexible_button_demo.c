/**
 * @File:    flexible_button_demo.c
 * @Author:  MurphyZhao
 * @Date:    2018-09-29
 * 
 * Copyright (c) 2018-2019 MurphyZhao <d2014zjt@163.com>
 *               https://github.com/murphyzhao
 * All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Change logs:
 * Date        Author       Notes
 * 2018-09-29  MurphyZhao   First add
 * 2019-08-02  MurphyZhao   迁移代码到 murphyzhao 仓库
*/
#include "bsp_pb.h"

#include "OSAL.h"

#include "OSAL_Debug.h"
#include "OSAL_Printf.h"

#include "flexible_button.h"
#include <stdint.h>

typedef enum
{
    USER_BUTTON_0 = 0,
    USER_BUTTON_1,
    USER_BUTTON_2,
    USER_BUTTON_MAX
} user_button_t;

static flex_button_t user_button[USER_BUTTON_MAX];

static void btn_0_cb(flex_button_t *btn)
{
    printf("btn_0_cb\n");
    switch (btn->event)
    {
        case FLEX_BTN_PRESS_DOWN:
            printf("btn_0_cb [FLEX_BTN_PRESS_DOWN]\n");
            break;
        case FLEX_BTN_PRESS_CLICK:
            printf("btn_0_cb [FLEX_BTN_PRESS_CLICK]\n");
            break;
        case FLEX_BTN_PRESS_DOUBLE_CLICK:
            printf("btn_0_cb [FLEX_BTN_PRESS_DOUBLE_CLICK]\n");
            break;
        case FLEX_BTN_PRESS_SHORT_START:
            printf("btn_0_cb [FLEX_BTN_PRESS_SHORT_START]\n");
            break;
        case FLEX_BTN_PRESS_SHORT_UP:
            printf("btn_0_cb [FLEX_BTN_PRESS_SHORT_UP]\n");
            break;
        case FLEX_BTN_PRESS_LONG_START:
            printf("btn_0_cb [FLEX_BTN_PRESS_LONG_START]\n");
            break;
        case FLEX_BTN_PRESS_LONG_UP:
            printf("btn_0_cb [FLEX_BTN_PRESS_LONG_UP]\n");
            break;
        case FLEX_BTN_PRESS_LONG_HOLD:
            printf("btn_0_cb [FLEX_BTN_PRESS_LONG_HOLD]\n");
            break;
        case FLEX_BTN_PRESS_LONG_HOLD_UP:
            printf("btn_0_cb [FLEX_BTN_PRESS_LONG_HOLD_UP]\n");
            break;
    }
}

static void btn_1_cb(flex_button_t *btn)
{
    printf("btn_1_cb\n");
    switch (btn->event)
    {
        case FLEX_BTN_PRESS_DOWN:
            printf("btn_1_cb [FLEX_BTN_PRESS_DOWN]\n");
            break;
        case FLEX_BTN_PRESS_CLICK:
            printf("btn_1_cb [FLEX_BTN_PRESS_CLICK]\n");
            break;
        case FLEX_BTN_PRESS_DOUBLE_CLICK:
            printf("btn_1_cb [FLEX_BTN_PRESS_DOUBLE_CLICK]\n");
            break;
        case FLEX_BTN_PRESS_SHORT_START:
            printf("btn_1_cb [FLEX_BTN_PRESS_SHORT_START]\n");
            break;
        case FLEX_BTN_PRESS_SHORT_UP:
            printf("btn_1_cb [FLEX_BTN_PRESS_SHORT_UP]\n");
            break;
        case FLEX_BTN_PRESS_LONG_START:
            printf("btn_1_cb [FLEX_BTN_PRESS_LONG_START]\n");
            break;
        case FLEX_BTN_PRESS_LONG_UP:
            printf("btn_1_cb [FLEX_BTN_PRESS_LONG_UP]\n");
            break;
        case FLEX_BTN_PRESS_LONG_HOLD:
            printf("btn_1_cb [FLEX_BTN_PRESS_LONG_HOLD]\n");
            break;
        case FLEX_BTN_PRESS_LONG_HOLD_UP:
            printf("btn_1_cb [FLEX_BTN_PRESS_LONG_HOLD_UP]\n");
            break;
    }
}

static void btn_2_cb(flex_button_t *btn)
{
    printf("btn_2_cb\n");
    switch (btn->event)
    {
        case FLEX_BTN_PRESS_DOWN:
            printf("btn_2_cb [FLEX_BTN_PRESS_DOWN]\n");
            break;
        case FLEX_BTN_PRESS_CLICK:
            printf("btn_2_cb [FLEX_BTN_PRESS_CLICK]\n");
            break;
        case FLEX_BTN_PRESS_DOUBLE_CLICK:
            printf("btn_2_cb [FLEX_BTN_PRESS_DOUBLE_CLICK]\n");
            break;
        case FLEX_BTN_PRESS_SHORT_START:
            printf("btn_2_cb [FLEX_BTN_PRESS_SHORT_START]\n");
            break;
        case FLEX_BTN_PRESS_SHORT_UP:
            printf("btn_2_cb [FLEX_BTN_PRESS_SHORT_UP]\n");
            break;
        case FLEX_BTN_PRESS_LONG_START:
            printf("btn_2_cb [FLEX_BTN_PRESS_LONG_START]\n");
            break;
        case FLEX_BTN_PRESS_LONG_UP:
            printf("btn_2_cb [FLEX_BTN_PRESS_LONG_UP]\n");
            break;
        case FLEX_BTN_PRESS_LONG_HOLD:
            printf("btn_2_cb [FLEX_BTN_PRESS_LONG_HOLD]\n");
            break;
        case FLEX_BTN_PRESS_LONG_HOLD_UP:
            printf("btn_2_cb [FLEX_BTN_PRESS_LONG_HOLD_UP]\n");
            break;
    }
}

static uint8_t button_key0_read(void)
{
    return BSP_PB_GetState(BUTTON_A);
}

static uint8_t button_key1_read(void)
{
    return BSP_PB_GetState(BUTTON_M);
}

static uint8_t button_key2_read(void)
{
    return BSP_PB_GetState(BUTTON_B);
}

void user_button_init(void)
{
    int i;

    BSP_PB_Init(BUTTON_A, BUTTON_MODE_GPIO);
    BSP_PB_Init(BUTTON_M, BUTTON_MODE_GPIO);
    BSP_PB_Init(BUTTON_B, BUTTON_MODE_GPIO);

    osal_memset(&user_button[0], 0x0, sizeof(user_button));

    user_button[USER_BUTTON_0].usr_button_read = button_key0_read;
    user_button[USER_BUTTON_0].cb = (flex_button_response_callback)btn_0_cb;

    user_button[USER_BUTTON_1].usr_button_read = button_key1_read;
    user_button[USER_BUTTON_1].cb = (flex_button_response_callback)btn_1_cb;

    user_button[USER_BUTTON_2].usr_button_read = button_key2_read;
    user_button[USER_BUTTON_2].cb = (flex_button_response_callback)btn_2_cb;

    for (i = 0; i < USER_BUTTON_MAX; i ++)
    {
        user_button[i].pressed_logic_level = 0;
        user_button[i].click_start_tick = 20;
        user_button[i].short_press_start_tick = 100;
        user_button[i].long_press_start_tick = 200;
        user_button[i].long_hold_start_tick = 300;

        flex_button_register(&user_button[i]);
    }
}

