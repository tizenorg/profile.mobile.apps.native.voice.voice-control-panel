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

#include <dlog.h>
#include <Elementary.h>

#include <voice_control_command.h>
#include <voice_control_common.h>
#include <voice_control_manager.h>

#include "voice_control_panel_action.h"
#include "voice_control_panel_command.h"
#include "voice_control_panel_main.h"
#include "voice_control_panel_vc.h"
#include "voice_control_panel_view.h"

static vc_cmd_list_h g_cmd_list_1st;
static vc_cmd_list_h g_cmd_list_2nd[NUM_COMMAND_1ST];

#define TIMEOUT	5
static Ecore_Timer *g_timeout_timer = NULL;
static Ecore_Timer *g_restart_timer = NULL;

static bool __current_command_cb(vc_cmd_h vc_command, void* user_data)
{
	int type;
	if (0 != vc_cmd_get_type(vc_command, &type)) {
		LOGE("[ERROR] Fail to get type");
		return false;
	}

	return true;
}

static int __vc_panel_vc_get_current_commands(void *data)
{
	appdata *ad = (appdata *)data;

	LOGD("==== Get current commands ====");
	if (0 < g_list_length(ad->cmd_list)) {
		GList *iter = NULL;
		iter = g_list_first(ad->cmd_list);

		while (NULL != iter) {
			char *cmd = iter->data;
			if (NULL != cmd) {
				free(cmd);
				cmd = NULL;
			}

			ad->cmd_list = g_list_remove_link(ad->cmd_list, iter);
			iter = g_list_first(ad->cmd_list);
		}
	}
	
	vc_cmd_list_h vc_cmd_list;
	if (0 != vc_mgr_get_current_commands(&vc_cmd_list)) {
		LOGE("[ERROR] Fail to get current commands");
		if (0 != vc_cmd_list_destroy(vc_cmd_list, true)) {
			LOGE("[WARNING] Fail to cmd list destroy");
		}
		return -1;
	}

	if (0 != vc_cmd_list_foreach_commands(vc_cmd_list, __current_command_cb, ad)) {
		LOGE("[ERROR] Fail to get current commands");
		if (0 != vc_cmd_list_destroy(vc_cmd_list, true)) {
			LOGE("[WARNING] Fail to cmd list destroy");
		}
		return -1;
	}

	if (0 != vc_cmd_list_destroy(vc_cmd_list, true)) {
		LOGE("[WARNING] Fail to cmd list destroy");
	}

	return 0;
}

static void __vc_panel_vc_destroy_command_list()
{
	LOGD( "==== Destroy Command List ====");

	if (0 != vc_cmd_list_destroy(g_cmd_list_1st, true)) {
		LOGE("[WARNING] Fail to destroy list");
	}

	int i;
	for (i = 0; i < NUM_COMMAND_1ST; i++) {
		if (0 != vc_cmd_list_destroy(g_cmd_list_2nd[i], true)) {
			LOGE("[WARNING] Fail to destroy list");
		}
	}
	
	LOGD( "====");
	LOGD( " ");
}

