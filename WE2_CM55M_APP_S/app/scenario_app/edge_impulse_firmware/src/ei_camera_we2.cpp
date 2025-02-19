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

#include "ei_camera_we2.h"
#include "ei_device_we2.h"
#include "model-parameters/model_metadata.h"
#include "edge-impulse-sdk/porting/ei_classifier_porting.h"
#include "edge-impulse-sdk/dsp/image/image.hpp"
#include "firmware-sdk/at_base64_lib.h"
#include <cmath>
#include <cstdint>
#include "console_io.h"
#include "img_proc_helium.h"
#include "app_datapath.h"
#include "app_xdma_cfg.h"
#include "xprintf.h"
extern "C" {
    #include <hx_drv_uart.h>
};

ei_device_snapshot_resolutions_t EiCameraWE2::resolutions[] = {
        { .width = 640, .height = 480 },
    };

static volatile uint8_t g_wdma1_data[0/*WDMA1_SIZE*/] = {};
static volatile uint8_t g_wdma2_data[WDMA2_SLOT_SIZE*WDMA2_CYCLIC_BUF_CNT] = {};
static volatile uint8_t g_wdma3_data[WDMA3_SIZE] = {};
static volatile uint8_t g_jpeg_autofill_data[JPEG_AUTOFILL_SIZE] = {};

EiCameraWE2::EiCameraWE2()
{
    this->camera_present = true;
    this->state = CAM_STATE_IDLE;
}

bool EiCameraWE2::init(uint16_t width, uint16_t height)
{
    app_dp_cfg_t app_dp_init_cfg;

    /* Set DP settings */
    app_dp_get_def_cap_settings(&this->app_dp_cap_set);

    //
    app_dp_get_def_init_cfg(&app_dp_init_cfg);

    /* Sensor */
    app_sensor_init(&app_dp_init_cfg);

    /* Datapath. */
    app_dp_init_cfg.wdma1 = (uint32_t)g_wdma1_data;
    app_dp_init_cfg.wdma2 = (uint32_t)g_wdma2_data;
    app_dp_init_cfg.wdma3 = (uint32_t)g_wdma3_data;
    app_dp_init_cfg.jpeg_auto_fill_data = (uint32_t)g_jpeg_autofill_data;
    app_dp_init_cfg.wdma2_cyclic_buffer_cnt = (uint8_t)WDMA2_CYCLIC_BUF_CNT;
    app_dp_init(&app_dp_init_cfg);

    return true;
}

bool EiCameraWE2::set_resolution(const ei_device_snapshot_resolutions_t res)
{
    width = res.width;
    height = res.height;

    return true;
}

ei_device_snapshot_resolutions_t EiCameraWE2::get_min_resolution(void)
{
    return resolutions[0];
}

EiCamera* EiCamera::get_camera(void)
{
    static EiCameraWE2 cam;

    return &cam;
}

bool EiCameraWE2::is_camera_present(void)
{
    return camera_present;
}

void EiCameraWE2::get_resolutions(ei_device_snapshot_resolutions_t **res, uint8_t *res_num)
{
    *res = &EiCameraWE2::resolutions[0];
    *res_num = sizeof(EiCameraWE2::resolutions) / sizeof(ei_device_snapshot_resolutions_t);
}

bool EiCameraWE2::start_stream(uint32_t width, uint32_t height)
{
    // try to set required resolution, returned is what has been set
    ei_device_snapshot_resolutions_t sensor_res = {width, height};
    auto dev = EiDeviceWE2::get_device();

    this->init(width, height);

    if(set_resolution(sensor_res) == 0) {
        ei_printf("ERR: Failed to set camera resolution!\n");
        return false;
    }

    this->stream_active = true;

    return true;
}

bool EiCameraWE2::run_stream(void)
{
    uint32_t raw_img_addr = 0, img_width = 0, img_heigh = 0;
    uint32_t jpg_addr = 0, jpg_size = 0;

    if(this->stream_active == false) {
        return false;
    }

    /* Capture jpeg and raw image */
    app_dp_capture_frame(&this->app_dp_cap_set);

    /* Capture sensor output to jpeg and raw */
    app_dp_get_frame(&raw_img_addr, &img_width, &img_heigh, &jpg_addr, &jpg_size);

    hx_InvalidateDCache_by_Addr((void *)jpg_addr, jpg_size);
    base64_encode((char*)jpg_addr, jpg_size, xputc);
    ei_printf("\r\n");

    return true;
}

