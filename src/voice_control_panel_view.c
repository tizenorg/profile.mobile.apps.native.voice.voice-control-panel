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

#include <app_control.h>
#include <dlog.h>
#include <Elementary.h>

#include "voice_control_panel_command.h"
#include "voice_control_panel_main.h"
#include "voice_control_panel_view.h"

#define MAIN_LAYOUT_EDJ		"/usr/apps/org.tizen.voice-control-panel/res/edje/voice-control-panel.edj"
#define MIC_IMAGE		"/usr/apps/org.tizen.voice-control-panel/res/images/Voice_control_icon_mic.png"
#define MUTE_IMAGE		"/usr/apps/org.tizen.voice-control-panel/res/images/Voice_control_icon_mute.png"
#define SETTING_IMAGE		"/usr/apps/org.tizen.voice-control-panel/res/images/Voice_control_icon_setting.png"
#define ARROW_IMAGE		"/usr/apps/org.tizen.voice-control-panel/res/images/Voice_control_icon_arrow.png"

static Elm_Genlist_Item_Class *g_itc = NULL;
static Elm_Genlist_Item_Class *g_itc_for_app = NULL;
static appdata *g_ad = NULL;

static char *__help_text_get(void *data, Evas_Object *obj, const char *part)
{
	if (NULL == part) {
		return NULL;
	}

	int idx = (int *)data;

	if (!strcmp("elm.text.main.left.top", part)) {
		if (1 == g_ad->current_depth) {
			return strdup(_(g_command_1st[idx]));
		} else if (2 == g_ad->current_depth) {
			return strdup(_(g_command_2nd[g_ad->current_path[0]][idx]));
		}
	} else if (!strcmp("elm.text.sub.left.bottom", part)) {
		if (1 == g_ad->current_depth) {
			return strdup(_(g_hint_1st[idx]));
		} else if (2 == g_ad->current_depth) {
			return strdup(_(g_hint_2nd[g_ad->current_path[0]][idx]));
		}
	}
	return NULL;
}

static char *__help_text_get_for_app(void *data, Evas_Object *obj, const char *part)
{
	if (NULL == part || NULL == data) {
		return NULL;
	}

	char *cmd = (char *)data;

	if (!strcmp("elm.text.main.left", part)) {
		LOGD("cmd (%s)", cmd);
		return strdup(cmd);
	}
	return NULL;
}