static int __vc_panel_vc_create_command_list()
{
	LOGD( "==== Create Command List ====");

	/* 1st depth */
	if (0 != vc_cmd_list_create(&g_cmd_list_1st)) {
		LOGE( "[ERROR] Fail to cmd list create");
		return -1;
	}

	int i;
	for (i = 0; i < NUM_COMMAND_1ST; i++) {
		vc_cmd_h cmd;
		if (0 != vc_cmd_create(&cmd)) {
			LOGE( "[ERROR] Fail to cmd create");
			return -1;
		}
		if (0 != vc_cmd_set_command(cmd, _(g_command_1st[i]))) {
			LOGE( "[ERROR] Fail to set command");
			vc_cmd_destroy(cmd);
			return -1;
		}
		if (0 != vc_cmd_set_type(cmd, VC_COMMAND_TYPE_SYSTEM)) {
			LOGE( "[ERROR] Fail to set type");
			vc_cmd_destroy(cmd);
			return -1;
		}
		if (0 != vc_cmd_set_format(cmd, VC_CMD_FORMAT_FIXED)) {
			LOGE( "[ERROR] Fail to set format");
			vc_cmd_destroy(cmd);
			return -1;
		}
		if (0 != vc_cmd_list_add(g_cmd_list_1st, cmd)) {
			LOGE( "[ERROR] Fail to list add");
			vc_cmd_destroy(cmd);
			return -1;
		}
	}

	/* 2nd depth */
	for (i = 0; i < NUM_COMMAND_1ST; i++) {
		if (0 != vc_cmd_list_create(&g_cmd_list_2nd[i])) {
			LOGE( "[ERROR] Fail to list create");
			return -1;
		}

		int j;
		for (j = 0; j < NUM_COMMAND_2ND; j++) {
			if (NULL != g_command_2nd[i][j]) {
				vc_cmd_h cmd;
				if (0 != vc_cmd_create(&cmd)) {
					LOGE( "[ERROR] Fail to cmd create");
					return -1;
				}
				if (0 != vc_cmd_set_command(cmd, _(g_command_2nd[i][j]))) {
					LOGE( "[ERROR] Fail to set command");
					vc_cmd_destroy(cmd);
					return -1;
				}
				if (0 != vc_cmd_set_type(cmd, VC_COMMAND_TYPE_SYSTEM)) {
					LOGE( "[ERROR] Fail to set type");
					vc_cmd_destroy(cmd);
					return -1;
				}
				if (0 != vc_cmd_set_format(cmd, VC_CMD_FORMAT_FIXED)) {
					LOGE( "[ERROR] Fail to set format");
					vc_cmd_destroy(cmd);
					return -1;
				}
				if (0 != vc_cmd_list_add(g_cmd_list_2nd[i], cmd)) {
					LOGE( "[ERROR] Fail to list add");
					vc_cmd_destroy(cmd);
					return -1;
				}
			}
		}
	}

	return 0;
}

static Eina_Bool __start_cb(void *data)
{
	appdata *ad = (appdata *)data;

	vc_service_state_e state;
	if (0 != vc_mgr_get_service_state(&state)) {
	    LOGE("[ERROR] Fail to get service state");
	    return EINA_FALSE;
	}

	if (VC_SERVICE_STATE_READY != state) {
	    LOGD("[WARNING] Wait for service state ready");
	    return EINA_TRUE;
	}

	if (0 != vc_mgr_set_recognition_mode(VC_RECOGNITION_MODE_RESTART_AFTER_REJECT)) {
		LOGE( "[ERROR] Fail to set recognition mode");
	}
	if (0 != vc_mgr_set_command_list(g_cmd_list_1st)) {
		LOGE( "[ERROR] Fail to set command list");
	}

	if (0 != __vc_panel_vc_get_current_commands(ad)) {
		LOGE("[ERROR] Fail to get current commands");
	}

	if (0 != vc_mgr_start(false)) {
		LOGE( "[ERROR] Fail to start");
	}

	return EINA_FALSE;
}

static void __vc_panel_vc_state_changed_cb(vc_state_e previous, vc_state_e current, void *user_data)
{
	LOGD( "==== State is changed ====");
	LOGD( "Previous(%d) -> Current(%d)", previous, current);
}

static void __vc_panel_vc_reset(void *data)
{
	appdata *ad = (appdata *)data;

	ad->current_depth = 1;
	ad->current_path[0] = -1;
	ad->current_path[1] = -1;

	if (0 != vc_mgr_set_command_list(g_cmd_list_1st)) {
		LOGE( "[ERROR] Fail to set command list");
	}
}

static Eina_Bool __vc_panel_finalize(void *data)
{
	LOGD("=== Finalize ===");
	if (g_timeout_timer != NULL) {
		ecore_timer_del(g_timeout_timer);
		g_timeout_timer = NULL;
	}
	ui_app_exit();
	return EINA_FALSE;
}