bool EiCameraWE2::is_stream_active(void)
{
    return this->stream_active;
}

void EiCameraWE2::take_inference_snapshot(uint8_t* frame_buffer, const uint32_t frame_buffer_size, const uint16_t req_width, const uint16_t req_height)
{
    uint32_t raw_img_addr = 0, img_width = 0, img_heigh = 0;
    uint32_t jpg_addr = 0, jpg_size = 0;

    const uint32_t scale_factor = SENCTRL_SENSOR_HEIGHT / req_height;
    const uint32_t inference_scaled_fb_width = SENCTRL_SENSOR_WIDTH / scale_factor;
    const uint32_t inference_scaled_fb_height = req_height;

    // Allocate the frame buffer for the rescaled image
    // TODO: allocate *3 or *1 depending on camera color/mono mode
    size_t scaled_frame_buffer_size = inference_scaled_fb_width * inference_scaled_fb_height * 1;
    uint8_t *scaled_frame_buffer = (uint8_t*)ei_malloc(scaled_frame_buffer_size);
    if(scaled_frame_buffer == nullptr) {
        ei_printf("ERR: Failed to allocate frame buffer! (%u B)\n", scaled_frame_buffer_size);
        return;
    }

    /* Capture jpeg and raw image */
    app_dp_capture_frame(&this->app_dp_cap_set);

    /* Capture sensor output to jpeg and raw */
    app_dp_get_frame(&raw_img_addr, &img_width, &img_heigh, &jpg_addr, &jpg_size);
    hx_InvalidateDCache_by_Addr((void *)jpg_addr, jpg_size);

	// Resize image to fit model input
    //TODO: adjust channels based on camera color mode
    float scale = (float)(img_heigh - 1) / (req_height - 1);
	hx_lib_image_resize_helium((uint8_t*)raw_img_addr, scaled_frame_buffer, img_width, img_heigh, 1, inference_scaled_fb_width, inference_scaled_fb_height, scale, scale);

    const uint32_t start_x = (inference_scaled_fb_width - req_width) / 2;
    hx_lib_image_copy_helium(scaled_frame_buffer, frame_buffer, inference_scaled_fb_width, inference_scaled_fb_height, 1, start_x, 0, req_width, req_height);

    ei_free(scaled_frame_buffer);
}

void EiCameraWE2::take_snapshot(uint32_t width, uint32_t height)
{
    // try to set required resolution, returned is what has been set
    ei_device_snapshot_resolutions_t sensor_res = {width, height};
    auto dev = EiDeviceWE2::get_device();
    uint32_t raw_img_addr = 0, img_width = 0, img_heigh = 0;
    uint32_t jpg_addr = 0, jpg_size = 0;

    this->init(width, height);

    if(set_resolution(sensor_res) == 0) {
        ei_printf("ERR: Failed to set camera resolution!\n");
        return;
    }

    /* Capture jpeg and raw image */
    app_dp_capture_frame(&this->app_dp_cap_set);

    /* Capture sensor output to jpeg and raw */
    app_dp_get_frame(&raw_img_addr, &img_width, &img_heigh, &jpg_addr, &jpg_size);

    hx_InvalidateDCache_by_Addr((void *)jpg_addr, jpg_size);
    base64_encode((char*)jpg_addr, jpg_size, xputc);
    ei_printf("\r\n");
}

bool EiCameraWE2::stop_stream(void)
{
    auto dev = EiDeviceWE2::get_device();

    ei_printf("OK\r\n");
    // we can call it even if the baudrate wasn't changed
    dev->set_default_data_output_baudrate();
    ei_sleep(100);
    ei_printf("Snapshot streaming stopped by user\n");
    ei_printf("OK\r\n");

    this->deinit();

    this->stream_active = false;

    return true;
}
