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

#include "voice_control_panel_command.h"

char* g_command_1st[NUM_COMMAND_1ST] = {
	"IDS_HOMESCREEN", "IDS_INTERNET", "IDS_TASK_MANAGER", "IDS_SETTINGS", "IDS_HELP", "IDS_CLOSE"
};
char *g_hint_1st[NUM_COMMAND_1ST] = {
	"IDS_HINT_HOMESCREEN", "IDS_HINT_INTERNET", "IDS_HINT_TASK_MANAGER", "IDS_HINT_SETTINGS", "IDS_HINT_HELP", "IDS_HINT_CLOSE"
};

char* g_command_2nd[NUM_COMMAND_1ST][NUM_COMMAND_2ND] = {
	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	{"IDS_WIFI", "IDS_BLUETOOTH", "IDS_GPS","IDS_SOUND", "IDS_MORE", "IDS_PREVIOUS", "IDS_HELP", "IDS_CLOSE"},
	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
};

char* g_hint_2nd[NUM_COMMAND_1ST][NUM_COMMAND_2ND] = {
	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	{"IDS_HINT_WIFI", "IDS_HINT_BLUETOOTH", "IDS_HINT_GPS", "IDS_HINT_SOUND", "IDS_HINT_MORE", "IDS_HINT_PREVIOUS", "IDS_HINT_HELP", "IDS_HINT_CLOSE"},
	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
};
/*
vi:ts=4:ai:nowrap:expandtab
*/
