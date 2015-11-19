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

#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>

#include <app.h>
#include <aul.h>
#include <dlog.h>
#include <vconf.h>

#include "voice_control_panel_main.h"
#include "voice_control_panel_action.h"
#include "voice_control_panel_command.h"
#include "voice_control_panel_view.h"

static void __vc_panel_action_send_key_event(void *data, Ecore_Thread *thread)
{
	LOGD( "==== Send Key Event ====");

	int keynum = (int)data;
	LOGD( "Key - %d", keynum);
	
	int fd;
	fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (0 > fd) {
		LOGE( "[ERROR] Fail to open dev");
		return;
	}

	int ret;
	struct uinput_user_dev uidev;
	memset(&uidev, 0, sizeof(uidev));
	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "vc-keyevent");
	uidev.id.bustype = BUS_USB;
	uidev.id.vendor = 1;
	uidev.id.product = 1;
	uidev.id.version = 1;

	ret = write(fd, &uidev, sizeof(uidev));
	if (sizeof(uidev) != ret) {
		LOGE( "[ERROR] Fail to write info");
		close(fd);
		return;
	}

	ret = ioctl(fd, UI_SET_EVBIT, EV_KEY);
	if (0 != ret) {
		LOGE( "[ERROR] Fail to ioctl");
		close(fd);
		return;
	}

	ret = ioctl(fd, UI_SET_EVBIT, EV_SYN);
	if (0 != ret) {
		LOGE( "[ERROR] Fail to ioctl");
		close(fd);
		return;
	}

	ret = ioctl (fd, UI_SET_KEYBIT, keynum);
	if (0 != ret) {
		LOGE( "[ERROR] Fail to register key");
		close(fd);
		return;
	}

	ret = ioctl(fd, UI_DEV_CREATE);
	if (0 != ret) {
		LOGE( "[ERROR] Fail to create");
		close(fd);
		return;
	}

	usleep(1000000);

	struct input_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.type = EV_KEY;
	ev.code = keynum;
	ev.value = 1;

	ret = write(fd, &ev, sizeof(ev));
	if (0 > ret) {
		LOGE( "[ERROR] Fail to send key event");
		close(fd);
		return;
	}

	memset(&ev, 0, sizeof(ev));
	ev.type = EV_SYN;
	ev.code = 0;
	ev.value = 0;

	ret = write(fd, &ev, sizeof(ev));
	if (0 > ret) {
		LOGE( "[ERROR] Fail to send sync event");
		close(fd);
		return;
	}

	usleep(1000000);

	ret = ioctl(fd, UI_DEV_DESTROY);
	if (0 != ret) {
		LOGE( "[ERROR] Fail to destroy");
		close(fd);
		return;
	}

	LOGD( "====");
	LOGD( " ");

	close(fd);
	return;
}

static void __vc_panel_action_launch_app(const char* app_id)
{
	LOGD("==== Launch app(%s) ====", app_id);
	app_control_h app_control;
	app_control_create(&app_control);
	app_control_set_app_id(app_control, app_id);
	app_control_send_launch_request(app_control, NULL, NULL);
	app_control_destroy(app_control);
	LOGD("====");
	LOGD("");
}

bool vc_panel_action(const char* result, void *data)
{
	if (NULL == result)
		return false;

	LOGD( "==== Action - %s ====", result);

	appdata *ad = (appdata *)data;

	if (1 == ad->current_depth) {
		if (!strcmp(result, _("IDS_HOMESCREEN"))) {
			char *path = NULL;
			path = vconf_get_str(VCONFKEY_SETAPPL_SELECTED_PACKAGE_NAME);
			if (NULL != path) {
				__vc_panel_action_launch_app(path);
				free(path);
			}
			return true;
		} else if (!strcmp(result, _("IDS_NOTIFICATION"))) {
			__vc_panel_action_launch_app("org.tizen.quickpanel");
			return true;
		} else if (!strcmp(result, _("IDS_INTERNET"))) {
			__vc_panel_action_launch_app("org.tizen.browser");
			return true;
		} else if (!strcmp(result, _("IDS_TASK_MANAGER"))) {
			__vc_panel_action_launch_app("org.tizen.task-mgr");
			return true;
		} else if (!strcmp(result, _("IDS_SCROLL_UP"))) {
			LOGD("Scroll up");
			return true;
		} else if (!strcmp(result, _("IDS_SCROLL_DOWN"))) {
			LOGD("Scroll down");
			return true;
		} else if (!strcmp(result, _("IDS_HELP"))) {
			if (0 == ad->show_help) {
				vc_panel_view_show_help(ad);
			} else {
				vc_panel_view_hide_help(ad);
			}
			return true;
		} else if (!strcmp(result, _("IDS_CLOSE"))) {
			return false;
		}
		
		int i;
		for (i = 0; i < NUM_COMMAND_1ST; i++) {
			if (NULL == g_command_1st[i])
				continue;

			if (!strcmp(result, _(g_command_1st[i]))) {
				ad->current_path[0] = i;
				ad->current_depth = 2;
			}
		}
	}

	LOGD( "====");
	LOGD( " ");

	return true;
}

/*
vi:ts=4:ai:nowrap:expandtab
*/
