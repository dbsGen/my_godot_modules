//
// Created by Gen on 16/1/21.
//

#ifndef GODOT_MAIN_PHANTOM_H
#define GODOT_MAIN_PHANTOM_H

#include "../../core/object.h"
#include "../../scene/2d/node_2d.h"
#include "../../scene/2d/sprite.h"
#include "../../scene/resources/color_ramp.h"
#include "../../scene/resources/texture.h"
#include "../../core/reference.h"
#include "../../core/math/math_2d.h"
#include "queue.hpp"

using namespace MyTools;
class Phantom : public Node2D {
    OBJ_TYPE(Phantom, Node2D);
protected:
    struct Item {
        Vector2 position;
        Ref<Texture> texture;
        Rect2 src_rect;
        Vector2 scale;
        float rotation;
        int life;
        Vector2 offset;
    };
private:
    NodePath target_path;
    Sprite  *target;
    Queue<Item> items;

    void _update_target();

    int life_frame;
    int frame_interval;
    void _update_size();

    Ref<ColorRamp> color_ramp;

    void _update_fixed_frame();
    void _update_and_draw();

    void make_item();

    int frame_count;

    bool phantom_enable;

protected:
    static void _bind_methods();
    void _notification(int p_what);

public:
    _FORCE_INLINE_ void set_target_path(const NodePath &p_path) {target_path=p_path;_update_target();}
    _FORCE_INLINE_ NodePath get_target_path() {return target_path;}
    _FORCE_INLINE_ Sprite *get_target() {return target;}

    _FORCE_INLINE_ void set_color_ramp(const Ref<ColorRamp> &p_color_ramp) {color_ramp=p_color_ramp;}
    _FORCE_INLINE_ Ref<ColorRamp> get_color_ramp() {return color_ramp;}

    _FORCE_INLINE_ void set_life_frame(int p_frame) {life_frame=p_frame;_update_size();}
    _FORCE_INLINE_ int get_life_frame() {return life_frame;}

    _FORCE_INLINE_ void set_frame_interval(int p_frame) {frame_interval=p_frame;_update_size();}
    _FORCE_INLINE_ int get_frame_interval() {return frame_interval;}

    _FORCE_INLINE_ void set_phantom_enable(bool p_enable) {phantom_enable = p_enable;}
    _FORCE_INLINE_ bool get_phantom_enable() {return phantom_enable;}

    _FORCE_INLINE_ Phantom() {
        target = NULL;
        frame_count = 0;
        items.alloc(30);
        frame_interval = 1;
        life_frame = 60;
        phantom_enable = false;
        set_fixed_process(true);
        set_process(true);
    }

};


#endif //GODOT_MAIN_PHANTOM_H
