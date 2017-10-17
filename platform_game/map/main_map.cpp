//
// Created by gen on 15-7-18.
//

#include "main_map.h"
#include "../../../core/math/math_funcs.h"
#include "../../../core/script_language.h"

void MainMap::update_size() {
    if (map_checked) {
        map_checked = (bool*)memrealloc((void*)map_checked, width*height*sizeof(bool));
    }else {
        map_checked = (bool*)memalloc(width*height*sizeof(bool));
    }
    for (int i = 0; i < width * height; ++i) {
        map_checked[i] = false;
    }
}

bool MainMap::checked_at(int x, int y) {
    ERR_FAIL_COND_V((x<0 || x >= width||y<0 || y >= height), false);
    return map_checked[x+y*width];
}

void MainMap::set_checked(int x, int y, bool checked) {
    ERR_FAIL_COND((x<0 || x >= width||y<0 || y >= height));
    if (map_checked[x+y*width] != checked) {
        map_checked[x+y*width] = checked;
        if (get_script_instance()) {
            get_script_instance()->call_multilevel(checked ? "turn_on":"turn_off", x, y);
        }
        emit_signal(checked ? "turn_on":"turn_off", x, y);
    }
}

void MainMap::at(int p_x, int p_y) {
    x = p_x;
    y = p_y;
    set_checked(p_x, p_y, true);
}

Dictionary MainMap::get_data() {
    Dictionary dic;
    dic["width"] = width;
    dic["height"] = height;
    Array arr;
    for (int i = 0; i < width * height; ++i) {
        arr.append(map_checked[i]);
    }
    dic["datas"] = arr;
    return dic;
}

void MainMap::set_data(Dictionary data) {
    width = data["width"];
    height = data["height"];
    Array array = data["datas"];

    if (map_checked) {
        map_checked = (bool*)memrealloc((void*)map_checked, width*height*sizeof(bool));
    }else {
        map_checked = (bool*)memalloc(width*height*sizeof(bool));
    }
    for (int i = 0; i < width * height; ++i) {
        map_checked[i] = array[i];
    }
}

void MainMap::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_width"), &MainMap::get_width);
    ClassDB::bind_method(D_METHOD("set_width", "width"), &MainMap::set_width);

    ClassDB::bind_method(D_METHOD("get_height"), &MainMap::get_height);
    ClassDB::bind_method(D_METHOD("set_height", "height"), &MainMap::set_height);

    ClassDB::bind_method(D_METHOD("get_texture"), &MainMap::get_texture);
    ClassDB::bind_method(D_METHOD("set_texture", "texture"), &MainMap::set_texture);

    ClassDB::bind_method(D_METHOD("get_data"), &MainMap::get_data);
    ClassDB::bind_method(D_METHOD("set_data", "data"), &MainMap::set_data);

    ClassDB::bind_method(D_METHOD("checked_at", "x", "y"), &MainMap::checked_at);
    ClassDB::bind_method(D_METHOD("set_checked", "x", "y", "checked"), &MainMap::set_checked);

    ClassDB::bind_method(D_METHOD("at", "x", "y"), &MainMap::at);
    ClassDB::bind_method(D_METHOD("get_x"), &MainMap::get_x);
    ClassDB::bind_method(D_METHOD("get_y"), &MainMap::get_y);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "width"), "set_width", "get_width");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "height"), "set_height", "get_height");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), "set_texture", "get_texture");

    ADD_SIGNAL(MethodInfo("turn_on", PropertyInfo(Variant::INT, "x"), PropertyInfo(Variant::INT, "y")));
    ADD_SIGNAL(MethodInfo("turn_off", PropertyInfo(Variant::INT, "x"), PropertyInfo(Variant::INT, "y")));
}