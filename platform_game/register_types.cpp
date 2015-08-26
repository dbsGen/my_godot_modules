/*************************************************************************/
/*  register_types.cpp                                                   */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#include "register_types.h"
#include "object_type_db.h"
#include "character.h"
#include "attack_area.h"
#include "buff.h"
#include "hit_status/hit_status.h"
#include "behaviornodes/buff_process.h"
#include "behaviornodes/hit_status_progress.h"
#include "behaviornodes/gravity.h"
#include "behaviornodes/action.h"
#include "behaviornodes/attack_node.h"
#include "camera/following_camera.h"
#include "story_script.h"
#include "kill_zone.h"
#include "map/main_map.h"
#include "map/scene_map.h"
#include "parallax.h"

void register_platform_game_types() {
	ObjectTypeDB::register_type<Character>();
    ObjectTypeDB::register_type<Buff>();
    ObjectTypeDB::register_type<BuffProcess>();
    ObjectTypeDB::register_type<AttackArea>();
    ObjectTypeDB::register_type<HitStatus>();
    ObjectTypeDB::register_type<HitStatusProgress>();
    ObjectTypeDB::register_type<Gravity>();
    ObjectTypeDB::register_type<Action>();
    ObjectTypeDB::register_type<AttackNode>();
    ObjectTypeDB::register_type<FollowingCamera>();
    ObjectTypeDB::register_type<StoryScript>();
    ObjectTypeDB::register_type<StoryScriptNode>();
    ObjectTypeDB::register_type<KillZone>();
    ObjectTypeDB::register_type<MainMap>();
    ObjectTypeDB::register_type<SceneMap>();
    ObjectTypeDB::register_type<ParallaxBG>();
}

void unregister_platform_game_types() {
}
