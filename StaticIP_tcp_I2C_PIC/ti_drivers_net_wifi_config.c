/*
 *  ======== ti_drivers_net_wifi_config.c ========
 */

/* TI-DRIVERS Header files */
#include <ti/drivers/net/wifi/simplelink.h>

/*
 *  =============================== SimpleLink Wifi ===============================
 */

/*
 *  ======== SimpleLinkWifi_config ========
 */
const SlWifiCC32XXConfig_t SimpleLinkWifiCC32XX_config = {

        .Mode = ROLE_STA,
		.Ipv4Mode = SL_NETCFG_IPV4_STA_ADDR_MODE,
		.ConnectionPolicy = SL_WLAN_CONNECTION_POLICY(1,0,0,0),
        .PMPolicy = SL_WLAN_NORMAL_POLICY,
		.MaxSleepTimeMS = 0,
		.ScanPolicy = SL_WLAN_SCAN_POLICY(0,0),
		.ScanIntervalInSeconds = 0,
		.Ipv4Config = SL_NETCFG_ADDR_STATIC,            /*Dinamic IP CFG Flag --> SL_NETCFG_ADDR_DHCP || Static IP FLAG --> SL_NETCFG_ADDR_STATIC*/
		.Ipv4 = SL_IPV4_VAL(192,168,1,80),
		.IpMask = SL_IPV4_VAL(255,255,255,0),
		.IpGateway = SL_IPV4_VAL(192,168,1,254),
		.IpDnsServer = SL_IPV4_VAL(192,168,1,254),
		.ProvisioningStop = 1,
		.DeleteAllProfile = 0
};
