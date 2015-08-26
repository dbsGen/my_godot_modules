//
// Created by gen on 15-7-18.
//

#ifndef GODOT_MASTER_SCENE_MAP_H
#define GODOT_MASTER_SCENE_MAP_H

#include "../../../core/resource.h"
#include "../../../scene/resources/texture.h"

class SceneMap : public Resource {
    OBJ_TYPE(SceneMap, Resource);
private:
    int start_x;
    int start_y;
    int width;
    int height;

    Ref<Texture> texture;
protected:
    static void _bind_methods();
public:
    _FORCE_INLINE_ int get_start_x() {return start_x;}
    _FORCE_INLINE_ void set_start_x(int p_start_x) {start_x=p_start_x;}

    _FORCE_INLINE_ int get_start_y() { return  start_y;}
    _FORCE_INLINE_ void set_start_y(int p_start_y) {start_y=p_start_y;}

    _FORCE_INLINE_ int get_width() {return width;}
    _FORCE_INLINE_ void set_width(int p_width) {width=p_width;}

    _FORCE_INLINE_ int get_height() {return height;}
    _FORCE_INLINE_ void set_height(int p_height) {height=p_height;}

    _FORCE_INLINE_ void set_texture(const Ref<Texture>& p_texture) {texture = p_texture;}
    _FORCE_INLINE_ Ref<Texture> get_texture() {return texture;}

    int convert_x(int x);
    int convert_y(int y);

    SceneMap() {
        start_x = 0;
        start_y = 0;
        width = 0;
        height = 0;
    }
};


#endif //GODOT_MASTER_SCENE_MAP_H
