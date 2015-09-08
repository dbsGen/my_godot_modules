//
// Created by Gen on 15-5-21.
//

#ifndef GODOT_MASTER_ATTACKAREA_H
#define GODOT_MASTER_ATTACKAREA_H

#include "../../scene/2d/area_2d.h"
#include "../../core/object.h"
#include "../../core/math/math_2d.h"
#include "hit_status/hit_status.h"
#include "../../core/script_language.h"
#include "../../scene/resources/packed_scene.h"

class Character;

struct HitStatusInfo {
    int count;
    float hit_time;
};

class AttackArea : public Area2D {
    OBJ_TYPE(AttackArea, Area2D);
private:
    bool always_attack;
    Character *attack_owner;

    bool attack_enable;
    Ref<HitStatus> hit_status;
    Ref<PackedScene> spark_scene;
    int hit_count;
    float attack_span;
    HashMap< uint32_t, HitStatusInfo > count_store;

    float time_record;
    bool force_invert;
    bool face_relative;
protected:
    static void _bind_methods();
    virtual void _attack_to(Ref<HitStatus> hit, Character* to) {}

    void _notification(int p_notification);
public:

    _FORCE_INLINE_ Ref<HitStatus> get_hit_status() {return hit_status;}
    _FORCE_INLINE_ void set_hit_status(Ref<HitStatus> p_status) {hit_status=p_status;}

    _FORCE_INLINE_ Ref<PackedScene> get_spark_scene() {return spark_scene;}
    _FORCE_INLINE_ void set_spark_scene(Ref<PackedScene> p_scene) {spark_scene = p_scene;}

    void set_attack_enable(bool p_attack_enable) {
        if (attack_enable != p_attack_enable) {
            attack_enable=p_attack_enable;
            if(p_attack_enable) reset();
        }
    }
    _FORCE_INLINE_ bool get_attack_enable() {return attack_enable;}

    _FORCE_INLINE_ void set_hit_count(int m_hit_count) {hit_count = m_hit_count;}
    _FORCE_INLINE_ int get_hit_count() {return hit_count;}

    _FORCE_INLINE_ void set_always_attack(bool p_always_attack) {always_attack = p_always_attack;}
    _FORCE_INLINE_ bool get_always_attack() {return always_attack;}

    _FORCE_INLINE_ void set_attack_span(float m_attack_span) {attack_span = m_attack_span;}
    _FORCE_INLINE_ float get_attack_span() {return attack_span;}

    _FORCE_INLINE_ void set_force_invert(bool m_force_invert) {force_invert = m_force_invert;}
    _FORCE_INLINE_ bool get_force_invert() {return force_invert;}

    _FORCE_INLINE_ void set_face_relative(bool p_face_relative) {face_relative = p_face_relative;}
    _FORCE_INLINE_ bool get_face_relative(){return face_relative;}

    void reset();

    bool attack(Character *from);

    AttackArea() {
        set_fixed_process(true);
        force_invert = false;
        hit_count=1;
        attack_span=0.3;
        attack_owner = NULL;
        always_attack = false;
    }
};


#endif //GODOT_MASTER_ATTACKAREA_H
