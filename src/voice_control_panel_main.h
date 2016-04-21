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

#ifndef __VOICE_CONTROL_PANEL_MAIN_H
#define __VOICE_CONTROL_PANEL_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <app.h>
#include <glib.h>
#include <Elementary.h>
#include <dlog.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG	"vcp"
//#define _(S)	gettext(S)

enum {
	APP_STATE_INIT = 1,
	APP_STATE_PAUSE,
	APP_STATE_SERVICE,
	APP_STATE_TERMINATE,
};

typedef struct _appdata {
	/* GUI */
	float scale_w;
	float scale_h;

	Evas_Object *win;
	Evas_Object *layout_main;
	Evas_Object *image_mic;
	Evas_Object *image_arrow;
	Evas_Object *content_box;
	Evas_Object *image_setting;

	Evas_Object *help_win;
	Evas_Object *help_genlist;
	Evas_Object *help_layout;

	Elm_Theme *theme;

	int app_state;

	int show_help;

	int current_depth;
	int current_path[2];

	GList *cmd_list;
}appdata;

#ifdef __cplusplus
}
#endif

#endif /* __VOICE_CONTROL_PANEL_MAIN_H */

/*
vi:ts=4:ai:nowrap:expandtab
*/
