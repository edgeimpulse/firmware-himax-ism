APPL_API_ROOT = $(SCENARIO_APP_ROOT)/app_api

USE_APP_MAIN_MK = y
USE_APP_API = y

APPL_API_INCDIRS = $(APPL_API_ROOT)/include
APPL_API_INCDIRS += $(APPL_API_ROOT)/include/app_peripheral

APPL_API_SRC_LIST = \
	src/app_api.c \
	src/bypass_sensor_reg.c \
	src/app_sys_info_cmd.c \
	src/app_pmu.c \
	src/app_datapath.c \
	src/app_mipi.c \
	src/app_flash.c \
	src/app_memory.c \
	src/app_board.c

APPL_API_SRC_LIST += \
    src/evt_reboot_api.c \
    src/ota_update.c

APPL_API_SRC_LIST += \
	src/app_peripheral/app_i2c_m.c \
	src/app_peripheral/app_uart.c \
	src/app_peripheral/app_spi_m.c \
	src/app_peripheral/app_spi_s.c \
	src/app_peripheral/app_adcc.c

ifeq ($(FREERTOS_SUPPORT), 1)
APPL_API_SRC_LIST += \
    src/freertos_app.c
endif

ifeq ($(CIS), HM01B0_MONO)
APPL_API_INCDIRS += $(APPL_API_ROOT)/include/hm01b0_mono
APPL_API_SRC_LIST += src/hm01b0_mono/app_cisdp_cfg.c
endif

ifeq ($(CIS), HM11B1_MONO)
APPL_API_INCDIRS += $(APPL_API_ROOT)/include/hm11b1_mono
APPL_API_SRC_LIST += src/hm11b1_mono/app_cisdp_cfg.c
endif

ifeq ($(CIS), HM0360_BAYER)
APPL_API_INCDIRS += $(APPL_API_ROOT)/include/hm0360_bayer
APPL_API_SRC_LIST += src/hm0360_bayer/app_cisdp_cfg.c
endif

ifeq ($(CIS), HM0360_MONO)
APPL_API_INCDIRS += $(APPL_API_ROOT)/include/hm0360_mono
APPL_API_SRC_LIST += src/hm0360_mono/app_cisdp_cfg.c
endif

ifeq ($(CIS), HM0360_MIPI)
APPL_API_INCDIRS += $(APPL_API_ROOT)/include/hm0360_mipi
APPL_API_SRC_LIST += src/hm0360_mipi/app_cisdp_cfg.c
APPL_DEFINES += -DAPP_MIPI_CIS
endif

ifeq ($(CIS), HM2170_BAYER)
APPL_API_INCDIRS += $(APPL_API_ROOT)/include/hm2170_bayer
APPL_API_SRC_LIST += src/hm2170_bayer/app_cisdp_cfg.c
endif

ifeq ($(CIS), HM2170_MIPI)
APPL_API_INCDIRS += $(APPL_API_ROOT)/include/hm2170_mipi
APPL_API_SRC_LIST += src/hm2170_mipi/app_cisdp_cfg.c
APPL_DEFINES += -DAPP_MIPI_CIS
endif

ifeq ($(CIS), MIPI_CSI_RX_INTF)
APPL_API_INCDIRS += $(APPL_API_ROOT)/include/mipi_csi_rx_intf
APPL_API_SRC_LIST += src/mipi_csi_rx_intf/app_cisdp_cfg.c
endif

ifeq ($(CIS), VIRTUAL_CISDP_INTF)
APPL_API_INCDIRS += $(APPL_API_ROOT)/include/virtual_cisdp_intf
endif

ifneq ($(CIS), )
APPL_DEFINES += -D$(CIS)
endif

ifeq ($(I2S_SPEAKER), y)
APPL_API_SRC_LIST += \
	src/app_speaker.c
endif

ifeq ($(PDM_REC), y)
APPL_API_SRC_LIST += \
	src/app_pdm.c
endif

ifeq ($(AUTO_PREROLL), y)
APPL_API_SRC_LIST += \
	src/app_preroll.c
endif

ifeq ($(CORE_SETTING), DUAL_CORE)
APPL_API_SRC_LIST += src/app_evt_mb.c
endif

APPL_API_SRCS = $(addprefix $(APPL_API_ROOT)/, $(APPL_API_SRC_LIST))

####### application definitions ######

## common
APPL_DEFINES += -DAPP_API
APPL_DEFINES += -D$(EVK)

ifeq ($(EVK), HX6538_ISM028_03M)
	APPL_DEFINES += -DHM_INTER_LDO_EN
endif

