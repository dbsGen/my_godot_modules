//
// Created by gen on 15-7-18.
//

#include "scene_map.h"

int SceneMap::convert_x(int x) {
    return start_x+x;
}

int SceneMap::convert_y(int y) {
    return start_y+y;
}

void SceneMap::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("get_start_x"), &SceneMap::get_start_x);
    ObjectTypeDB::bind_method(_MD("set_start_x", "start_x"), &SceneMap::set_start_x);

    ObjectTypeDB::bind_method(_MD("get_start_y"), &SceneMap::get_start_y);
    ObjectTypeDB::bind_method(_MD("set_start_y", "start_y"), &SceneMap::set_start_y);

    ObjectTypeDB::bind_method(_MD("get_width"), &SceneMap::get_width);
    ObjectTypeDB::bind_method(_MD("set_width", "width"), &SceneMap::set_width);

    ObjectTypeDB::bind_method(_MD("get_height"), &SceneMap::get_height);
    ObjectTypeDB::bind_method(_MD("set_height", "height"), &SceneMap::set_height);

    ObjectTypeDB::bind_method(_MD("get_texture:Texture"), &SceneMap::get_texture);
    ObjectTypeDB::bind_method(_MD("set_texture", "texture:Texture"), &SceneMap::set_texture);

    ObjectTypeDB::bind_method(_MD("convert_x", "x"), &SceneMap::convert_x);
    ObjectTypeDB::bind_method(_MD("convert_y", "y"), &SceneMap::convert_y);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "start_x"), _SCS("set_start_x"), _SCS("get_start_x"));
    ADD_PROPERTY(PropertyInfo(Variant::INT, "start_y"), _SCS("set_start_y"), _SCS("get_start_y"));
    ADD_PROPERTY(PropertyInfo(Variant::INT, "width"), _SCS("set_width"), _SCS("get_width"));
    ADD_PROPERTY(PropertyInfo(Variant::INT, "height"), _SCS("set_height"), _SCS("get_height"));
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), _SCS("set_texture"), _SCS("get_texture"));
}