static Eina_Bool __vc_panel_vc_restart(void *data)
{
	LOGD( "==== Restart ====");

	appdata *ad = (appdata *)data;

	vc_service_state_e state;
	if (0 != vc_mgr_get_service_state(&state)) {
	    LOGE("[ERROR] Fail to get service state");
	    return EINA_FALSE;
	}

	if (VC_SERVICE_STATE_READY != state) {
	    LOGD("[WARNING] Wait for service state ready");
	    return EINA_TRUE;
	}

	/* set current command list */
	if (1 == ad->current_depth) {
		if (0 != vc_mgr_set_command_list(g_cmd_list_1st)) {
			LOGE( "[ERROR] Fail to set command list");
		}
	} else if (2 == ad->current_depth) {
		if (0 != vc_mgr_set_command_list(g_cmd_list_2nd[ad->current_path[0]])) {
			LOGE( "[ERROR] Fail to set command list");
		}
	}

	vc_panel_view_show(ad);

	if (0 != __vc_panel_vc_get_current_commands(ad)) {
		LOGE("[ERROR] Fail to get current commands");
	}

	if (0 != vc_mgr_start(false)) {
		LOGE( "[ERROR] Fail to start");
	}

	LOGD( "====");
	LOGD( " ");

	return EINA_FALSE;
}

static void __vc_panel_vc_service_state_changed_cb(vc_service_state_e previous, vc_service_state_e current, void *user_data)
{
	LOGD( "==== Service state is changed ====");
	LOGD( "Previous(%d) -> Current(%d)", previous, current);
	
	appdata *ad = (appdata *)user_data;

	if (VC_SERVICE_STATE_READY == previous && VC_SERVICE_STATE_RECORDING == current) {
		LOGD( "==== Show by recording ====");
		vc_panel_view_show(ad);
		if (NULL != g_timeout_timer) {
			ecore_timer_reset(g_timeout_timer);
		}
	} else if ((VC_SERVICE_STATE_RECORDING == previous || VC_SERVICE_STATE_PROCESSING == previous) && VC_SERVICE_STATE_READY == current) {
		if (APP_STATE_SERVICE == ad->app_state) {
			LOGD("==== Restart ====");
			if (NULL != g_restart_timer) {
				ecore_timer_del(g_restart_timer);
			}
			g_restart_timer = ecore_timer_add(0.5, __vc_panel_vc_restart, user_data);
		} else {
			LOGD("==== Hide ====");
			__vc_panel_vc_reset(ad);
		}
	}

	if (VC_SERVICE_STATE_RECORDING == current) {
		if (NULL != g_timeout_timer) {
			ecore_timer_reset(g_timeout_timer);
		}
	}
}

static Eina_Bool __hide_result(void *data)
{
	LOGD("");
	vc_panel_view_show(data);

	return EINA_FALSE;
}

static bool __vc_panel_vc_all_result_cb(vc_result_event_e event, vc_cmd_list_h vc_cmd_list, const char* result, const char* msg, void *user_data)
{
	LOGD( "==== All result cb ====");

	if (VC_RESULT_EVENT_REJECTED == event) {
		LOGD("Rejected event");
		vc_panel_view_show_result(user_data, _("IDS_SORRY"));
		ecore_timer_add(0.5, __hide_result, user_data);
	} else {
		if (NULL != result) {
			LOGD( "Result Text - %s", result);
			vc_panel_view_show_result(user_data, result);
		
    		if (strcmp(result, _("IDS_HELP"))) {
    			vc_panel_view_hide_help(user_data);
    		}
        }
	}

	return true;
}

static void __vc_panel_vc_result_cb(vc_result_event_e event, vc_cmd_list_h vc_cmd_list, const char* result, void *user_data)
{
	LOGD( "==== Result cb ====");

	if (NULL != result) {
		LOGD( "Result Text - %s", result);

		bool ret;
		ret = vc_panel_action(result, user_data);
		if (true != ret) {
			ecore_idler_add(__vc_panel_finalize, user_data);
		}
	}
}

static void __vc_panel_vc_speech_detected_cb(void *user_data)
{
	LOGD("==== Speech detected ====");
	if (NULL != g_timeout_timer)
		ecore_timer_reset(g_timeout_timer);
}

static void __vc_panel_vc_language_changed_cb(const char* previous, const char* current, void* user_data)
{
	LOGD("Language is changed (%s) to (%s)", previous, current);

	char loc[64] = {'\0', };
	snprintf(loc, 64, "%s.UTF-8", current);

	setlocale(LC_ALL, loc);

	__vc_panel_vc_destroy_command_list();
	if (0 != __vc_panel_vc_create_command_list()) {
		LOGE("[ERROR] Fail to create command list");
	}
}

