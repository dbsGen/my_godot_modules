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
    ClassDB::bind_method(D_METHOD("get_start_x"), &SceneMap::get_start_x);
    ClassDB::bind_method(D_METHOD("set_start_x", "start_x"), &SceneMap::set_start_x);

    ClassDB::bind_method(D_METHOD("get_start_y"), &SceneMap::get_start_y);
    ClassDB::bind_method(D_METHOD("set_start_y", "start_y"), &SceneMap::set_start_y);

    ClassDB::bind_method(D_METHOD("get_width"), &SceneMap::get_width);
    ClassDB::bind_method(D_METHOD("set_width", "width"), &SceneMap::set_width);

    ClassDB::bind_method(D_METHOD("get_height"), &SceneMap::get_height);
    ClassDB::bind_method(D_METHOD("set_height", "height"), &SceneMap::set_height);

    ClassDB::bind_method(D_METHOD("get_texture"), &SceneMap::get_texture);
    ClassDB::bind_method(D_METHOD("set_texture", "texture:Texture"), &SceneMap::set_texture);

    ClassDB::bind_method(D_METHOD("convert_x", "x"), &SceneMap::convert_x);
    ClassDB::bind_method(D_METHOD("convert_y", "y"), &SceneMap::convert_y);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "start_x"), "set_start_x", "get_start_x");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "start_y"), "set_start_y", "get_start_y");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "width"), "set_width", "get_width");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "height"), "set_height", "get_height");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), "set_texture", "get_texture");
}