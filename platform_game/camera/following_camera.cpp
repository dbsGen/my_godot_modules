//
// Created by gen on 15-7-1.
//

#include "following_camera.h"
#include "../../../core/math/math_2d.h"
#include "../../../scene/2d/node_2d.h"
#include <core/bind/core_bind.h>

void FollowingCamera::_notification(int p_notification) {
    switch (p_notification) {
        case NOTIFICATION_ENTER_TREE: {
            update_target();
            break;
        }
    }
}

Transform2D FollowingCamera::get_camera_transform()  {
    if (_Engine::get_singleton()->is_editor_hint())
        return Transform2D();
    Vector2 view_size = get_viewport_rect().size * get_zoom();
    Vector2 global_pos = get_global_position();
    Vector2 offset = get_offset();
    if (target != NULL) {
        if (shaking_during > 0) {
            global_pos = shake_base + Math::sin(shaking_time*31.4)*shake_force;
        }else {
            Vector2 target_pos = target->get_global_position() + offset;
            float min_pos_x = limit_left + view_size.x/2 - offset.x;
            float min_pos_y = limit_top + view_size.y/2 - offset.x;
            float max_pos_x = limit_right - view_size.x/2 - offset.y;
            float max_pos_y = limit_bottom - view_size.y/2 - offset.y;
            target_pos.x = MIN(MAX(target_pos.x, min_pos_x), max_pos_x);
            target_pos.y = MIN(MAX(target_pos.y, min_pos_y), max_pos_y);
            if (target_pos.distance_to(global_pos) > 2) {
                global_pos = global_pos*(1-follow)+target_pos*follow;
            }
        }
        if (shaking_time < shaking_during) {
            shaking_time += get_fixed_process_delta_time();
            if (shaking_time > shaking_during) {
                shaking_during = 0;
            }
        }
    }
    set_global_position(global_pos);

    camera_screen_center=global_pos;

    Transform2D xform;
    if(rotating){
        xform.set_rotation(get_global_transform().get_rotation());
    }
    xform.scale_basis(get_zoom());
    Vector2 point = (global_pos - view_size*0.5)/get_zoom();
    point.x = (float)Math::round(point.x);
    point.y = (float)Math::round(point.y);
    point=point*get_zoom();
    xform.set_origin(point);
    return (xform).affine_inverse();
}

void FollowingCamera::update_target() {
    if (is_inside_tree() && target_path != NodePath() && has_node(target_path)) {
        target = Object::cast_to<Node2D>(get_node(target_path));
        if (target) {
            target->connect("exit_tree", this, "_on_target_exit");
        }
    }else {
        target = NULL;
    }
}

void FollowingCamera::_on_target_exit() {
    target->disconnect("exit_tree", this, "_on_target_exit");
    target_path = NodePath();
    target = NULL;
}

void FollowingCamera::shake(float during, Vector2 force) {
    shake_base = get_global_position();
    shaking_during = during;
    shaking_time = 0;
    shake_force = force;
}

void FollowingCamera::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_target_path", "target_path"), &FollowingCamera::set_target_path);
    ClassDB::bind_method(D_METHOD("get_target_path"), &FollowingCamera::get_target_path);

    ClassDB::bind_method(D_METHOD("set_follow", "follow"), &FollowingCamera::set_follow);
    ClassDB::bind_method(D_METHOD("get_follow"), &FollowingCamera::get_follow);

    ClassDB::bind_method(D_METHOD("_on_target_exit"), &FollowingCamera::_on_target_exit);

    ClassDB::bind_method(D_METHOD("set_limit_left", "limit_left"), &FollowingCamera::set_limit_left);
    ClassDB::bind_method(D_METHOD("get_limit_left"), &FollowingCamera::get_limit_left);

    ClassDB::bind_method(D_METHOD("set_limit_top", "limit_top"), &FollowingCamera::set_limit_top);
    ClassDB::bind_method(D_METHOD("get_limit_top"), &FollowingCamera::get_limit_top);

    ClassDB::bind_method(D_METHOD("set_limit_right", "limit_right"), &FollowingCamera::set_limit_right);
    ClassDB::bind_method(D_METHOD("get_limit_right"), &FollowingCamera::get_limit_right);

    ClassDB::bind_method(D_METHOD("set_limit_bottom", "limit_bottom"), &FollowingCamera::set_limit_bottom);
    ClassDB::bind_method(D_METHOD("get_limit_bottom"), &FollowingCamera::get_limit_bottom);

    ClassDB::bind_method(D_METHOD("shake", "during", "force"), &FollowingCamera::shake);

    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "target_path"), "set_target_path", "get_target_path");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "follow"), "set_follow", "get_follow");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "limit_left"), "set_limit_left", "get_limit_left");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "limit_top"), "set_limit_top", "get_limit_top");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "limit_right"), "set_limit_right", "get_limit_right");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "limit_bottom"), "set_limit_bottom", "get_limit_bottom");
}