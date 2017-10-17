//
// Created by gen on 15-8-15.
//

#include "parallax.h"
#include "../../scene/main/viewport.h"
#include "../../core/math/math_funcs.h"
#include <core/bind/core_bind.h>


void ParallaxBG::_notification(int p_what) {

    switch(p_what) {

        case NOTIFICATION_ENTER_TREE: {
            set_camera = false;
            original_position = get_global_position();

            group_name = "__cameras_"+itos(get_viewport()->get_viewport_rid().get_id());
            add_to_group(group_name);

        } break;
        case NOTIFICATION_EXIT_TREE: {

            remove_from_group(group_name);
        } break;
    }

}

void ParallaxBG::_camera_moved(const Transform2D &p_transform) {
    if (_Engine::get_singleton()->is_editor_hint())
        return;
    if (set_camera) {
        const Vector2& off = p_transform.get_origin();
        Vector2 v2 = (camera_base_position-off)*(Vector2(1,1)-scroll/2)+original_position;
        v2.x = Math::ceil(v2.x);
        v2.y = Math::ceil(v2.y);
        set_global_position(v2);
    }else{
        camera_base_position = p_transform.get_origin();
        set_camera = true;
    }
}

void ParallaxBG::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_camera_moved"),&ParallaxBG::_camera_moved);

    ClassDB::bind_method(D_METHOD("get_scroll"), &ParallaxBG::get_scroll);
    ClassDB::bind_method(D_METHOD("set_scroll", "scroll"), &ParallaxBG::set_scroll);

    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "scroll"), "set_scroll", "get_scroll");
}