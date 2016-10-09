static u32 plugin_active = 0;

static int AutoPowerOffGame = -1;
static int AutoPowerOffVideo = -1;

static void setAutoPowerOff(bool disable)
{
	if(c_firmware < 4.46f) return;

	if(AutoPowerOffGame < 0)
	{
		xsetting_D0261D72()->loadRegistryIntValue(0x33, &AutoPowerOffGame);
		xsetting_D0261D72()->loadRegistryIntValue(0x32, &AutoPowerOffVideo);
	}

	xsetting_D0261D72()->saveRegistryIntValue(0x33, disable ? 0 : AutoPowerOffGame);
	xsetting_D0261D72()->saveRegistryIntValue(0x32, disable ? 0 : AutoPowerOffVideo);
}

static void setPluginActive(void)
{
	if(plugin_active == 0) setAutoPowerOff(true);
	plugin_active++;
}

static void setPluginInactive(void)
{
	if(plugin_active > 0)
	{
		plugin_active--;
		if(plugin_active == 0) setAutoPowerOff(false);
	}
}

static void setPluginExit(void)
{
	working = plugin_active = 0;
	setAutoPowerOff(false);
	{ DELETE_TURNOFF }
}
