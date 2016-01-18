//
// Created by Gen on 2016/1/17.
//

#ifndef GODOT_NEW_BARRAGE_H
#define GODOT_NEW_BARRAGE_H

#include "../../scene/2d/node_2d.h"
#include "../../scene/resources/texture.h"
#include "../../core/reference.h"
#include "../../core/math/math_2d.h"

class Barrage;

class Bullet : public Object {
OBJ_TYPE(Bullet, Object);
    int id;
    int frame;
    Vector2 speed;
    float rotation;
    float scale;
    Point2 position;
    bool live;
    Variant data;
    RID checker;
    float body_size;
    Barrage *owner;
    friend class Barrage;
protected:
    static void _bind_methods();

    void _body_inout(int p_status,const RID& p_body, int p_instance, int p_body_shape,int p_area_shape);
public:

    _FORCE_INLINE_ void set_frame(const int &p_frame) {frame=p_frame;}
    _FORCE_INLINE_ int get_frame() {return frame;}

    _FORCE_INLINE_ void set_speed(const Vector2 &p_speed) {speed = p_speed;}
    _FORCE_INLINE_ Vector2 get_speed() {return speed;}

    _FORCE_INLINE_ void set_rotation(const float &p_rotation) {rotation = p_rotation;}
    _FORCE_INLINE_ float get_rotation() {return rotation;}

    _FORCE_INLINE_ void set_scale(const float &p_scale) {scale=p_scale;}
    _FORCE_INLINE_ float get_scale() {return scale;}

    _FORCE_INLINE_ void set_position(const Point2 &p_position) {position=p_position;}
    _FORCE_INLINE_ Point2 get_position() {return position;}

    _FORCE_INLINE_ void set_data(const Variant &p_data) {data=p_data;}
    _FORCE_INLINE_ Variant get_data() {return data;}

    _FORCE_INLINE_ void set_body_size(float p_body_size) {body_size=p_body_size;}
    _FORCE_INLINE_ float get_body_size() {return body_size;}

    _FORCE_INLINE_ Bullet(){id=0;live = true;scale=1;body_size=8;}
    _FORCE_INLINE_ ~Bullet(){}
};

class Barrage : public Node2D {
    OBJ_TYPE(Barrage, Node2D);
private:
friend class Bullet;
    Ref<Texture> texture;
    Vector<Bullet*> bullets;
    DVector<int> dead_cache;
    int h_frames;
    int v_frames;

    bool  shape_created;
    RID shape;

    void _fixed_process_bullets(float delta_time);
    void _process_and_draw_bullets(float delta_time);

    void kill(Bullet *bullet, int index);

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

    Bullet *create_bullet(Point2 p_position, float p_rotation, Vector2 p_speed, int p_frame, const Variant &customer_data);

    Barrage();
    ~Barrage();
};

class ScatterBarrage : public Barrage {
OBJ_TYPE(ScatterBarrage, Barrage);
private:
    float interval;
    Vector2 speed;

public:
    _FORCE_INLINE_ void set_interval(float p_interval) {interval=p_interval;}
    _FORCE_INLINE_ float get_interval() {return interval;}

    _FORCE_INLINE_ void set_speed(const Vector2 &p_speed) {speed = p_speed;}
    _FORCE_INLINE_ Vector2 get_speed() {return speed;}

    void shoot(Point2 target, int count);
};

#endif //GODOT_NEW_BARRAGE_H
