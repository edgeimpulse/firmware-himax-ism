#ifdef HM2170_BAYER

#include "hx_drv_hxautoi2c_mst.h"

//#include "hx_drv_CIS_common.h"
//#include "sensor_dp_lib.h"

#include "app_datapath.h"
#include "app_cisdp_cfg.h"

#include "WE2_debug.h"

#include "hx_drv_scu_export.h"

#ifdef CIS_AOS_MODE
static HX_CIS_SensorSetting_t HM2170_init_setting_aos[] = {
#include "HM2170_setA_Bin4_480x272_setB_240x136_FRC_On_Serial_Same_SDI_76Mhz_mirror_used_R1_Channel_20221202.i"
};
#else
static HX_CIS_SensorSetting_t HM2170_init_setting[] = {
/*
* MIPI/Serial 60HZ Concurrent: Enable AE
*/
#include "HM2170_setA_1928x1088-480x272@60_setB_964x544-240x136@60_Continue_2Lane_MIPI384Mhz_FRC_R1.i"
};
#endif

static HX_CIS_SensorSetting_t HM2170_stream_on[] = {
        {HX_CIS_I2C_Action_W, 0x0100, 0x01},
};

static HX_CIS_SensorSetting_t HM2170_stream_off[] = {
        {HX_CIS_I2C_Action_W, 0x0100, 0x00},
};

static HX_CIS_SensorSetting_t HM2170_stream_standby[] = {
        {HX_CIS_I2C_Action_W, 0x0100, 0x02},
};

static HX_CIS_SensorSetting_t HM2170_stream_1frm[] = {
        {HX_CIS_I2C_Action_W, 0x0100, 0x03},
};

#if (CIS_ENABLE_HX_AUTOI2C != 0x00)
void app_cisdp_set_hxautoi2c(APP_INIT_TYPE_E type)
{
	//set to synopsys IP, change to Himax IP
	dbg_printf(DBG_LESS_INFO, "INIT AUTOI2C\n");
	HXAUTOI2CHC_STATIC_CFG_T scfg;
	HXAUTOI2CHC_INT_CFG_T icfg;
	HXAUTOI2CHC_CMD_CFG_T trig_cfg;
	HXAUTOI2CHC_CMD_CFG_T stop_cfg;

	scfg.slaveid = CIS_I2C_ID;

	scfg.clkdiv = HXAUTOI2C_SCL_CLK_DIV_60;
	scfg.cmdtype = HXAUTOI2CHC_CMD_TRIG;

	scfg.trig_ctrl_sw = HXAUTOI2CHC_CMD_CTRL_BY_HW;
	scfg.stop_ctrl_sw = HXAUTOI2CHC_CMD_CTRL_BY_HW;

	icfg.trig_int_loc = HXAUTOI2CHC_INT_LOC_AFT_CMD1;
	icfg.stop_int_loc = HXAUTOI2CHC_INT_LOC_AFT_CMD1;
	icfg.trig_delay = 0;
	icfg.stop_delay = 1680;// delay for MCLK extends, 24 = 1us
	dbg_printf(DBG_LESS_INFO, "icfg.stop_delay =%d\r\n", icfg.stop_delay );

	trig_cfg.cmd_num = HXAUTOI2CHC_CMD_NUM_1;
	trig_cfg.cmd1_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	trig_cfg.cmd2_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	trig_cfg.cmd3_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	trig_cfg.cmd4_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	trig_cfg.delay01 = 0;
	trig_cfg.delay12 = 0x100;
	trig_cfg.delay23 = 0x100;
	trig_cfg.delay34 = 0x100;

	dbg_printf(DBG_LESS_INFO, "trig_cfg.delay01=%d\r\n", trig_cfg.delay01);

	stop_cfg.cmd_num = HXAUTOI2CHC_CMD_NUM_1;
	stop_cfg.cmd1_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	stop_cfg.cmd2_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	stop_cfg.cmd3_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	stop_cfg.cmd4_byte_num = HXAUTOI2CHC_BYTE_NUM_3;
	stop_cfg.delay01 = 0;
	stop_cfg.delay12 = 0x100;
	stop_cfg.delay23 = 0x100;
	stop_cfg.delay34 = 0x100;

	sensordplib_autoi2c_cfg(scfg, icfg, trig_cfg, stop_cfg);


	HXAUTOI2CHC_CMD_T trig_cmd1, trig_cmd2, trig_cmd3, trig_cmd4;

	trig_cmd1.byte1 = (HM2170_stream_1frm[0].RegAddree >> 8 & 0xFF);
	trig_cmd1.byte2 = (HM2170_stream_1frm[0].RegAddree & 0xFF);
	trig_cmd1.byte3 = HM2170_stream_1frm[0].Value;
	trig_cmd1.byte4 = 0x00;

	}

	trig_cmd2.byte1 = trig_cmd2.byte2 = trig_cmd2.byte3 = trig_cmd2.byte4 = 0x11;
	trig_cmd3.byte1 = trig_cmd3.byte2 = trig_cmd3.byte3 = trig_cmd3.byte4 = 0x22;
	trig_cmd4.byte1 = trig_cmd4.byte2 = trig_cmd4.byte3 = trig_cmd4.byte4 = 0x33;

	sensordplib_autoi2c_trigcmd(trig_cmd1, trig_cmd2, trig_cmd3, trig_cmd4);

	HXAUTOI2CHC_CMD_T stop_cmd1, stop_cmd2, stop_cmd3, stop_cmd4;

	stop_cmd1.byte1 = 0x00;
	stop_cmd1.byte2 = 0x00;
	stop_cmd1.byte3 = 0x00;
	stop_cmd1.byte4 = 0x00;

	stop_cmd2.byte1 = stop_cmd2.byte2 = stop_cmd2.byte3 = stop_cmd2.byte4 = 0x11;
	stop_cmd3.byte1 = stop_cmd3.byte2 = stop_cmd3.byte3 = stop_cmd3.byte4 = 0x22;
	stop_cmd4.byte1 = stop_cmd4.byte2 = stop_cmd4.byte3 = stop_cmd4.byte4 = 0x33;

	sensordplib_autoi2c_stopcmd(stop_cmd1, stop_cmd2, stop_cmd3, stop_cmd4);
}
#endif

