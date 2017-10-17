//
// Created by gen on 15-5-20.
//

#include "hit_status.h"
#include "../../../core/script_language.h"

void HitStatus::_step(Object *character, Dictionary env) {
    velocity.y += fall_acc;
}

void HitStatus::step(Object *character, Dictionary env) {
    if (!started) {
        started = true;
        _start(character, env);
        if (get_script_instance()) {
            Variant v1 = Variant(character);
            Variant var_env = Variant(env);
            const Variant* ptr[2]={&v1,&var_env};
            get_script_instance()->call_multilevel(StringName("_start"),ptr,2);
        }
    }
    _step(character, env);
    if (get_script_instance()) {
        Variant v1 = Variant(character);
        Variant var_env = Variant(env);
        const Variant* ptr[2]={&v1,&var_env};
        get_script_instance()->call_multilevel(StringName("_step"),ptr,2);
    }
}

Ref<HitStatus> HitStatus::new_hit_status() {
    Ref<HitStatus> n_hs = duplicate(true);
    n_hs->original_id = get_original_id();
    return n_hs;
}

void HitStatus::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_face_me", "face_me"), &HitStatus::set_face_me);
    ClassDB::bind_method(D_METHOD("get_face_me"), &HitStatus::get_face_me);

    ClassDB::bind_method(D_METHOD("set_force", "force"), &HitStatus::set_force);
    ClassDB::bind_method(D_METHOD("get_force"), &HitStatus::get_force);

    ClassDB::bind_method(D_METHOD("set_power", "power"), &HitStatus::set_power);
    ClassDB::bind_method(D_METHOD("get_power"), &HitStatus::get_power);

    ClassDB::bind_method(D_METHOD("set_life_time", "life_time"), &HitStatus::set_life_time);
    ClassDB::bind_method(D_METHOD("get_life_time"), &HitStatus::get_life_time);

    ClassDB::bind_method(D_METHOD("set_launcher_time", "launcher_time"), &HitStatus::set_launcher_time);
    ClassDB::bind_method(D_METHOD("get_launcher_time"), &HitStatus::get_launcher_time);

    ClassDB::bind_method(D_METHOD("set_stun_time", "stun_time"), &HitStatus::set_stun_time);
    ClassDB::bind_method(D_METHOD("get_stun_time"), &HitStatus::get_stun_time);

    ClassDB::bind_method(D_METHOD("set_freeze_time", "freeze_time"), &HitStatus::set_freeze_time);
    ClassDB::bind_method(D_METHOD("get_freeze_time"), &HitStatus::get_freeze_time);

    ClassDB::bind_method(D_METHOD("set_self_freeze", "self_freeze"), &HitStatus::set_self_freeze);
    ClassDB::bind_method(D_METHOD("get_self_freeze"), &HitStatus::get_self_freeze);

    ClassDB::bind_method(D_METHOD("set_damage", "damage"), &HitStatus::set_damage);
    ClassDB::bind_method(D_METHOD("get_damage"), &HitStatus::get_damage);

    ClassDB::bind_method(D_METHOD("set_fall_acc", "fall_acc"), &HitStatus::set_fall_acc);
    ClassDB::bind_method(D_METHOD("get_fall_acc"), &HitStatus::get_fall_acc);

    ClassDB::bind_method(D_METHOD("set_velocity", "velocity"), &HitStatus::set_velocity);
    ClassDB::bind_method(D_METHOD("get_velocity"), &HitStatus::get_velocity);

    ClassDB::bind_method(D_METHOD("set_stun_velocity", "velocity"), &HitStatus::set_stun_velocity);
    ClassDB::bind_method(D_METHOD("get_stun_velocity"), &HitStatus::get_stun_velocity);

    ClassDB::bind_method(D_METHOD("set_hit_type", "hit_type"), &HitStatus::set_hit_type);
    ClassDB::bind_method(D_METHOD("get_hit_type"), &HitStatus::get_hit_type);
    
    ClassDB::bind_method(D_METHOD("set_damage_reduction", "damage_reduction"), &HitStatus::set_damage_reduction);
    ClassDB::bind_method(D_METHOD("get_damage_reduction"), &HitStatus::get_damage_reduction);

    ClassDB::bind_method(D_METHOD("set_stun_reduction", "stun_reduction"), &HitStatus::set_stun_reduction);
    ClassDB::bind_method(D_METHOD("get_stun_reduction"), &HitStatus::get_stun_reduction);
    
    ClassDB::bind_method(D_METHOD("set_hit_id", "hit_id"), &HitStatus::set_hit_id);
    ClassDB::bind_method(D_METHOD("get_hit_id"), &HitStatus::get_hit_id);
    
    ClassDB::bind_method(D_METHOD("new_hit_status"), &HitStatus::new_hit_status);
    ClassDB::bind_method(D_METHOD("get_original_id"), &HitStatus::get_original_id);

    ClassDB::bind_method(D_METHOD("step", "character", "env"), &HitStatus::step);

    BIND_VMETHOD( MethodInfo("_start", PropertyInfo(Variant::OBJECT, "target"), PropertyInfo(Variant::DICTIONARY,"env")) );
    BIND_VMETHOD( MethodInfo("_step", PropertyInfo(Variant::OBJECT, "target"), PropertyInfo(Variant::DICTIONARY,"env")) );

    ADD_PROPERTY( PropertyInfo(Variant::REAL, "time/stun"), "set_stun_time", "get_stun_time");
    ADD_PROPERTY( PropertyInfo(Variant::REAL, "time/launcher"), "set_launcher_time", "get_launcher_time");
    ADD_PROPERTY( PropertyInfo(Variant::REAL, "time/freeze"), "set_freeze_time", "get_freeze_time");
    ADD_PROPERTY( PropertyInfo(Variant::REAL, "time/self_freeze"), "set_self_freeze", "get_self_freeze");

    ADD_PROPERTY( PropertyInfo(Variant::BOOL, "hit/face_me"), "set_face_me", "get_face_me");
    ADD_PROPERTY( PropertyInfo(Variant::VECTOR2, "hit/force"), "set_force", "get_force");
    ADD_PROPERTY( PropertyInfo(Variant::REAL, "hit/power"), "set_power", "get_power");
    ADD_PROPERTY( PropertyInfo(Variant::REAL, "hit/damage"), "set_damage", "get_damage");
    ADD_PROPERTY( PropertyInfo(Variant::REAL, "hit/fall_acc"), "set_fall_acc", "get_fall_acc");
    ADD_PROPERTY( PropertyInfo(Variant::INT, "hit/hit_type", PROPERTY_HINT_ENUM, "NoHit,Stun,Pursuit,Launcher,OTG"), "set_hit_type", "get_hit_type");
    ADD_PROPERTY( PropertyInfo(Variant::REAL, "hit/damage_reduction"), "set_damage_reduction", "get_damage_reduction");
    ADD_PROPERTY( PropertyInfo(Variant::REAL, "hit/stun_reduction"), "set_stun_reduction", "get_stun_reduction");
    ADD_PROPERTY( PropertyInfo(Variant::STRING, "hit/id"), "set_hit_id", "get_hit_id");

    BIND_CONSTANT(HS_NO_HIT);
    BIND_CONSTANT(HS_HIT_STUN);
    BIND_CONSTANT(HS_PURSUIT);
    BIND_CONSTANT(HS_LAUNCHER);
    BIND_CONSTANT(HS_OTG);
}