int vc_panel_view_show_help(void *data)
{
	LOGD( "==== Show help ====");

	appdata *ad = (appdata *)data;

	if (NULL != ad->help_genlist) {
		elm_genlist_clear(ad->help_genlist);
	}

	int i;
	Elm_Object_Item *it = NULL;
	if (1 == ad->current_depth) {
		for (i = 0; i < NUM_COMMAND_1ST; i++) {
			if (NULL != g_command_1st[i]) {
				it = elm_genlist_item_append(ad->help_genlist, g_itc, (void *)i, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
				elm_genlist_item_select_mode_set(it, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
			}
		}
	} else if (2 == ad->current_depth) {
		for (i = 0; i < NUM_COMMAND_2ND; i++) {
			if (NULL != g_command_2nd[ad->current_path[0]][i]) {
				it = elm_genlist_item_append(ad->help_genlist, g_itc, (void *)i, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
				elm_genlist_item_select_mode_set(it, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
			}
		}
	}

	GList *iter = NULL;
	for (i = 0; i < g_list_length(ad->cmd_list); i++) {
		iter = g_list_nth(ad->cmd_list, i);
		if (NULL != iter) {
			char *cmd = iter->data;
			if (NULL != cmd) {
				LOGD("Add list - (%s)", cmd);
				char *tmp = strdup(cmd);
				it = elm_genlist_item_append(ad->help_genlist, g_itc_for_app, (void *)tmp, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
				elm_genlist_item_select_mode_set(it, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
			}
		}
	}

	evas_object_show(ad->help_win);
	ad->show_help = 1;

	LOGD( "====");
	LOGD( " ");
	
	return 0;
}

int vc_panel_view_hide_help(void *data)
{
	LOGD( "==== Hide help ====");

	appdata *ad = (appdata *)data;

	evas_object_hide(ad->help_win);
	ad->show_help = 0;

	return 0;
}

int vc_panel_view_show_result(void *data, const char *result)
{
	LOGD("==== Show result ====");

	appdata *ad = (appdata *)data;

	elm_box_clear(ad->content_box);

	Evas_Object *label = elm_label_add(ad->content_box);
	evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(label, 0.0, 0.0);
	char text[256] = {'\0', };
	snprintf(text, 256, "<font=Tizen:style=Regular><font_size=30><color=#FFFFFFFF>%s</color></font_size></font>", result);
	elm_object_text_set(label, text);
	elm_box_pack_end(ad->content_box, label);
	evas_object_show(label);

	return 0;
}

int vc_panel_view_show(void *data)
{
	LOGD( "==== Show content ====");

	appdata *ad = (appdata *)data;
	LOGD( "Current Depth = %d", ad->current_depth);

	Evas_Object *box = elm_box_add(ad->layout_main);
	elm_box_padding_set(box, 0, 0);
	elm_box_horizontal_set(box, EINA_TRUE);
	elm_box_homogeneous_set(box, EINA_FALSE);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_box_align_set(box, 0.0, 0.0);
	int image_size = 42 * ad->scale_h;

	if (1 == ad->current_depth) {
		Evas_Object *label = elm_label_add(box);
		evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(label, 0.0, 0.0);
		char text1[256] = {'\0', };
		snprintf(text1, 256, "<font=Tizen:style=Regular><font_size=30><color=#FFFFFFFF>%s</color></font_size></font>", _("IDS_LISTENING"));
		elm_object_text_set(label, text1);
		elm_box_pack_end(box, label);
		evas_object_show(label);
	} else if (2 == ad->current_depth) {
		Evas_Object *label1 = elm_label_add(box);
		evas_object_size_hint_align_set(label1, 0.0, 0.5);
		char text1[256] = {'\0', };
		snprintf(text1, 256, "<font=Tizen:style=Regular><font_size=30><color=#FFFFFFB8>%s</color></font_size></font>", _(g_command_1st[ad->current_path[0]]));
		elm_object_text_set(label1, text1);
		elm_box_pack_end(box, label1);
		evas_object_show(label1);

		Evas_Object *image_arrow = elm_image_add(box);
		elm_image_file_set(image_arrow, ARROW_IMAGE, NULL);
		evas_object_size_hint_min_set(image_arrow, image_size, image_size);
		elm_box_pack_end(box, image_arrow);
		evas_object_show(image_arrow);

		Evas_Object *label2 = elm_label_add(box);
		evas_object_size_hint_align_set(label2, 0.0, 0.5);
		char text2[256] = {'\0', };
		snprintf(text2, 256, "<font=Tizen:style=Regular><font_size=30><color=#FFFFFFFF>%s</color><font_size></font>", _("IDS_LISTENING"));
		elm_object_text_set(label2, text2);
		elm_box_pack_end(box, label2);
		evas_object_show(label2);
	}

	elm_object_part_content_set(ad->layout_main, "content", box);
	evas_object_show(box);
	ad->content_box = box;

	evas_object_show(ad->win);

	return 0;
}

int vc_panel_view_hide(void *data)
{
	appdata *ad = (appdata *)data;

	evas_object_hide(ad->win);

	vc_panel_view_hide_help(ad);

	return 0;
}

static void __vc_panel_setting_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	LOGD("==== Setting Clicked ====");
	
	app_control_h app_control;
	app_control_create(&app_control);
	app_control_set_app_id(app_control, "org.tizen.voice-setting");
	app_control_add_extra_data(app_control, "show", "voice-control");
	app_control_send_launch_request(app_control, NULL, NULL);
	app_control_destroy(app_control);

	ui_app_exit();

	LOGD("====");
	LOGD("");
}

int vc_panel_view_create(void *data)
{
	LOGD( "==== Create View ====");

	appdata *ad = (appdata *)data;
	g_ad = ad;

	elm_theme_extension_add(NULL, MAIN_LAYOUT_EDJ);

	/* Window */
	Evas_Object *win = elm_win_add(NULL, "voice-control-panel", ELM_WIN_UTILITY);
	int x,y,w,h;
	elm_win_screen_size_get(win, &x, &y, &w, &h);
	LOGD("Size x(%d), y(%d), w(%d), h(%d)", x, y, w, h);
	elm_win_autodel_set(win, EINA_TRUE);
	elm_win_title_set(win, "voice-control-panel");
	elm_win_borderless_set(win, EINA_TRUE);
	elm_win_alpha_set(win, EINA_TRUE);
	
	ad->scale_w = w / 720.0;
	ad->scale_h = h / 1280.0;

	int scaled_h = 90 * ad->scale_h;

	elm_win_aux_hint_add(win, "wm.policy.win.user.geometry", "1");
	elm_win_prop_focus_skip_set(win, EINA_TRUE);

	evas_object_resize(win, w, scaled_h);
	evas_object_move(win, 0, h - scaled_h);

	ad->win = win;

	/* Layout Main */
	Evas_Object *layout = elm_layout_add(win);
	elm_layout_file_set(layout, MAIN_LAYOUT_EDJ, "main");
	elm_win_resize_object_add(win, layout);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(layout);
	ad->layout_main = layout;

	/* Image MIC */
	Evas_Object *image_mic = elm_image_add(layout);
	elm_image_file_set(image_mic, MIC_IMAGE, NULL);
	elm_object_part_content_set(layout, "icon1", image_mic);
	evas_object_show(image_mic);
	ad->image_mic = image_mic;

	/* Image arrow */
	Evas_Object *image_arrow = elm_image_add(layout);
	elm_image_file_set(image_arrow, ARROW_IMAGE, NULL);
	elm_object_part_content_set(layout, "icon2", image_arrow);
	evas_object_show(image_arrow);
	ad->image_arrow = image_arrow;

	/* Image Setting */
	Evas_Object *image_setting = elm_image_add(layout);
	elm_image_file_set(image_setting, SETTING_IMAGE, NULL);
	elm_object_part_content_set(layout, "icon3", image_setting);
	evas_object_smart_callback_add(image_setting, "clicked", __vc_panel_setting_clicked_cb, ad);
	evas_object_show(image_setting);
	ad->image_setting = image_setting;

	ad->current_depth = 1;

	Evas_Object *help_win = elm_win_add(NULL, "voice-control-panel-help", ELM_WIN_UTILITY);
	elm_win_autodel_set(help_win, EINA_TRUE);
	elm_win_title_set(help_win, "voice-control-panel-help");
	elm_win_borderless_set(help_win, EINA_TRUE);
	elm_win_alpha_set(help_win, EINA_TRUE);

	elm_win_aux_hint_add(help_win, "wm.policy.win.user.geometry", "1");
	elm_win_prop_focus_skip_set(help_win, EINA_TRUE);

	evas_object_resize(help_win, w, h / 2);
	evas_object_move(help_win, 0, h - (h / 2) - (90 * ad->scale_h));

	ad->help_win = help_win;

	Evas_Object *help_layout = elm_layout_add(help_win);
	elm_layout_file_set(help_layout, "/usr/apps/org.tizen.voice-control-panel/res/edje/voice-control-panel.edj", "help");
	elm_win_resize_object_add(help_win, help_layout);
	evas_object_size_hint_weight_set(help_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(help_layout);
	ad->help_layout = help_layout;

	Evas_Object *genlist = elm_genlist_add(ad->help_layout);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_genlist_homogeneous_set(genlist, EINA_FALSE);
	elm_object_part_content_set(ad->help_layout, "content", genlist);
	evas_object_show(genlist);
	ad->help_genlist = genlist;

	g_itc = elm_genlist_item_class_new();
	if (NULL == g_itc) {
		LOGE("Fail to new item class");
		return -1;
	}
	g_itc->item_style = "2line.top";
	g_itc->func.text_get = __help_text_get;
	g_itc->func.content_get = NULL;
	g_itc->func.state_get = NULL;
	g_itc->func.del = NULL;

	g_itc_for_app = elm_genlist_item_class_new();
	if (NULL == g_itc_for_app) {
		LOGE("Fail to new item class");
		return -1;
	}
	g_itc_for_app->item_style = "1line";
	g_itc_for_app->func.text_get = __help_text_get_for_app;
	g_itc_for_app->func.content_get = NULL;
	g_itc_for_app->func.state_get = NULL;
	g_itc_for_app->func.del = NULL;

	ad->show_help = 0;

	LOGD( "====");
	LOGD( " ");

	return 0;
}

void vc_panel_view_destroy(void *data)
{
	LOGD("");
	if (NULL != g_itc) {
		elm_genlist_item_class_free(g_itc);
		g_itc = NULL;
	}

	if (NULL != g_itc_for_app) {
		elm_genlist_item_class_free(g_itc_for_app);
		g_itc_for_app = NULL;
	}

	return;
}

/*
vi:ts=4:ai:nowrap:expandtab
*/
