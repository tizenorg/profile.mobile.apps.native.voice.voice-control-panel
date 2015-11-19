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

#ifndef __VOICE_CONTROL_PANEL_COMMAND_H
#define __VOICE_CONTROL_PANEL_COMMAND_H

#ifdef __cplusplus
extern "C" {
#endif

#define	NUM_COMMAND_DEPTH	2

#define NUM_COMMAND_1ST		6
#define NUM_COMMAND_2ND		8

extern char* g_command_1st[NUM_COMMAND_1ST];
extern char* g_command_2nd[NUM_COMMAND_1ST][NUM_COMMAND_2ND];

extern char* g_hint_1st[NUM_COMMAND_1ST];
extern char* g_hint_2nd[NUM_COMMAND_1ST][NUM_COMMAND_2ND];

#ifdef __cplusplus
}
#endif

#endif /* __VOICE_CONTROL_PANEL_VIEW_H */

/*
vi:ts=4:ai:nowrap:expandtab
*/
