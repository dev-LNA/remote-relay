

#ifndef USER_ETHERNET_H
#define USER_ETHERNET_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_err.h"

#include "esp_eth.h"
#include "esp_system.h"

#include "esp_eth_driver.h"
#include "esp_check.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "lwip/ip4_addr.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#define ETH_PHY_ADDR             1
#define ETH_PHY_RST_GPIO        -1       // Reset pin is not connected
#define ETH_OSC_ENAB        GPIO_NUM_16  // External oscillator pulled down at boot to allow IO0 strapping
#define ETH_MDC_GPIO        GPIO_NUM_23  // Management Data clock
#define ETH_MDIO_GPIO       GPIO_NUM_18  // Management Data Input/output
#define ETH_RMII_TX_EN      GPIO_NUM_21 
#define ETH_RMII_TX0        GPIO_NUM_19
#define ETH_RMII_TX1        GPIO_NUM_22
#define ETH_RMII_RX0        GPIO_NUM_25
#define ETH_RMII_RX1_EN     GPIO_NUM_26
#define ETH_RMII_CRS_DV     GPIO_NUM_27

#define STATIC_IP               0
#if STATIC_IP
    #define S_IP        "192.168.1.5"     
    #define GATEWAY     "192.168.1.1"    
    #define NETMASK     "255.255.255.0"
#endif /* STATIC_IP */


esp_err_t ethernet_setup(void);


#endif 
