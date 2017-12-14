#include "minui/minui.h"
#include <cutils/klog.h>
#include <fcntl.h>
#include "healthd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "property_service.h"
#include "util.h"
#include "vendor_init.h"


#ifndef BACKLIGHT_PATH
#define BACKLIGHT_PATH          "/sys/class/leds/lcd-backlight/brightness"
#endif
#define BACKLIGHT_ON_LEVEL      100

#define LOGE(x...) do { KLOG_ERROR("charger", x); } while (0)
#define LOGW(x...) do { KLOG_WARNING("charger", x); } while (0)
#define LOGV(x...) do { KLOG_DEBUG("charger", x); } while (0)

std::string bootloader;
std::string device;
char* devicename;



void vendor_load_properties()
{
 	bootloader = property_get("ro.bootloader");
            property_set("ro.build.fingerprint", "samsung/millet3gxx/millet3g:5.1.1/LMY47X/T331XXS1BQD1:user/release-keys");
            property_set("ro.build.description", "millet3gxx-user 5.0.2 LRX22G T531XXU1BOE6 release-keys");
            property_set("ro.product.model", "SM-T331");
            property_set("ro.product.device", "millet3g");
     


    device = property_get("ro.product.device");
    INFO("Found bootloader id %s setting build properties for %s device\n", bootloader.c_str(), device.c_str());
}

void healthd_board_init(struct healthd_config *config)
{
    config->batteryCapacityPath    = "/sys/class/power_supply/battery/capacity";
    config->batteryStatusPath      = "/sys/class/power_supply/battery/status";
    config->batteryVoltagePath     = "/sys/class/power_supply/battery/voltage_now";
    config->batteryPresentPath     = "/sys/class/power_supply/battery/present";
    config->batteryHealthPath      = "/sys/class/power_supply/battery/health";
    config->batteryTemperaturePath = "/sys/class/power_supply/battery/temp";
    config->batteryTechnologyPath  = "/sys/class/power_supply/battery/technology";
}

#define STR_LEN 8
void healthd_board_mode_charger_draw_battery(
                struct android::BatteryProperties *batt_prop)
{
    char cap_str[STR_LEN];
    int x, y;
    int str_len_px;
    GRFont* gr_font= NULL;
    static int char_height = -1, char_width = -1;

    if (char_height == -1 && char_width == -1)
        gr_font_size(gr_font, &char_width, &char_height);
    snprintf(cap_str, (STR_LEN - 1), "%d%%", batt_prop->batteryLevel);
    str_len_px = gr_measure(gr_font, cap_str);
    x = (gr_fb_width() - str_len_px) / 2;
    y = (gr_fb_height() + char_height) / 2;
    gr_color(0xa4, 0xc6, 0x39, 255);
    gr_text(gr_font, x, y, cap_str, 0);
}

int healthd_board_battery_update(__attribute__((unused)) struct android::BatteryProperties *props)
{
    // return 0 to log periodic polled battery status to kernel log
    return 0;
}

void healthd_board_mode_charger_battery_update(struct android::BatteryProperties*)
{

}

void healthd_board_mode_charger_set_backlight(bool on)
{
    int fd;
    char buffer[10];

    if (access(BACKLIGHT_PATH, R_OK | W_OK) != 0)
    {
        LOGW("Backlight control not support\n");
        return;
    }

    memset(buffer, '\0', sizeof(buffer));
    fd = open(BACKLIGHT_PATH, O_RDWR);
    if (fd < 0) {
        LOGE("Could not open backlight node : %s\n", strerror(errno));
        return;
    }
    LOGV("Enabling backlight\n");
    snprintf(buffer, sizeof(buffer), "%d\n", on ? BACKLIGHT_ON_LEVEL : 0);
    if (write(fd, buffer,strlen(buffer)) < 0) {
        LOGE("Could not write to backlight node : %s\n", strerror(errno));
    }
    close(fd);
}

void healthd_board_mode_charger_init()
{

}
