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
#include "character.h"
#include "attack_area.h"
#include "buff.h"
#include "hit_status/hit_status.h"
#include "behaviornodes/buff_process.h"
#include "behaviornodes/hit_status_progress.h"
#include "behaviornodes/gravity.h"
#include "behaviornodes/action.h"
#include "behaviornodes/attack_node.h"
#include "behaviornodes/cancel_node.h"
#include "camera/following_camera.h"
#include "kill_zone.h"
#include "map/main_map.h"
#include "map/scene_map.h"
#include "parallax.h"
#include "anim_controller.h"
#include "hit_area.h"
#include "barrage.h"
#include "graze_area.h"

void register_platform_game_types() {
    HitStatusProgress::PROCESS_HIT_NAME = "process_hit";
    HitStatusProgress::TYPE_CHANGE_NAME = "_on_type_change";
    Action::CHECK_NAME = "_check_action";
    
    Character::COMBO_BEGIN_NAME = "combo_begin";
    Character::COMBO_END_NAME = "combo_end";
    Character::COMBO_CHANGE_NAME = "combo_change";

	ClassDB::register_class<Character>();
    ClassDB::register_class<Buff>();
    ClassDB::register_class<BuffProcess>();
    ClassDB::register_class<AttackArea>();
    ClassDB::register_class<HitStatus>();
    ClassDB::register_class<HitStatusProgress>();
    ClassDB::register_class<CancelNode>();
    ClassDB::register_class<Gravity>();
    ClassDB::register_class<Action>();
    ClassDB::register_class<AttackNode>();
    ClassDB::register_class<FollowingCamera>();
    ClassDB::register_class<KillZone>();
    ClassDB::register_class<MainMap>();
    ClassDB::register_class<SceneMap>();
    ClassDB::register_class<ParallaxBG>();
    ClassDB::register_class<AnimController>();
    ClassDB::register_class<HitArea>();
    ClassDB::register_class<Barrage>();
    ClassDB::register_class<Bullet>();
    ClassDB::register_class<ShootBarrage>();
    ClassDB::register_class<ScatterBarrage>();
    ClassDB::register_class<RandomBarrage>();
    ClassDB::register_class<GrazeArea>();
}

void unregister_platform_game_types() {
    HitStatusProgress::PROCESS_HIT_NAME = StringName();
    HitStatusProgress::TYPE_CHANGE_NAME = StringName();
    Action::CHECK_NAME = StringName();
    Character::COMBO_BEGIN_NAME = StringName();
    Character::COMBO_END_NAME = StringName();
    Character::COMBO_CHANGE_NAME = StringName();
}
