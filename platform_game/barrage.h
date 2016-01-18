//
// Created by Gen on 2016/1/17.
//

#ifndef GODOT_NEW_BARRAGE_H
#define GODOT_NEW_BARRAGE_H

#include "../../scene/2d/node_2d.h"
#include "../../scene/resources/texture.h"
#include "../../core/reference.h"
#include "../../core/math/math_2d.h"


class Barrage : public Node2D {
    OBJ_TYPE(Barrage, Node2D);
protected:
    struct Bullet {
        int id;
        int frame;
        Vector2 speed;
        Matrix32 matrix;
        bool live;
        Variant data;
        RID checker;

        _FORCE_INLINE_ Bullet(){id=0;live = true;}
    };
private:
    Ref<Texture> texture;
    Vector<Bullet> bullets;
    DVector<int> dead_cache;
    int h_frames;
    int v_frames;

    bool  shape_created;
    RID shape;

    void _fixed_process_bullets(float delta_time);
    void _process_and_draw_bullets(float delta_time);
    void _body_inout(int p_status,const RID& p_body, int p_instance, int p_body_shape,int p_area_shape);

    void kill(Bullet &bullet, int index);

    int _layer_mask;
    int _collision_mask;

protected:
    void _notification(int p_what);
    static void _bind_methods();

    virtual void _process_bullet(Bullet *bullet, float delta_time) {}

public:
    _FORCE_INLINE_ void set_texture(const Ref<Texture> &p_texture) {texture = p_texture;}
    _FORCE_INLINE_ Ref<Texture> get_texture() {return texture;}

    _FORCE_INLINE_ void set_layer_mask(int p_layer_mask) {_layer_mask=p_layer_mask;}
    _FORCE_INLINE_ int get_layer_mask() {return _layer_mask;}

    _FORCE_INLINE_ void set_collision_mask(int p_collision_mask) {_collision_mask=p_collision_mask;}
    _FORCE_INLINE_ int get_collision_mask() {return _collision_mask;}

    int create_bullet(Point2 p_position, float p_rotation, Vector2 p_speed, int p_frame, const Variant &customer_data);

    Barrage();
};

class ScatterBarrage : public Barrage {
OBJ_TYPE(ScatterBarrage, Barrage);
public:
    void shoot(Point2 target, int count);
};

#endif //GODOT_NEW_BARRAGE_H
