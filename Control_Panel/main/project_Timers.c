#include "headers/project_Timers.h"

TimerHandle_t xTimer_rfs_top_menu;

static void refresh_top_menu(void)
{
    ESP_LOGI(TIMER_TAG, "Refreshing top menu....");
    updateTopMenu();
}

void initTimer_rfs_top_menu(void)
{
    xTimer_rfs_top_menu = xTimerCreate("rfs top menu", pdMS_TO_TICKS(TIMER_RFS_TOP_MENU_inMS), true, NULL, refresh_top_menu);
    xTimerStart(xTimer_rfs_top_menu, 0);
}