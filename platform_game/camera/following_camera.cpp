//
// Created by gen on 15-7-1.
//

#include "following_camera.h"
#include "../../../core/math/math_2d.h"
#include "../../../scene/2d/node_2d.h"
#include "../../../core/object_type_db.h"

void FollowingCamera::_notification(int p_notification) {
    switch (p_notification) {
        case NOTIFICATION_ENTER_TREE: {
            update_target();
            break;
        }
    }
}

Matrix32 FollowingCamera::get_camera_transform()  {
    if (!get_tree() || get_tree()->is_editor_hint())
        return Matrix32();
    Vector2 view_size = get_viewport_rect().size * get_zoom();
    Vector2 global_pos = get_global_pos();
    if (target != NULL) {
        if (shaking_during > 0) {
            global_pos = shake_base + Math::sin(shaking_time*31.4)*shake_force;
        }else {
            Vector2 target_pos = target->get_global_pos();
            float min_pos_x = limit_left + view_size.x/2;
            float min_pos_y = limit_top + view_size.y/2;
            float max_pos_x = limit_right - view_size.x/2;
            float max_pos_y = limit_bottom - view_size.y/2;
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
    set_global_pos(global_pos);

    camera_screen_center=global_pos;

    Matrix32 xform;
    if(rotating){
        xform.set_rotation(get_global_transform().get_rotation());
    }
    xform.scale_basis(get_zoom());
    Vector2 point = (global_pos - view_size*0.5)/get_zoom();
    point.x = Math::round(point.x);
    point.y = Math::round(point.y);
    point=point*get_zoom();
    xform.set_origin(point);
    return (xform).affine_inverse();
}

void FollowingCamera::update_target() {
    if (is_inside_tree() && target_path != NodePath() && has_node(target_path)) {
        target = get_node(target_path)->cast_to<Node2D>();
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
    shake_base = get_global_pos();
    shaking_during = during;
    shaking_time = 0;
    shake_force = force;
}

void FollowingCamera::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("get_target_path", "target_path"), &FollowingCamera::get_target_path);
    ObjectTypeDB::bind_method(_MD("set_target_path"), &FollowingCamera::set_target_path);

    ObjectTypeDB::bind_method(_MD("get_follow", "follow"), &FollowingCamera::get_follow);
    ObjectTypeDB::bind_method(_MD("set_follow"), &FollowingCamera::set_follow);

    ObjectTypeDB::bind_method(_MD("_on_target_exit"), &FollowingCamera::_on_target_exit);

    ObjectTypeDB::bind_method(_MD("get_limit_left", "limit_left"), &FollowingCamera::get_limit_left);
    ObjectTypeDB::bind_method(_MD("set_limit_left"), &FollowingCamera::set_limit_left);

    ObjectTypeDB::bind_method(_MD("get_limit_top", "limit_top"), &FollowingCamera::get_limit_top);
    ObjectTypeDB::bind_method(_MD("set_limit_top"), &FollowingCamera::set_limit_top);

    ObjectTypeDB::bind_method(_MD("get_limit_right", "limit_right"), &FollowingCamera::get_limit_right);
    ObjectTypeDB::bind_method(_MD("set_limit_right"), &FollowingCamera::set_limit_right);

    ObjectTypeDB::bind_method(_MD("get_limit_bottom", "limit_bottom"), &FollowingCamera::get_limit_bottom);
    ObjectTypeDB::bind_method(_MD("set_limit_bottom"), &FollowingCamera::set_limit_bottom);

    ObjectTypeDB::bind_method(_MD("shake", "during", "force"), &FollowingCamera::shake);

    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "target_path"), _SCS("set_target_path"), _SCS("get_target_path"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "follow"), _SCS("set_follow"), _SCS("get_follow"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "limit_left"), _SCS("set_limit_left"), _SCS("get_limit_left"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "limit_top"), _SCS("set_limit_top"), _SCS("get_limit_top"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "limit_right"), _SCS("set_limit_right"), _SCS("get_limit_right"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "limit_bottom"), _SCS("set_limit_bottom"), _SCS("get_limit_bottom"));
}