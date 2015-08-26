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

void HitStatus::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("set_face_me", "face_me"), &HitStatus::set_face_me);
    ObjectTypeDB::bind_method(_MD("get_face_me"), &HitStatus::get_face_me);

    ObjectTypeDB::bind_method(_MD("set_force", "force"), &HitStatus::set_force);
    ObjectTypeDB::bind_method(_MD("get_force"), &HitStatus::get_force);

    ObjectTypeDB::bind_method(_MD("set_power", "power"), &HitStatus::set_power);
    ObjectTypeDB::bind_method(_MD("get_power"), &HitStatus::get_power);

    ObjectTypeDB::bind_method(_MD("set_life_time", "life_time"), &HitStatus::set_life_time);
    ObjectTypeDB::bind_method(_MD("get_life_time"), &HitStatus::get_life_time);

    ObjectTypeDB::bind_method(_MD("set_launcher_time", "launcher_time"), &HitStatus::set_launcher_time);
    ObjectTypeDB::bind_method(_MD("get_launcher_time"), &HitStatus::get_launcher_time);

    ObjectTypeDB::bind_method(_MD("set_stun_time", "stun_time"), &HitStatus::set_stun_time);
    ObjectTypeDB::bind_method(_MD("get_stun_time"), &HitStatus::get_stun_time);

    ObjectTypeDB::bind_method(_MD("set_freeze_time", "freeze_time"), &HitStatus::set_freeze_time);
    ObjectTypeDB::bind_method(_MD("get_freeze_time"), &HitStatus::get_freeze_time);

    ObjectTypeDB::bind_method(_MD("set_self_freeze", "self_freeze"), &HitStatus::set_self_freeze);
    ObjectTypeDB::bind_method(_MD("get_self_freeze"), &HitStatus::get_self_freeze);

    ObjectTypeDB::bind_method(_MD("set_damage", "damage"), &HitStatus::set_damage);
    ObjectTypeDB::bind_method(_MD("get_damage"), &HitStatus::get_damage);

    ObjectTypeDB::bind_method(_MD("set_fall_acc", "fall_acc"), &HitStatus::set_fall_acc);
    ObjectTypeDB::bind_method(_MD("get_fall_acc"), &HitStatus::get_fall_acc);

    ObjectTypeDB::bind_method(_MD("set_velocity", "velocity"), &HitStatus::set_velocity);
    ObjectTypeDB::bind_method(_MD("get_velocity"), &HitStatus::get_velocity);

    ObjectTypeDB::bind_method(_MD("set_stun_velocity", "velocity"), &HitStatus::set_stun_velocity);
    ObjectTypeDB::bind_method(_MD("get_stun_velocity"), &HitStatus::get_stun_velocity);

    ObjectTypeDB::bind_method(_MD("set_hit_type", "hit_type"), &HitStatus::set_hit_type);
    ObjectTypeDB::bind_method(_MD("get_hit_type"), &HitStatus::get_hit_type);

    ObjectTypeDB::bind_method(_MD("step", "character", "env"), &HitStatus::step);

    BIND_VMETHOD( MethodInfo("_start", PropertyInfo(Variant::OBJECT, "target"), PropertyInfo(Variant::DICTIONARY,"env")) );
    BIND_VMETHOD( MethodInfo("_step", PropertyInfo(Variant::OBJECT, "target"), PropertyInfo(Variant::DICTIONARY,"env")) );

    ADD_PROPERTY( PropertyInfo(Variant::REAL, "time/stun"), _SCS("set_stun_time"), _SCS("get_stun_time" ) );
    ADD_PROPERTY( PropertyInfo(Variant::REAL, "time/launcher"), _SCS("set_launcher_time"), _SCS("get_launcher_time" ) );
    ADD_PROPERTY( PropertyInfo(Variant::REAL, "time/freeze"), _SCS("set_freeze_time"), _SCS("get_freeze_time" ) );
    ADD_PROPERTY( PropertyInfo(Variant::REAL, "time/self_freeze"), _SCS("set_self_freeze"), _SCS("get_self_freeze" ) );

    ADD_PROPERTY( PropertyInfo(Variant::BOOL, "hit/face_me"), _SCS("set_face_me"), _SCS("get_face_me" ) );
    ADD_PROPERTY( PropertyInfo(Variant::VECTOR2, "hit/force"), _SCS("set_force"), _SCS("get_force" ) );
    ADD_PROPERTY( PropertyInfo(Variant::REAL, "hit/power"), _SCS("set_power"), _SCS("get_power" ) );
    ADD_PROPERTY( PropertyInfo(Variant::REAL, "hit/damage"), _SCS("set_damage"), _SCS("get_damage" ) );
    ADD_PROPERTY( PropertyInfo(Variant::REAL, "hit/fall_acc"), _SCS("set_fall_acc"), _SCS("get_fall_acc" ) );
    ADD_PROPERTY( PropertyInfo(Variant::INT, "hit/hit_type", PROPERTY_HINT_ENUM, "NoHit,Stun,Pursuit,Launcher,OTG"), _SCS("set_hit_type"), _SCS("get_hit_type" ) );

    BIND_CONSTANT(HS_NO_HIT);
    BIND_CONSTANT(HS_HIT_STUN);
    BIND_CONSTANT(HS_PURSUIT);
    BIND_CONSTANT(HS_LAUNCHER);
    BIND_CONSTANT(HS_OTG);
}