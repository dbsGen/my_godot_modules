//
// Created by Gen on 2016/1/17.
//

#ifndef GODOT_NEW_BARRAGE_H
#define GODOT_NEW_BARRAGE_H

#include "../../scene/2d/node_2d.h"
#include "../../scene/resources/texture.h"
#include "../../scene/resources/packed_scene.h"
#include "../../core/reference.h"
#include "../../core/math/math_2d.h"
#include "hit_status/hit_status.h"
#include "graze_area.h"

class Barrage;
class HitArea;
struct BulletComparator;

class Bullet : public Object {
OBJ_TYPE(Bullet, Object);
    int index;
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
    friend struct BulletComparator;
protected:
    static void _bind_methods();

    void _body_inout(int p_status,const RID& p_body, int p_instance, int p_body_shape,int p_area_shape);
    void _area_inout(int p_status,const RID& p_area, int p_instance, int p_area_shape,int p_self_shape);
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
struct BulletComparator {

    inline bool operator()(const Bullet* a,const Bullet* b) const { return (a->index<b->index); }
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

    static bool  shape_created;
    static RID shape;

    void _fixed_process_bullets(float delta_time);
    void _process_and_draw_bullets(float delta_time);

    void hit(Bullet *bullet, int index, Node* target);
    void hit(Bullet *bullet, int index, HitArea* target);
    void graze(Bullet *bullet, GrazeArea* area);
    void kill(Bullet *bullet, int index);

    int max_index;
    int _layer_mask;
    int _collision_mask;

    NodePath character_path;
    ObjectID character_id;
    void _update_character();
    Ref<HitStatus> hit_status;
    Ref<PackedScene> explosion_scene;

    Vector2 gravity;

    bool kill_out_screen;
    float kill_range;

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

    _FORCE_INLINE_ void set_character_path(const NodePath &p_character_path) {character_path=p_character_path;_update_character();}
    _FORCE_INLINE_ NodePath get_character_path() {return character_path;}
    _FORCE_INLINE_ Character *get_character() {return character_id == 0 ? NULL : ObjectDB::get_instance(character_id)->cast_to<Character>();}

    _FORCE_INLINE_ void set_hit_status(const Ref<HitStatus> p_hit_status) {hit_status = p_hit_status;}
    _FORCE_INLINE_ Ref<HitStatus> get_hit_status() {return hit_status;}

    _FORCE_INLINE_ void set_explosion_scene(Ref<PackedScene> p_scene) {explosion_scene=p_scene;}
    _FORCE_INLINE_ Ref<PackedScene> get_explosion_scene() {return explosion_scene;}

    _FORCE_INLINE_ void set_gravity(Vector2 p_gravity) {gravity = p_gravity;}
    _FORCE_INLINE_ Vector2 get_gravity() {return gravity;}

    _FORCE_INLINE_ void set_kill_out_screen(bool p_is_kill) {kill_out_screen = p_is_kill;}
    _FORCE_INLINE_ bool get_kill_out_screen() {return kill_out_screen;}

    _FORCE_INLINE_ void set_kill_range(float p_range) {kill_range = p_range;}
    _FORCE_INLINE_ float get_kill_range() {return kill_range;}

    virtual void clear();

    Bullet *create_bullet(Point2 p_position, float p_rotation, Vector2 p_speed, int p_frame, const Variant &customer_data=Variant());

    Barrage();
    ~Barrage();
};

class ShootBarrage : public Barrage {
OBJ_TYPE(ShootBarrage, Barrage);
protected:
    float speed;
    float radius;
    float body_size;
    float bullet_scale;

    int shoot_time;
    int frame_interval;
    int shoot_count;
    int frame_count;

    void _notification(int p_what);
    static void _bind_methods();

    virtual void _shoot(){}

public:

    _FORCE_INLINE_ void set_speed(float p_speed) {speed = p_speed;}
    _FORCE_INLINE_ float get_speed() {return speed;}

