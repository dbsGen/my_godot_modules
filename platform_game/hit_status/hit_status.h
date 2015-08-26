//
// Created by gen on 15-5-20.
//

#ifndef GODOT_MASTER_HITSTATUS_H
#define GODOT_MASTER_HITSTATUS_H

#include "../../../core/resource.h"
#include "../../../core/math/math_2d.h"

class Character;
class HitStatusProgress;

class HitStatus : public Resource {
    OBJ_TYPE(HitStatus, Resource);
public:
    enum HSType {
        HS_NO_HIT,
        HS_HIT_STUN,
        HS_PURSUIT,
        HS_LAUNCHER,
        HS_OTG
    };

private:
    bool started;

    float _life_time;
    float power;
    float damage;
    float launcher_time;
    float stun_time;
    float freeze_time;
    float self_freeze;

    Vector2 force;
    Vector2 velocity;
    Vector2 stun_velocity;
    HSType hit_type;
    float fall_acc;
    bool face_me;
    friend class HitStatusProgress;

protected:
    static void _bind_methods();
    virtual void _step(Object *character, Dictionary env);
    virtual void _start(Object *character, Dictionary env) {}

public:
    _FORCE_INLINE_ void set_face_me(bool p_face_me) {face_me = p_face_me;}
    _FORCE_INLINE_ bool get_face_me() {return face_me;}

    _FORCE_INLINE_ void set_damage(float m_damage) {damage = m_damage;}
    _FORCE_INLINE_ float get_damage() {return damage;}

    _FORCE_INLINE_ void set_force(Vector2 v2) {force = v2;}
    _FORCE_INLINE_ Vector2 get_force() {return force;}

    _FORCE_INLINE_ void set_life_time(float life_time){_life_time = life_time;}
    _FORCE_INLINE_ float get_life_time() { return _life_time;}

    _FORCE_INLINE_ void set_power(float p_power){power = p_power;}
    _FORCE_INLINE_ float get_power() {return power;}

    _FORCE_INLINE_ void set_fall_acc(float p_fall_acc) {fall_acc = p_fall_acc;}
    _FORCE_INLINE_ float get_fall_acc() {return fall_acc;}

    _FORCE_INLINE_ void set_velocity(Vector2 m_velocity) { velocity = m_velocity; }
    _FORCE_INLINE_ Vector2 get_velocity() { return velocity; }

    _FORCE_INLINE_ void set_stun_velocity(Vector2 m_stun_velocity) { stun_velocity = m_stun_velocity; }
    _FORCE_INLINE_ Vector2 get_stun_velocity() { return stun_velocity; }

    _FORCE_INLINE_ float get_launcher_time() {return launcher_time;}
    _FORCE_INLINE_ void set_launcher_time(float p_launcher_time) {launcher_time = p_launcher_time;}

    _FORCE_INLINE_ float get_stun_time() {return stun_time;}
    _FORCE_INLINE_ void set_stun_time(float p_stun_time) {stun_time=p_stun_time;}

    _FORCE_INLINE_ float get_freeze_time() {return freeze_time;}
    _FORCE_INLINE_ void set_freeze_time(float p_freeze_time) {freeze_time=p_freeze_time;}

    _FORCE_INLINE_ float get_self_freeze() {return self_freeze;}
    _FORCE_INLINE_ void set_self_freeze(float p_self_freeze) {self_freeze=p_self_freeze;}

    _FORCE_INLINE_ void set_hit_type(HSType m_hit_type) { hit_type = m_hit_type; }
    _FORCE_INLINE_ HSType get_hit_type() { return hit_type; }
    void step(Object *character, Dictionary env);

    HitStatus() {
        _life_time = 2;
        damage = 10;
        force = Vector2(1,1);
        hit_type = HS_HIT_STUN;
        started = false;
        launcher_time = 2;
        stun_time = 0.2;
        fall_acc=0.06;
        freeze_time=0.1;
        self_freeze=0.1;
    }
};

VARIANT_ENUM_CAST(HitStatus::HSType);

#endif //GODOT_MASTER_HITSTATUS_H
