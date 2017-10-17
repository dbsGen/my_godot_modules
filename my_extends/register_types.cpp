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
#include "new_timer.h"
#include "core/project_settings.h"
#include "new_tween.h"
#include "inputstorage.h"
#include "trail_2d.h"
#include "phantom.h"
#include "remote_pos_rot_2d.h"

void register_my_extends_types() {
    ProjectSettings::get_singleton()->add_singleton(ProjectSettings::Singleton("NewTimer", NewTimer::get_singleton()));
    ProjectSettings::get_singleton()->add_singleton(ProjectSettings::Singleton("NewTween", memnew(NewTween)));
    ProjectSettings::get_singleton()->add_singleton(ProjectSettings::Singleton("InputStorage", InputStorage::get_singleton()));
	ClassDB::register_class<NewTimer>();
    ClassDB::register_class<TimerObject>();
    ClassDB::register_class<NewTween>();
    ClassDB::register_class<TweenAction>();
    ClassDB::register_class<InputNode>();
    ClassDB::register_class<InputStorage>();
    ClassDB::register_class<InputStorageNode>();
    ClassDB::register_class<TrailPoint2D>();
    ClassDB::register_class<TrailLine2D>();
    ClassDB::register_class<Phantom>();
    ClassDB::register_class<RemotePosRot2D>();
}

void unregister_my_extends_types() {
}
