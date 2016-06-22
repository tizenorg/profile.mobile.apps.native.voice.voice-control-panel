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

#ifndef __VOICE_CONTROL_PANEL_VIEW_H
#define __VOICE_CONTROL_PANEL_VIEW_H

#ifdef __cplusplus
extern "C" {
#endif

int vc_panel_view_create(void *data);

void vc_panel_view_destroy(void *data);

int vc_panel_view_show(void *data);

int vc_panel_view_hide(void *data);

int vc_panel_view_show_help(void *data);

int vc_panel_view_hide_help(void *data);

int vc_panel_view_show_result(void *data, const char *result);

#ifdef __cplusplus
}
#endif

#endif /* __VOICE_CONTROL_PANEL_VIEW_H */

/*
vi:ts=4:ai:nowrap:expandtab
*/