//
// Created by Gen on 15-5-21.
//

#include "hit_status_progress.h"
#include "../character.h"

StringName HitStatusProgress::PROCESS_HIT_NAME;
StringName HitStatusProgress::TYPE_CHANGE_NAME;

#define ChangeType(new_type) \
    if (hit_type != new_type && (bool)call(TYPE_CHANGE_NAME, new_type)) { \
        hit_type = new_type; \
        old_hit_type = hit_type;\
    }
bool HitStatusProgress::_pre_behavior(const Variant& target, Dictionary env) {
    Character *character = Object::cast_to<Character>((Object*)target);
    if (character) {
        if (character->get_hit_status() != NULL) {
            return true;
        }else {
            ChangeType(HitStatus::HS_NO_HIT);
            if (old_status != NULL) {
                old_status = nullptr;
                character->set_hit_status(NULL);
            }
        }
    }
    return false;
}

BehaviorNode::Status HitStatusProgress::_behavior(const Variant &target, Dictionary env) {
    env["move"] = call(PROCESS_HIT_NAME, target, env);
    return BehaviorNode::STATUS_RUNNING;
}

Vector2 HitStatusProgress::process_hit(const Variant &target, Dictionary env) {
    Character *character = Object::cast_to<Character>((Object*)target);
    Ref<HitStatus> hit_status = character->get_hit_status();
    if (hit_status != NULL) {
        bool start = !hit_status->started;
        if (start) {
            character->freeze(hit_status->get_freeze_time());
            switch (hit_status->get_hit_type()) {
                case HitStatus::HS_HIT_STUN:
                    hit_status->set_life_time(hit_status->get_stun_time());
                    break;
                case HitStatus::HS_PURSUIT:
                case HitStatus::HS_LAUNCHER:
                    hit_status->set_life_time(hit_status->get_launcher_time());
                    break;
            }
        }
        hit_status->step(character, env);
        HitStatus::HSType type = hit_status->get_hit_type();
        Vector2 velocity = env["move"];

        if (type <= HitStatus::HS_HIT_STUN) {
            if (hit_type <= HitStatus::HS_HIT_STUN && character->get_on_floor()) {
                if (old_status != hit_status) {
                    velocity = hit_status->get_stun_velocity();
                }else {
                    velocity.x *= 0.8;
                }
                ChangeType(HitStatus::HS_HIT_STUN);
            }else {
                velocity = hit_status->get_velocity();
                if (start) hit_status->set_life_time(hit_status->get_launcher_time());
                hit_status->set_hit_type(HitStatus::HS_LAUNCHER);
                ChangeType(HitStatus::HS_LAUNCHER);
            }
        }else if (type == HitStatus::HS_PURSUIT || type == HitStatus::HS_LAUNCHER) {
            velocity = hit_status->get_velocity();
            if (start) {
                hit_status->set_life_time(hit_status->get_launcher_time());
            }
            if (type == HitStatus::HS_LAUNCHER && velocity.y > 0 && character->get_on_floor()) {
                hit_status->set_hit_type(HitStatus::HS_OTG);
                ChangeType(HitStatus::HS_OTG);
            }else {
                ChangeType(type);
            }
        }else if (type == HitStatus::HS_OTG) {
            if (old_status != hit_status) {
                velocity = hit_status->get_velocity();
            }else {
                velocity.x *= 0.8;
            }
            hit_status->set_velocity(velocity);
            ChangeType(HitStatus::HS_OTG);

        }
        if (old_status != hit_status)
            old_status = hit_status;
        return velocity;
    }else {
        ChangeType(HitStatus::HS_NO_HIT);
        if (old_status != NULL) {
            old_status = nullptr;
            character->set_hit_status(NULL);
        }
        return Vector2();
    }
}
#undef ChangeType

void HitStatusProgress::_bind_methods() {

    ClassDB::bind_method(D_METHOD("get_hit_type"), &HitStatusProgress::get_hit_type);
    ClassDB::bind_method(D_METHOD("process_hit", "target", "env"), &HitStatusProgress::process_hit);

    ClassDB::bind_method(D_METHOD("_on_type_change", "new_type"), &HitStatusProgress::_on_type_change);

    BIND_VMETHOD(MethodInfo(TYPE_CHANGE_NAME, PropertyInfo(Variant::INT, "new_type")));
    BIND_VMETHOD(MethodInfo(PROCESS_HIT_NAME, PropertyInfo(Variant::OBJECT, "target"), PropertyInfo(Variant::DICTIONARY, "env")));
}