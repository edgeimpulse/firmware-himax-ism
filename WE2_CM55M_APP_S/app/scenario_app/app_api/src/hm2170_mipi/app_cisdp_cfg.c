
#include "hx_drv_hxautoi2c_mst.h"

//#include "hx_drv_CIS_common.h"
//#include "sensor_dp_lib.h"

#include "app_datapath.h"
#include "app_cisdp_cfg.h"
#include "app_mipi.h"

#include "WE2_debug.h"

#include "hx_drv_scu_export.h"
static HX_CIS_SensorSetting_t HM2170_init_setting[] = {
#if (SENDPLIB_MIPIRX_LANE_NB == 0x02)
#include "HM2170_mipi_1928x1088_40FPS_2lane.i"
#else
#include "HM2170_mipi_1928x1088_48M_1lane.i"
#endif
};
//
static HX_CIS_SensorSetting_t HM2170_init_setting_aos[] = {
#include "HM2170_setA_Bin4_480x272_setB_240x136_FRC_On_Serial_Same_SDI_76Mhz_mirror_used_R1_Channel_20221202.i"
};

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

	scfg.cmdtype = HXAUTOI2CHC_CMD_TRIG_STOP;

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
	trig_cfg.delay01 = 20000; //866us
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

    trig_cmd1.byte1 = (HM2170_stream_on[0].RegAddree >> 8 & 0xFF);
    trig_cmd1.byte2 = (HM2170_stream_on[0].RegAddree & 0xFF);
    trig_cmd1.byte3 = HM2170_stream_on[0].Value;
    trig_cmd1.byte4 = 0x00;

	trig_cmd2.byte1 = trig_cmd2.byte2 = trig_cmd2.byte3 = trig_cmd2.byte4 = 0x11;
	trig_cmd3.byte1 = trig_cmd3.byte2 = trig_cmd3.byte3 = trig_cmd3.byte4 = 0x22;
	trig_cmd4.byte1 = trig_cmd4.byte2 = trig_cmd4.byte3 = trig_cmd4.byte4 = 0x33;

	sensordplib_autoi2c_trigcmd(trig_cmd1, trig_cmd2, trig_cmd3, trig_cmd4);

	HXAUTOI2CHC_CMD_T stop_cmd1, stop_cmd2, stop_cmd3, stop_cmd4;

    stop_cmd1.byte1 = (HM2170_stream_off[0].RegAddree >> 8 & 0xFF);
    stop_cmd1.byte2 = (HM2170_stream_off[0].RegAddree & 0xFF);
    stop_cmd1.byte3 = HM2170_stream_off[0].Value;
    stop_cmd1.byte4 = 0x00;

	stop_cmd2.byte1 = stop_cmd2.byte2 = stop_cmd2.byte3 = stop_cmd2.byte4 = 0x11;
	stop_cmd3.byte1 = stop_cmd3.byte2 = stop_cmd3.byte3 = stop_cmd3.byte4 = 0x22;
	stop_cmd4.byte1 = stop_cmd4.byte2 = stop_cmd4.byte3 = stop_cmd4.byte4 = 0x33;

	sensordplib_autoi2c_stopcmd(stop_cmd1, stop_cmd2, stop_cmd3, stop_cmd4);
}
#endif

int app_cisdp_datapath_init(const app_dp_cfg_t* dp_init_cfg)
{
    if(dp_init_cfg->init_type == APP_INIT_TYPE_VIDEO_STREAM)
    {
        //sensordplib_set_sensorctrl_inp_wi_crop_bin(dp_init_cfg->sensor_type, dp_init_cfg->stream_type,
        //	dp_init_cfg->sensor_width, dp_init_cfg->sensor_height, dp_init_cfg->inp_subsample_type, dp_init_cfg->crop, dp_init_cfg->inp_bin);
        sensordplib_set_sensorctrl_inp_wi_crop(dp_init_cfg->sensor_type, dp_init_cfg->stream_type,
            dp_init_cfg->sensor_width, dp_init_cfg->sensor_height, dp_init_cfg->inp_subsample_type, dp_init_cfg->crop);
    }
    else //AOS
    {
    	sensordplib_set_sensorctrl_inp_wi_crop(dp_init_cfg->sensor_type, dp_init_cfg->stream_type_aos,
    		dp_init_cfg->sensor_width, dp_init_cfg->sensor_height, dp_init_cfg->inp_subsample_type, dp_init_cfg->crop);
    }

    return 0;
}

static int priv_sensor_common_init(const app_dp_cfg_t* dp_init_cfg)
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
    else
    {
		if(hx_drv_cis_setRegTable(HM2170_init_setting, HX_CIS_SIZE_N(HM2170_init_setting, HX_CIS_SensorSetting_t))!= HX_CIS_NO_ERROR)
        {
            dbg_printf(DBG_LESS_INFO, "HM2170 Init AOS by app fail \r\n");
            return -1;
        }
        else
        {
            dbg_printf(DBG_LESS_INFO, "HM2170 Init AOS by app \n");
        }
    }

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

int app_cisdp_sensor_init(const app_dp_cfg_t* dp_init_cfg)
{
    app_mipi_rx_config_t cfg = {0};

    dbg_printf(DBG_LESS_INFO, "app_cisdp_sensor_init\n");

    /* MIPI RX Init */
	app_mipi_change_to_pll();
	/*mipi config for hm2170*/
	cfg.mipi_lnno = 2;
	cfg.pixel_dpp = 10;
	app_mipi_set_csirx_enable(&cfg);

    /* Sensor Common Init */
    priv_sensor_common_init(dp_init_cfg);
}

int app_cisdp_sensor_init_passthru(const app_dp_cfg_t* dp_init_cfg)
{
    app_mipi_passthrough_config_t cfg = {0};

    dbg_printf(DBG_LESS_INFO, "app_cisdp_sensor_init_passthru\n");

    /* MIPI RX Init */
	app_mipi_change_to_pll();
	/*mipi passthru config for hm2170*/
	cfg.bitrate_1lane=768;
	cfg.mipi_lnno=2;
	cfg.pixel_dpp=10;
	cfg.line_length=1928;
	cfg.frame_length=1088;
	cfg.continuousout=1;
	cfg.deskew_en=0;
	app_mipi_passthrough_init(&cfg);

    /* Sensor Common Init */
    priv_sensor_common_init(dp_init_cfg);
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

    app_mipi_set_csirx_disable();
}

