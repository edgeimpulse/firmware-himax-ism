/* The Clear BSD License
 *
 * Copyright (c) 2025 EdgeImpulse Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the disclaimer
 * below) provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 *   * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstdio>
#include <cstdint>
#include "firmware-sdk/at-server/ei_at_server.h"
#include "xprintf.h"
#include "hx_drv_scu.h"
#include "ei_at_handlers.h"
#include "app_datapath.h"
#include "app_board.h"
#include "ei_device_we2.h"
#include "ei_run_impulse.h"
extern "C" {
    #include <hx_drv_uart.h>
	#include "hx_drv_pmu.h"
	#include "timer_interface.h"
};
#include "WE2_core.h"
#include "ethosu_driver.h"
#ifdef FLASH_AS_SRAM
#include "app_flash.h"
#include "spi_eeprom_comm.h"
#endif

#ifdef TRUSTZONE_SEC
#define U55_BASE	BASE_ADDR_APB_U55_CTRL_ALIAS
#else
#ifndef TRUSTZONE
#define U55_BASE	BASE_ADDR_APB_U55_CTRL_ALIAS
#else
#define U55_BASE	BASE_ADDR_APB_U55_CTRL
#endif
#endif

struct ethosu_driver ethosu_drv; /* Default Ethos-U device driver */

// Callback function declaration
static int get_signal_data(size_t offset, size_t length, float *out_ptr);

#if defined(EI_CLASSIFIER_SENSOR) && (EI_CLASSIFIER_SENSOR == EI_CLASSIFIER_SENSOR_MICROPHONE)
// audio raw samples are ususally collected in int16_t format
// this is an example of helloworld recording
// should be classified with confidence 0.84
#else
static const float features[] = {};
#endif

static void _arm_npu_irq_handler(void)
{
    /* Call the default interrupt handler from the NPU driver */
    ethosu_irq_handler(&ethosu_drv);
}

static void _arm_npu_irq_init(void)
{
    const IRQn_Type ethosu_irqnum = (IRQn_Type)U55_IRQn;

    /* Register the EthosU IRQ handler in our vector table.
     * Note, this handler comes from the EthosU driver */
    EPII_NVIC_SetVector(ethosu_irqnum, (uint32_t)_arm_npu_irq_handler);

    /* Enable the IRQ */
    NVIC_EnableIRQ(ethosu_irqnum);

}

static int _arm_npu_init(bool security_enable, bool privilege_enable)
{
    int err = 0;

    /* Initialise the IRQ */
    _arm_npu_irq_init();

    /* Initialise Ethos-U55 device */
    void * const ethosu_base_address = (void *)(U55_BASE);

    if (0 != (err = ethosu_init(
                            &ethosu_drv,             /* Ethos-U driver device pointer */
                            ethosu_base_address,     /* Ethos-U NPU's base address. */
                            NULL,       /* Pointer to fast mem area - NULL for U55. */
                            0, /* Fast mem region size. */
							security_enable,                       /* Security enable. */
							privilege_enable))) {                   /* Privilege enable. */
    	ei_printf("failed to initalise Ethos-U device\n");
            return err;
        }

    ei_printf("Ethos-U55 device initialised\n");

    return 0;
}

extern "C" int app_main(void)
{
    int c;
    ATServer *at;
	EiDeviceWE2 *device = static_cast<EiDeviceWE2*>(EiDeviceInfo::get_device());
    EiCameraWE2* cam = static_cast<EiCameraWE2*>(EiCameraWE2::get_camera());
	uint32_t wakeup_event;
	uint32_t wakeup_event1;
    DEV_UART* console_uart;
    size_t uart_read_count;

	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT, &wakeup_event);
	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT1, &wakeup_event1);

#ifdef WE2_DUAL_CORE
    ei_printf("CM55M_ENABLE_CM55S \n\n\n");
    hx_drv_scu_set_cm55s_state(SCU_CM55S_STATE_RESET);
    hx_drv_scu_set_cm55s_state(SCU_CM55S_STATE_NORMAL);
    hx_drv_scu_set_CM55S_CPUWAIT(SCU_CM55_CPU_RUN);

    ei_printf("app_main start(dual core).\n");
#endif

#ifdef FLASH_AS_SRAM
    flash_init();
#endif

    /* PINMUX. */
    app_board_pinmux_settings();

	if(_arm_npu_init(true, true) !=0 ) {
		ei_printf("Faield to init NPU\n");
	}

    console_uart = hx_drv_uart_get_dev(USE_DW_UART_0);
    console_uart->uart_open(UART_BAUDRATE_921600);

	at = ei_at_init(device);
 	at->print_prompt();

    while(1)
    {
		uart_read_count = console_uart->uart_read_nonblock(&c, 1);
		if(uart_read_count > 0) {
            if(cam->is_stream_active() && (char)c =='b') {
                // dameon is sending 'b\r', we need to discard \r
                console_uart->uart_control(UART_CMD_FLUSH_RX_BUF, 0);
                cam->stop_stream();
                at->print_prompt();
                continue;
            }

            if(is_inference_running() && (char)c == 'b') {
                // dameon is sending 'b\r', we need to discard \r
                console_uart->uart_control(UART_CMD_FLUSH_RX_BUF, 0);
                ei_stop_impulse();
                at->print_prompt();
                continue;
            }
            at->handle(c);
		}

        if(cam->is_stream_active()) {
            cam->run_stream();
        }

		if(is_inference_running() == true) {
			ei_run_impulse();
		}

		hx_drv_timer_cm55x_delay_ms(1, TIMER_STATE_DC);
	}

    return 0;
}