int vc_panel_vc_start(void *data)
{
	if (NULL != g_timeout_timer)
		ecore_timer_reset(g_timeout_timer);

	ecore_idler_add(__start_cb, data);
	return 0;
}

int vc_panel_vc_cancel(void *data)
{
	vc_service_state_e service_state;

	if (0 != vc_mgr_get_service_state(&service_state)) {
		LOGE( "[ERROR] Fail to get service state");
		return -1;
	}

	if ((VC_SERVICE_STATE_RECORDING == service_state) || (VC_SERVICE_STATE_PROCESSING == service_state)) {
		LOGD( "==== service state (%d)", service_state);

		if (0 != vc_mgr_cancel()) {
			LOGE( "[ERROR] Fail to cancel");
			return -1;
		}
	}

	return 0;
}

int vc_panel_vc_init(void *data)
{
	LOGD( "==== Initialze Voice control ====");

	if (0 != vc_mgr_initialize()) {
		LOGE( "[ERROR] Fail to initialize");
		return -1;
	}

	if (0 != vc_mgr_set_state_changed_cb(__vc_panel_vc_state_changed_cb, data)) {
		LOGE( "[ERROR] Fail to set state changed cb");
		return -1;
	}

	if (0 != vc_mgr_set_service_state_changed_cb(__vc_panel_vc_service_state_changed_cb, data)) {
		LOGE( "[ERROR] Fail to set service state changed cb");
		return -1;
	}

	if (0 != vc_mgr_set_all_result_cb(__vc_panel_vc_all_result_cb, data)) {
		LOGE( "[ERROR] Fail to set all result cb");
		return -1;
	}

	if (0 != vc_mgr_set_result_cb(__vc_panel_vc_result_cb, data)) {
		LOGE( "[ERROR] Fail to set result cb");
		return -1;
	}

	if (0 != vc_mgr_set_speech_detected_cb(__vc_panel_vc_speech_detected_cb, data)) {
		LOGE("[ERROR] Fail to set speech detected cb");
		return -1;
	}

	if (0 != vc_mgr_set_current_language_changed_cb(__vc_panel_vc_language_changed_cb, data)) {
		LOGE("[ERROR] Fail to set language changed cb");
		return -1;
	}

	if (0 != vc_mgr_prepare()) {
		LOGE( "[ERROR] Fail to prepare");
		return -1;
	}

	if (0 != __vc_panel_vc_create_command_list()) {
		LOGE( "[ERROR] Fail to create command list");
		return -1;
	}

	if (NULL == g_timeout_timer) {
		g_timeout_timer = ecore_timer_add(TIMEOUT, __vc_panel_finalize, NULL);
	}

	LOGD( "====");
	LOGD( " ");

	return 0;
}

int vc_panel_vc_deinit(void *data)
{
	LOGD( "==== De-initialize Voice control ====");

	appdata *ad = (appdata *)data;

	if (g_restart_timer != NULL) {
		ecore_timer_del(g_restart_timer);
		g_restart_timer = NULL;
	}

	if (g_timeout_timer != NULL) {
		ecore_timer_del(g_timeout_timer);
		g_timeout_timer = NULL;
	}

	if (0 < g_list_length(ad->cmd_list)) {
		GList *iter = NULL;
		iter = g_list_first(ad->cmd_list);

		while (NULL != iter) {
			char *cmd = iter->data;
			if (NULL != cmd) {
				free(cmd);
				cmd = NULL;
			}

			ad->cmd_list = g_list_remove_link(ad->cmd_list, iter);
			iter = g_list_first(ad->cmd_list);
		}
	}

	__vc_panel_vc_destroy_command_list();

	if (0 != vc_mgr_deinitialize()) {
		LOGE( "[ERROR] Fail to deinitialize");
		return -1;
	}

	LOGD( "====");
	LOGD( " ");

	return 0;
}

/*
vi:ts=4:ai:nowrap:expandtab
*/
