SCENARIO_APP_SUPPORT_LIST := $(APP_TYPE)

FLASH_AS_SRAM_ENABLE = 1

ifeq ($(FLASH_AS_SRAM_ENABLE), 1)
    APPL_DEFINES += -DFLASH_AS_SRAM
endif

###################
## Sensor Option ##
###################
#CIS = HM11B1_MONO
# CIS = HM0360_BAYER
CIS = HM0360_MONO
#CIS = HM2170_BAYER

#APPL_DEFINES += -DAIOT_NB_EXAMPLE_TZ_S_ONLY
#APPL_DEFINES += -DEVT_CM55MMB_NBAPP
#APPL_DEFINES += -DEVT_DATAPATH
#APPL_DEFINES += -DEVT_CM55MTIMER
#APPL_DEFINES += -DCM55M_ENABLE_CM55S
#APPL_DEFINES += -DENABLE_EVENT_IDLE_WFI
#APPL_DEFINES += -DTF_LITE_STATIC_MEMORY
APPL_DEFINES += -DEI_PORTING_HIMAX_WE2
APPL_DEFINES += -DEI_ETHOS
APPL_DEFINES += -DETHOSU_ARCH=u55
APPL_DEFINES += -DETHOSU55
APPL_DEFINES += -DETHOS_U
APPL_DEFINES += -DEI_STANDALONE_INFERENCING
APPL_DEFINES += -DEI_MODEL_SECTION=".ei_model"

#For PMU dump register
#APPL_DEFINES += -DLIBPWRMGMT_PMUDUMP
#APPL_DEFINES += -DAPP_PMU_REG_EXP

#EVENTHANDLER_SUPPORT = event_handler
#EVENTHANDLER_SUPPORT_LIST += evt_datapath
#EVENTHANDLER_SUPPORT_LIST += evt_cm55mmb_nbapp
#EVENTHANDLER_SUPPORT_LIST += evt_cm55mtimer
#EVENTHANDLER_SUPPORT_LIST += evt_i2ccomm

USE_APP_MAIN_MK = y

SCENARIO_APP_SUPPORT_LIST +=	edge_impulse_firmware/src \
								edge_impulse_firmware/inference \
								edge_impulse_firmware/ei-model/tflite-model \
								edge_impulse_firmware/ei-model \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/classifier \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/BasicMathFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/BayesFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/CommonTables \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/ComplexMathFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/ControllerFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/DistanceFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/FastMathFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/FilteringFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/InterpolationFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/MatrixFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/QuaternionMathFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/SVMFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/StatisticsFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/SupportFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/DSP/Source/TransformFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/NN/Source/ActivationFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/NN/Source/BasicMathFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/NN/Source/ConcatenationFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/NN/Source/ConvolutionFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/NN/Source/FullyConnectedFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/NN/Source/NNSupportFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/NN/Source/PoolingFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/NN/Source/ReshapeFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/NN/Source/SVDFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/CMSIS/NN/Source/SoftmaxFunctions \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/porting/ethos-core-driver/include \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/porting/ethos-core-driver/src \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/dsp/kissfft \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/dsp/dct \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/dsp/image \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/porting \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/porting/himax-we2 \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/tensorflow/lite/ \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/tensorflow/lite/c \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/tensorflow/lite/core/api \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/tensorflow/lite/kernels \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/tensorflow/lite/kernels/internal \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/tensorflow/lite/micro \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/tensorflow/lite/micro/kernels \
								edge_impulse_firmware/ei-model/edge-impulse-sdk/tensorflow/lite/micro/memory_planner \
								edge_impulse_firmware/firmware-sdk \
								edge_impulse_firmware/firmware-sdk/jpeg \
								edge_impulse_firmware/firmware-sdk/at-server \
								edge_impulse_firmware/firmware-sdk/sensor-aq \
								edge_impulse_firmware/firmware-sdk/QCBOR/inc \
								edge_impulse_firmware/firmware-sdk/QCBOR/src
################
## Config APP ##
################
include $(SCENARIO_APP_ROOT)/app_api/app_api.inc
APPL_APP_ROOT = $(SCENARIO_APP_ROOT)/$(APP_TYPE)

####################
## C source files ##
####################
# APPL_APP_CSRC_LIST += src/app_main.c

#####################
## CC source files ##
#####################
# APPL_APP_CCSRC_LIST += src/app_main.cpp

##################
## Header files ##
##################
APPL_APP_INCDIR_LIST = include

## append file path
APPL_APP_CSRCS = $(addprefix $(APPL_APP_ROOT)/, $(APPL_APP_CSRC_LIST))
APPL_APP_CCSRCS = $(addprefix $(APPL_APP_ROOT)/, $(APPL_APP_CCSRC_LIST))
APPL_APP_INCDIRS = $(addprefix $(APPL_APP_ROOT)/, $(APPL_APP_INCDIR_LIST))

##
# library support feature
# Add new library here
# The source code should be loacted in ~\library\{lib_name}\
##
#LIB_SEL = hxevent
LIB_SEL = pwrmgmt
LIB_SEL += sensordp
LIB_SEL += spi_ptl
LIB_SEL += spi_eeprom
#LIB_SEL += tflm
LIB_SEL += img_proc
LIB_SEL += i2c_comm
LIB_SEL += audio

##
# middleware support feature
# Add new middleware here
# The source code should be loacted in ~\middleware\{mid_name}\
##
MID_SEL =

override undefine OS_SEL
override TRUSTZONE := y
override TRUSTZONE_TYPE := security
override TRUSTZONE_FW_TYPE := 1

ifeq ($(CORE_SETTING), DUAL_CORE)
override EPII_USECASE_SEL := drv_dualcore_cm55m_s_only
APPL_DEFINES += -DWE2_DUAL_CORE
else ifeq ($(CORE_SETTING), SINGLE_CORE)
APPL_DEFINES += -DWE2_SINGLE_CORE
override EPII_USECASE_SEL := drv_singlecore_cm55m_s_only
endif

override CIS_SEL := HM_COMMON

ifeq ($(strip $(TOOLCHAIN)), arm)
override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/TrustZone_S_ONLY.sct
else#TOOLChain
override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/TrustZone_S_ONLY.ld
endif

##
# Add new external device here
# The source code should be located in ~\external\{device_name}\
##
#EXT_DEV_LIST +=