int app_cisdp_datapath_init(const app_dp_cfg_t* dp_init_cfg)
{
    #ifdef CIS_AOS_MODE
	if(dp_init_cfg->init_type == APP_INIT_TYPE_AOS)
    {
    	sensordplib_set_sensorctrl_inp(dp_init_cfg->sensor_type, dp_init_cfg->stream_type_aos, dp_init_cfg->sensor_width,
            dp_init_cfg->sensor_height, dp_init_cfg->inp_subsample_type);
    	hx_drv_inp1bitparser_set_tg2utg(dp_init_cfg->sclk_utg);
    }
	#else
	if(dp_init_cfg->init_type == APP_INIT_TYPE_VIDEO_STREAM)
    {
    	sensordplib_set_sensorctrl_inp_serial_pack2l(dp_init_cfg->sensor_type, dp_init_cfg->stream_type_nonaos, 
            dp_init_cfg->sensor_width, dp_init_cfg->sensor_height, dp_init_cfg->inp_subsample_type, dp_init_cfg->h_fporch);
    	hx_drv_inp1bitparser_set_tg2utg(dp_init_cfg->sclk_utg);
    }
	#endif

    return 0;
}

int app_cisdp_sensor_init(const app_dp_cfg_t* dp_init_cfg)
{
    dbg_printf(DBG_LESS_INFO, "cis_hm2170_init \r\n");

    /*
     * common CIS init
     */
    hx_drv_cis_init((CIS_XHSHUTDOWN_INDEX_E)dp_init_cfg->xshutdown_pin, dp_init_cfg->mclk_div);
    dbg_printf(DBG_LESS_INFO, "xshutdown_pin=%d, mclk_div=%d\n",dp_init_cfg->xshutdown_pin, dp_init_cfg->mclk_div);
    //hx_drv_sensorctrl_set_MCLKCtrl(SENSORCTRL_MCLKCTRL_NONAOS);
    hx_drv_sensorctrl_set_xSleepCtrl(SENSORCTRL_XSLEEP_BY_CPU);
    hx_drv_sensorctrl_set_xSleep(1);
    dbg_printf(DBG_LESS_INFO, "hx_drv_sensorctrl_set_xSleep(1)\n");

    hx_drv_cis_set_slaveID(CIS_I2C_ID);
    dbg_printf(DBG_LESS_INFO, "hx_drv_cis_set_slaveID(0x%02X)\n", CIS_I2C_ID);

    /*
     * off stream before init sensor
     */
    if(hx_drv_cis_setRegTable(HM2170_stream_off, HX_CIS_SIZE_N(HM2170_stream_off, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
    {
        dbg_printf(DBG_LESS_INFO, "HM2170 off by app fail \r\n");
        return -1;
    }
    else
    {
        dbg_printf(DBG_LESS_INFO, "HM2170 off by app \n");
    }

	#ifdef CIS_AOS_MODE
    if(dp_init_cfg->init_type == APP_INIT_TYPE_AOS)
    {
		if(hx_drv_cis_setRegTable(HM2170_init_setting_aos, HX_CIS_SIZE_N(HM2170_init_setting_aos, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
        {
            dbg_printf(DBG_LESS_INFO, "HM2170 Init AOS by app fail \r\n");
            return -1;
        }
        else
        {
            dbg_printf(DBG_LESS_INFO, "HM2170 Init AOS by app \n");
        }
    }
	#else
	if(dp_init_cfg->init_type == APP_INIT_TYPE_VIDEO_STREAM)
    {
        if(hx_drv_cis_setRegTable(HM2170_init_setting, HX_CIS_SIZE_N(HM2170_init_setting, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
        {
            dbg_printf(DBG_LESS_INFO, "HM2170 Init Stream by app fail \r\n");
            return -1;
        } 
        else
        {
            dbg_printf(DBG_LESS_INFO, "HM2170 Init Stream by app \n");
        }
    }
	#endif

    HX_CIS_SensorSetting_t HM2170_mirror_setting[] = {
            {HX_CIS_I2C_Action_W, 0x0101, CIS_MIRROR_SETTING},
    };

    if(hx_drv_cis_setRegTable(HM2170_mirror_setting, HX_CIS_SIZE_N(HM2170_mirror_setting, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
    {
        dbg_printf(DBG_LESS_INFO, "HM2170 Init Mirror 0x%02X by app fail \r\n", HM2170_mirror_setting[0].Value);
        return -1;
    }
    else
    {
        #if (CIS_MIRROR_SETTING == 0x01)
        dbg_printf(DBG_LESS_INFO, "HM2170 Init Horizontal Mirror by app \n");
        #elif (CIS_MIRROR_SETTING == 0x02)
        dbg_printf(DBG_LESS_INFO, "HM2170 Init Vertical Mirror by app \n");
        #elif (CIS_MIRROR_SETTING == 0x03)
        dbg_printf(DBG_LESS_INFO, "HM2170 Init Horizontal & Vertical Mirror by app \n");
        #else
        dbg_printf(DBG_LESS_INFO, "HM2170 Init Mirror Off by app \n");
        #endif
    }

    return 0;
}

void app_cisdp_sensor_start(void)
{
    /*
     * Default Stream On
     */
    if(hx_drv_cis_setRegTable(HM2170_stream_on, HX_CIS_SIZE_N(HM2170_stream_on, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
    {
    	dbg_printf(DBG_LESS_INFO, "HM2170 on by app fail \r\n");
    }
    else
    {
    	dbg_printf(DBG_LESS_INFO, "HM2170 on by app \n");
        return;
    }
}

void app_cisdp_sensor_start_AOS(void)
{
	#if (CIS_ENABLE_HX_AUTOI2C != 0x00)
    sensordplib_autoi2c_enable();
    dbg_printf(DBG_LESS_INFO, "hxauto i2c enable \n");
	#endif
}

void app_cisdp_sensor_stop()
{
    sensordplib_stop_capture();
    sensordplib_start_swreset();
    sensordplib_stop_swreset_WoSensorCtrl();

    /*
     * Default Stream On
     */
    if(hx_drv_cis_setRegTable(HM2170_stream_off, HX_CIS_SIZE_N(HM2170_stream_off, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
    {
    	dbg_printf(DBG_LESS_INFO, "HM2170 off by app fail \r\n");
    }
    else
    {
    	dbg_printf(DBG_LESS_INFO, "HM2170 off by app \n");
    }
	
	#if (CIS_ENABLE_HX_AUTOI2C != 0x00)
    //disable hxauotoi2c
    sensordplib_autoi2c_disable();
    dbg_printf(DBG_LESS_INFO, "hxauto i2c disable \n");
	#endif
}

#endif //HM2170_BAYER

