/*
*  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved 
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*  http://www.apache.org/licenses/LICENSE-2.0
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <voice_control_setting.h>

#include "voice_control_panel_main.h"
#include "voice_control_panel_view.h"
#include "voice_control_panel_vc.h"

static void __vc_enabled_changed_cb(bool enabled, void* user_data)
{
	LOGD("Voice control enabled changed to (%d)", enabled);

	if (false == enabled) {
		ui_app_exit();
	}
}

static bool app_create(void *data)
{
	LOGD("");

	appdata *ad = (appdata *)data;

	/* Check voice control enabled */
	if (0 != vc_setting_initialize()) {
		LOGE("Fail to init");
	}

	bool enabled;
	if (0 != vc_setting_get_enabled(&enabled)) {
		LOGE("Fail to get enabled");
		vc_setting_deinitialize();
		return false;
	}
	if (false == enabled) {
		LOGE("Voice control is disabled");
		vc_setting_deinitialize();
		return false;
	}
	
	if (0 != vc_setting_set_enabled_changed_cb(__vc_enabled_changed_cb, NULL)) {
		LOGE("Fail to set enabled cb");
	}

	char* lang = NULL;
	if (0 != vc_setting_get_language(&lang)) {
		LOGE("Fail to get language");
		return false;
	}

	char loc[64] = {'\0',};
	snprintf(loc, 64, "%s.UTF-8", lang);
	
	setlocale(LC_ALL, loc);
	
	bindtextdomain("org.tizen.voice-control-panel", "/usr/apps/org.tizen.voice-control-panel/res/locale");
	textdomain("org.tizen.voice-control-panel");

	if (NULL != lang) {
	    free(lang);
	    lang = NULL;
	}

	/* Create View */
	if (0 != vc_panel_view_create(ad)) {
		LOGE( "[ERROR] Fail to create view");
		return -1;
	}

	/* Initialize Voice Control */
	if (0 != vc_panel_vc_init(ad)) {
		LOGE( "[ERROR] Fail to vc init");
		return -1;
	}

	return true;
}

static void app_control(app_control_h app_control, void *data)
{
	LOGD("");

	appdata *ad = (appdata *)data;
	LOGD("state - %d", ad->app_state);

	if (0 != ad->app_state) {
		ui_app_exit();
		return;
	}

	if (ad->win) {
		elm_win_activate(ad->win);
	}
	ad->app_state = APP_STATE_INIT;
}

static void app_pause(void *data)
{
	LOGD("");

	appdata *ad = (appdata *)data;
	ad->app_state = APP_STATE_PAUSE;
	ui_app_exit();
}

static void app_resume(void *data)
{
	LOGD("");

	appdata *ad = (appdata *)data;
	if(ad->app_state == APP_STATE_PAUSE) {
		if (ad->win) {
			elm_win_activate(ad->win);
		}
	}
	ad->app_state = APP_STATE_SERVICE;

	vc_panel_vc_start(ad);
}

static void app_terminate(void *data)
{
	LOGD("");

	appdata *ad = (appdata *)data;
	ad->app_state = APP_STATE_TERMINATE;
	
	vc_panel_vc_cancel(ad);

	vc_panel_view_destroy(ad);

	vc_setting_deinitialize();

	if (0 != vc_panel_vc_deinit(data)) {
		LOGE( "[ERROR] Fail to vc deinit");
	}

}

static void ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	LOGD("");
}

static void ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	LOGD("");
}

static void ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	LOGD("");
}

static void ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	LOGD("");
}

static void ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	LOGD("");
}

int main (int argc, char *argv[])
{
	appdata ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		LOGW("ui_app_main failed, Err=%d\n", ret);
	}

	return ret;
}

/*
vi:ts=4:ai:nowrap:expandtab
*/
