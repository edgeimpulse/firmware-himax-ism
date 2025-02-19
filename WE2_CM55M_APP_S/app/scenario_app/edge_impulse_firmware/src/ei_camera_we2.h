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

#ifndef EI_CAMERA_WE2_H
#define EI_CAMERA_WE2_H
#include "firmware-sdk/ei_camera_interface.h"
#include "firmware-sdk/ei_device_info_lib.h"
#include "app_datapath.h"

typedef enum {
    CAM_STATE_IDLE = 0,
    CAM_STATE_STREAM_ACTIVE,
    CAM_STATE_SNAPSHOT_PREPARE,
    CAM_STATE_SNAPSHOT,
    CAM_STATE_INFERENCING,
} EiCameraState_t;

class EiCameraWE2 : public EiCamera {

private:
    static ei_device_snapshot_resolutions_t resolutions[];
    uint32_t width;
    uint32_t height;
    uint32_t inference_scaled_fb_width;
    uint32_t inference_scaled_fb_height;
    uint8_t *stream_buffer;
    uint32_t stream_buffer_size;
    bool camera_present;
    bool stream_active;
    EiCameraState_t state;
    const uint8_t frames_to_drop = 20;
    void (*run_impulse_callback)(void);
    app_dp_cap_t app_dp_cap_set;

public:
    EiCameraWE2();
    bool init(uint16_t width, uint16_t height) override;
    bool set_resolution(const ei_device_snapshot_resolutions_t res);
    ei_device_snapshot_resolutions_t get_min_resolution(void);
    bool is_camera_present(void);
    void get_resolutions(ei_device_snapshot_resolutions_t **res, uint8_t *res_num);
    void take_snapshot(uint32_t width, uint32_t height);
    void take_inference_snapshot(uint8_t* frame_buffer, const uint32_t frame_buffer_size, const uint16_t req_width, const uint16_t req_height);

    bool start_stream(uint32_t width, uint32_t height);
    bool run_stream(void);
    bool is_stream_active(void);
    bool stop_stream(void);
};

#endif /* EI_CAMERA_WE2_H */
