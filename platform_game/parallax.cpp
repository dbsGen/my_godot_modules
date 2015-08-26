//
// Created by gen on 15-8-15.
//

#include "parallax.h"
#include "../../core/object_type_db.h"
#include "../../scene/main/viewport.h"


void ParallaxBG::_notification(int p_what) {

    switch(p_what) {

        case NOTIFICATION_ENTER_TREE: {
            set_camera = false;
            original_position = get_global_pos();

            group_name = "__cameras_"+itos(get_viewport()->get_viewport().get_id());
            add_to_group(group_name);

        } break;
        case NOTIFICATION_EXIT_TREE: {

            remove_from_group(group_name);
        } break;
    }

}

void ParallaxBG::_camera_moved(const Matrix32 &p_transform) {
    if (get_tree()->is_editor_hint())
        return;
    if (set_camera) {
        const Vector2& off = p_transform.get_origin();
        set_global_pos((camera_base_position-off)*(Vector2(1,1)-scroll/2)+original_position);
    }else{
        camera_base_position = p_transform.get_origin();
        set_camera = true;
    }
}

void ParallaxBG::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("_camera_moved"),&ParallaxBG::_camera_moved);

    ObjectTypeDB::bind_method(_MD("get_scroll"), &ParallaxBG::get_scroll);
    ObjectTypeDB::bind_method(_MD("set_scroll", "scroll"), &ParallaxBG::set_scroll);

    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "scroll"), _SCS("set_scroll"), _SCS("get_scroll"));
}