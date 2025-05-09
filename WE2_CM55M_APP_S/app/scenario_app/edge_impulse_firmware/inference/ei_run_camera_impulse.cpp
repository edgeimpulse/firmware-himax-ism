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

#include "model-parameters/model_metadata.h"
#if defined(EI_CLASSIFIER_SENSOR) && (EI_CLASSIFIER_SENSOR == EI_CLASSIFIER_SENSOR_CAMERA)
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "firmware-sdk/ei_camera_interface.h"
#include "firmware-sdk/at_base64_lib.h"
#include "firmware-sdk/jpeg/encode_as_jpg.h"
#include "ei_run_impulse.h"
#include "ei_camera_we2.h"
#include "ei_device_we2.h"

static bool debug_mode = false;
bool inference_running = false;
static uint8_t *image_buf = nullptr;
static uint32_t image_buf_size;

static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr)
{
    // we already have a RGB888 buffer, so recalculate offset into pixel index
    size_t pixel_ix = offset;
    size_t pixels_left = length;
    size_t out_ptr_ix = 0;

    while (pixels_left != 0)
    {
        out_ptr[out_ptr_ix] = (image_buf[pixel_ix] << 16) + (image_buf[pixel_ix + 1] << 8) + image_buf[pixel_ix + 2];

        // go to the next pixel
        out_ptr_ix++;
        pixel_ix += 1;
        pixels_left--;
    }

    // and done!
    return 0;
}

/**
 * This callback is going to be called by camera driver when the frame is ready
 * and it was already rescaled and cropped to the dimensions required by the model
 */
void ei_run_impulse(void)
{
    int res;
    EiCameraWE2 *cam = static_cast<EiCameraWE2 *>(EiCameraWE2::get_camera());

    // tjis is already done in the camera_event_cb
    ei_printf("Taking photo...\n");

    cam->take_inference_snapshot(image_buf, image_buf_size, EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);

    ei::signal_t signal;
    signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
    signal.get_data = &ei_camera_get_data;

    // print and discard JPEG buffer before inference to free some memory
    if (debug_mode)
    {
        ei_printf("Begin output\n");
        ei_printf("Framebuffer: ");
        encode_bw_signal_as_jpg_and_output_base64(&signal, EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);
        ei_printf("\r\n");
    }

    // run the impulse: DSP, neural network and the Anomaly algorithm
    ei_impulse_result_t result = {0};

    EI_IMPULSE_ERROR ei_error = run_classifier(&signal, &result, false);
    if (ei_error != EI_IMPULSE_OK)
    {
        ei_printf("ERR: Failed to run impulse (%d)\n", ei_error);
        return;
    }

    display_results(&ei_default_impulse, &result);

    if (debug_mode)
    {
        ei_printf("End output\n");
    }
}

bool ei_start_impulse(bool continuous, bool debug, bool use_max_uart_speed)
{
    EiCameraWE2 *cam = static_cast<EiCameraWE2 *>(EiCameraWE2::get_camera());
    auto dev = EiDeviceWE2::get_device();

    debug_mode = debug;

    if (cam->is_camera_present() == false) {
        ei_printf("ERR: Failed to start inference, camera is missing!\n");
        return false;
    }

    if (cam->init(EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT) == false) {
        return false;
    }

    image_buf_size = EI_CLASSIFIER_NN_INPUT_FRAME_SIZE;
    image_buf = (uint8_t *)ei_malloc(image_buf_size);
    // check if allocation was succesful
    if (image_buf == nullptr)
    {
        ei_printf("ERR: Failed to allocate snapshot buffer!\n");
        return false;
    }

    // summary of inferencing settings (from model_metadata.h)
    ei_printf("Inferencing settings:\n");
    ei_printf("\tImage resolution: %dx%d\n", EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);
    ei_printf("\tFrame size: %d\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
    ei_printf("\tNo. of classes: %d\n", sizeof(ei_classifier_inferencing_categories) / sizeof(ei_classifier_inferencing_categories[0]));

    if (use_max_uart_speed && debug_mode) {
        ei_printf("OK\r\n");
        ei_sleep(100);
        dev->set_max_data_output_baudrate();
        ei_sleep(100);
    }

    inference_running = true;

    return true;
}

bool is_inference_running(void)
{
    return inference_running;
}

void ei_stop_impulse(void)
{
    auto dev = EiDeviceWE2::get_device();

    ei_free(image_buf);

    if (debug_mode) {
        ei_printf("OK\r\n");
        ei_sleep(100);
        dev->set_default_data_output_baudrate();
        ei_sleep(100);
    }

    inference_running = false;
}

#endif /* defined(EI_CLASSIFIER_SENSOR) && EI_CLASSIFIER_SENSOR == EI_CLASSIFIER_SENSOR_CAMERA */
