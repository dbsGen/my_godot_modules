//
// Created by Gen on 15-5-19.
//

#ifndef GODOT_MASTER_CHARACTER_H
#define GODOT_MASTER_CHARACTER_H

#include "../../scene/2d/physics_body_2d.h"
#include "../../core/object.h"
#include "../../core/vector.h"
#include "../../core/list.h"
#include "hit_status/hit_status.h"
#include "buff.h"
#include "../behaviornode/behaviornode.h"
#include "../../scene/2d/node_2d.h"
#include "../../scene/2d/visibility_notifier_2d.h"
#include "../../scene/2d/ray_cast_2d.h"]
#include "attack_area.h"


class Character : public KinematicBody2D {
    OBJ_TYPE(Character, KinematicBody2D);
private:
    NodePath        _visibility_path;
    VisibilityNotifier2D    *_visibility_notifier;
    void update_visibility_notifier();
    void _on_enter_screen();
    void _on_exit_screen();

    NodePath        _behavior_tree_path;
    BehaviorNode    *behavior_root;
    Ref<HitStatus>  hit_status;
    Vector<Ref <Buff> >  buffs;

    NodePath        _sprite_path;
    NodePath        _left_ray_path;
    NodePath        _right_ray_path;
    Node2D          *_cha_sprite;
    RayCast2D       *_left_ray;
    RayCast2D       *_right_ray;
    void            update_sprite();
    void            update_left_ray();
    void            update_right_ray();

    void update_behavior_node();
    bool can_buff;
    bool can_attack;
    bool on_floor;

    struct {
        bool left, right, top, bottom;
        Vector2 normal;
    } colliding;
    float   _move_duration;
    Vector2 _move_vec;

    Vector2 _move;
    Vector2 _source_scale;
    bool can_turn;
    bool first_set;
    bool face_left;
    bool default_face_left;

    float reset_guard_point;
    float max_guard_point;
    float guard_point;
    float guard_percent;
    float freeze_time;

    float health;
    float max_health;

protected:
    void _notification(int p_notification);
    static void _bind_methods();
    virtual Dictionary behavior_data();
    virtual bool _attack_by(Ref<HitStatus> p_hit_status, Object *from);
    virtual void _step(Dictionary env) {}
public:
    _FORCE_INLINE_ NodePath get_behavior_tree_path() {return _behavior_tree_path;}
    void set_behavior_tree_path(NodePath path);

    void add_buff(Ref<Buff> &buff);
    void clear_buff_name(String buff_name);
    void clear_buff_unique(String buff_unique);
    Array get_buffs();

    bool attack_by(Ref<HitStatus> p_hit_status, Character *from);

    _FORCE_INLINE_ bool  get_on_floor() { return on_floor; }

    _FORCE_INLINE_ void set_move_vec(Vector2 p_move_vec, float p_move_duration = 0) {_move_vec=p_move_vec;_move_duration=p_move_duration;}

    _FORCE_INLINE_ void set_move(Vector2 p_move) {_move=p_move;}
    _FORCE_INLINE_ Vector2 get_move() {return _move;}

    _FORCE_INLINE_ Node2D *get_sprite() {return _cha_sprite;}
    _FORCE_INLINE_ NodePath get_sprite_path() {return _sprite_path;}
    _FORCE_INLINE_ void set_sprite_path(NodePath p_sprite_path) {_sprite_path = p_sprite_path;update_sprite();}

    _FORCE_INLINE_ NodePath get_left_ray_path() { return _left_ray_path; }
    _FORCE_INLINE_ void set_left_ray_path(NodePath left_ray_path) {_left_ray_path = left_ray_path; update_left_ray();}

    _FORCE_INLINE_ NodePath get_right_ray_path() { return _right_ray_path; }
    _FORCE_INLINE_ void set_right_ray_path(NodePath right_ray_path) {_right_ray_path = right_ray_path; update_right_ray();}

    _FORCE_INLINE_ RayCast2D *get_left_ray() { return _left_ray; }
    _FORCE_INLINE_ RayCast2D *get_right_ray() { return _right_ray; }

    _FORCE_INLINE_ NodePath get_visibility_path() {return _visibility_path;}
    void set_visibility_path(NodePath path);

    _FORCE_INLINE_ void set_hit_status(Ref<HitStatus> hs){hit_status = hs;}
    _FORCE_INLINE_ Ref<HitStatus> get_hit_status() {return hit_status;}

    _FORCE_INLINE_ bool get_can_buff() {return can_buff;}
    _FORCE_INLINE_ void set_can_buff(bool p_can_buff) {can_buff=p_can_buff;}

    _FORCE_INLINE_ bool get_face_left() {return face_left;}
    void set_face_left(bool p_face_left);

    _FORCE_INLINE_ bool get_can_turn() { return can_turn; }
    _FORCE_INLINE_ void set_can_turn(bool m_can_turn) { can_turn = m_can_turn; }

    _FORCE_INLINE_ void freeze(float time) {freeze_time = time;}
    _FORCE_INLINE_ float get_freeze_time() {return freeze_time;}

    _FORCE_INLINE_ float get_guard_point() {return guard_point;}
    _FORCE_INLINE_ void set_guard_point(float p_guard_point) {guard_point=p_guard_point;}

    _FORCE_INLINE_ float get_reset_guard_point() {return reset_guard_point;}
    _FORCE_INLINE_ void set_reset_guard_point(float p_guard_point) {reset_guard_point=p_guard_point;}

    _FORCE_INLINE_ float get_max_guard_point() {return max_guard_point;}
    _FORCE_INLINE_ void set_max_guard_point(float p_guard_point) {max_guard_point=p_guard_point;}

    _FORCE_INLINE_ float get_guard_percent() {return guard_percent;}
    _FORCE_INLINE_ void set_guard_percent(float p_guard_percent) {guard_percent=p_guard_percent;}

    _FORCE_INLINE_ bool get_can_attack() {return can_attack;}
    _FORCE_INLINE_ void set_can_attack(bool p_can_attack) {can_attack=p_can_attack;}

    _FORCE_INLINE_ float get_health() {return health;}
    _FORCE_INLINE_ void set_health(float p_health) {health=health < max_health?health:max_health;}

    _FORCE_INLINE_ float get_max_health() {return max_health;}
    _FORCE_INLINE_ void set_max_health(float p_max_health) {max_health=p_max_health; if (health>max_health) health=max_health;}

    Character() {
        first_set = true;
        _visibility_notifier = NULL;
        behavior_root = NULL;
        _left_ray = NULL;
        _right_ray = NULL;
        _cha_sprite = NULL;
        guard_point = 0;
        freeze_time = 0;
        guard_percent = 0.8;
        health=100;
        max_health=100;
        can_turn = true;
        set_fixed_process(true);
        colliding = {false, false, false, false, Vector2()};
    }
};


#endif //GODOT_MASTER_CHARACTER_H