    _FORCE_INLINE_ void set_radius(float p_radius) {radius=p_radius;}
    _FORCE_INLINE_ float get_radius() {return radius;}

    _FORCE_INLINE_ void set_body_size(float p_body_size) {body_size = p_body_size;}
    _FORCE_INLINE_ float get_body_size() { return body_size;}

    _FORCE_INLINE_ void set_bullet_scale(float p_bullet_scale) {bullet_scale=p_bullet_scale;}
    _FORCE_INLINE_ float get_bullet_scale() { return bullet_scale; }

    _FORCE_INLINE_ void set_shoot_time(int p_time) {shoot_time=p_time;}
    _FORCE_INLINE_ float get_shoot_time() {return shoot_time;}

    _FORCE_INLINE_ void set_frame_interval(int p_interval) {frame_interval=p_interval;}
    _FORCE_INLINE_ int get_frame_interval() {return frame_interval;}

    _FORCE_INLINE_ virtual void clear() {shoot_count=0;Barrage::clear();}

    _FORCE_INLINE_ void start_shoot() {shoot_count=shoot_time;frame_count=0;}

    _FORCE_INLINE_ ShootBarrage() {radius = 0;speed=60;bullet_scale=1;body_size=8;shoot_time=1;frame_interval=2;shoot_count=0;}
};

class ScatterBarrage : public ShootBarrage {
    OBJ_TYPE(ScatterBarrage, ShootBarrage);
private:
    float angle_interval;
    float distance_interval;
    Point2 target;
    int bullet_count;
    int sprite_frame;

protected:
    static void _bind_methods();
    virtual void _shoot();

public:
    _FORCE_INLINE_ void set_interval(float p_interval) {angle_interval=p_interval;}
    _FORCE_INLINE_ float get_interval() {return angle_interval;}

    _FORCE_INLINE_ void set_distance_interval(float p_interval) {distance_interval=p_interval;}
    _FORCE_INLINE_ float get_distance_interval() {return distance_interval;}

    _FORCE_INLINE_ void set_target(Point2 p_target) {target=p_target;}
    _FORCE_INLINE_ Point2 get_target() {return target;}

    _FORCE_INLINE_ void set_bullet_count(int p_bullet_count) {bullet_count=p_bullet_count;}
    _FORCE_INLINE_ int get_bullet_count() {return bullet_count;}

    void shoot(int frame = 0);

    _FORCE_INLINE_ ScatterBarrage() {angle_interval = 5;distance_interval=0;bullet_count = 1;sprite_frame=0;}
};

class RandomBarrage : public ShootBarrage {
    OBJ_TYPE(RandomBarrage, ShootBarrage);
private:
    Vector2 pos_range;
    float angle_range;
    float speed_range;
    int bullet_once_count;
    int sprite_frame;

    float shoot_angle;

    Bullet *make_bullet();
    float randf();
protected:
    static void _bind_methods();
    virtual void _shoot();

public:
    _FORCE_INLINE_ void set_pos_range(const Vector2 &p_range) {pos_range = p_range;}
    _FORCE_INLINE_ Vector2 get_pos_range() {return pos_range;}

    _FORCE_INLINE_ void set_angle_range(float p_range) {angle_range=p_range;}
    _FORCE_INLINE_ float get_angle_range() {return angle_range;}

    _FORCE_INLINE_ void set_speed_range(float p_range) {speed_range=p_range;}
    _FORCE_INLINE_ float get_speed_range() {return speed_range;}

    _FORCE_INLINE_ void set_bullet_once_count(int p_count) {bullet_once_count=p_count;}
    _FORCE_INLINE_ int get_bullet_once_count() {return bullet_once_count;}

    void shoot(float angle, int frame);

    _FORCE_INLINE_ RandomBarrage() {angle_range=0; speed_range=0; bullet_once_count=2;sprite_frame=0;}

};

#endif //GODOT_NEW_BARRAGE_H
