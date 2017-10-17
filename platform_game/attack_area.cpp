//
// Created by Gen on 15-5-21.
//

#include "attack_area.h"
#include "character.h"

bool AttackArea::attack(Character *from) {
    if (hit_status == NULL || !from->get_can_attack()) {
        return false;
    }
    Ref<HitStatus> from_hit = from->get_hit_status();
    if (from_hit != NULL && from_hit->get_hit_type() != HitStatus::HS_NO_HIT) {
        return false;
    }
    Array bodies = get_overlapping_bodies();
    if (attack_enable && (bodies.size() > 0 || hit_areas.size() > 0)) {
        bool ret = false;
        Vector2 force = hit_status->get_force();
        Vector2 p_force = Vector2(force.x * (((from->get_face_left() && force_invert)||(!from->get_face_left() && !force_invert)) ? 1:-1), force.y);

        for (int i = 0, t = bodies.size(); i < t; ++i) {
            Character *cha = Object::cast_to<Character>((Object*)(bodies[i]));
            if (cha) {
                if (to_target(cha, from, force, p_force)) {
                    ret = true;
                }
            }else {
            }
        }

        for (int j = 0, t2 = hit_areas.size(); j < t2; ++j) {
            HitArea *area = hit_areas[j];
            if (area) {
                if (to_target(area, from, force, p_force)) {
                    ret = true;
                }
            }
        }
        return ret;
    }
    return false;
}

bool AttackArea::to_target(HitArea *area, Character *from, Vector2 force, Vector2 p_force) {
    Character *cha = area->get_character();
    if (!cha) return false;
    bool ret = false;
    int count = 0;
    float old_time = 0;
    if (count_store.has((int)((long)area))) {
        HitStatusInfo info = count_store[(int)((long)area)];
        count = info.count;
        old_time = info.hit_time;
    }

    if (((hit_count < 0 || count < hit_count) && old_time < time_record - attack_span)) {
        Ref<HitStatus> hs = hit_status->new_hit_status();
        if (face_relative) {
            bool right = cha->get_global_position().x > from->get_global_position().x;
            hs->set_force(Vector2( ((right && force_invert) || (!right && !force_invert) ? -1:1) * force.x, force.y));
            hs->set_velocity(hs->get_force());
            hs->set_stun_velocity(Vector2(hs->get_force().x, 0));
        }else{
            hs->set_force(p_force);
            hs->set_velocity(p_force);
            hs->set_stun_velocity(Vector2(p_force.x, 0));
        }
        if (area->attack_by(hs, from)) {
            if (spark_scene != NULL) {
                Node2D* spark = Object::cast_to<Node2D>(spark_scene->instance());
                if (spark) {
                    cha->get_parent()->add_child(spark);
                    spark->set_global_position(cha->get_global_position() + Vector2(spark_range.x * (Math::randf()*2-1), spark_range.y * (Math::randf()*2-1)));
                    spark->set_scale(Vector2(from->get_face_left()?-1:1, 1));
                }
            }

            _attack_to(hs, cha);
            if (get_script_instance()) {
                Variant v1 = hs;
                Variant v2 = Object::cast_to<Object>(cha);
                const Variant* ptr[2]={&v1, &v2};
                get_script_instance()->call_multilevel("_attack_to", ptr, 2);
            }
            ret = true;
        }
        if (ret) {
            from->freeze(hs->get_self_freeze());
            HitStatusInfo info;
            info.count = count + 1;
            info.hit_time = time_record;
            count_store[(int)((long)area)] = info;
        }
    }
    return ret;
}

bool AttackArea::to_target(Character *cha, Character *from, Vector2 force, Vector2 p_force) {
    bool ret = false;
    int count = 0;
    float old_time = - attack_span;
    if (count_store.has((int)((long)cha))) {
        HitStatusInfo info = count_store[(int)((long)cha)];
        count = info.count;
        old_time = info.hit_time;
    }

    if ((hit_count < 0 || count < hit_count) && old_time < time_record - attack_span) {
        Ref<HitStatus> hs = hit_status->new_hit_status();
        if (face_relative) {
            bool right = cha->get_global_position().x > from->get_global_position().x;
            hs->set_force(Vector2( ((right && force_invert) || (!right && !force_invert) ? -1:1) * force.x, force.y));
            hs->set_velocity(hs->get_force());
            hs->set_stun_velocity(Vector2(hs->get_force().x, 0));
        }else{
            hs->set_force(p_force);
            hs->set_velocity(p_force);
            hs->set_stun_velocity(Vector2(p_force.x, 0));
        }
        if (cha->attack_by(hs, from)) {
            if (spark_scene != NULL) {
                Node2D* spark = Object::cast_to<Node2D>(spark_scene->instance());
                if (spark) {
                    cha->get_parent()->add_child(spark);
                    spark->set_global_position(cha->get_global_position() + Vector2(spark_range.x * (Math::randf()*2-1), spark_range.y * (Math::randf()*2-1)));
                    spark->set_scale(Vector2(from->get_face_left()?-1:1, 1));
                }
            }

            _attack_to(hs, cha);
            if (get_script_instance()) {
                Variant v1 = hs;
                Variant v2 = Object::cast_to<Object>(cha);
                const Variant* ptr[2]={&v1, &v2};
                get_script_instance()->call_multilevel("_attack_to", ptr, 2);
            }
            ret = true;
        }
        if (ret) {
            from->freeze(hs->get_self_freeze());
            HitStatusInfo info;
            info.count = count + 1;
            info.hit_time = time_record;
            count_store[(int)((long)cha)] = info;
        }
    }
    return ret;
}

void AttackArea::reset() {
    count_store.clear();
    time_record = 0;
}

void AttackArea::_notification(int p_notification) {
    switch (p_notification) {
        case NOTIFICATION_ENTER_TREE: {
            hit_areas.clear();
            if (!is_connected("area_enter", this, "_on_area_enter"))
                connect("area_enter", this, "_on_area_enter");
            if (!is_connected("area_exit", this, "_on_area_exit"))
                connect("area_exit", this, "_on_area_exit");
        } break;
        case NOTIFICATION_FIXED_PROCESS: {
            if (attack_enable) {
                time_record += get_fixed_process_delta_time();
                if (always_attack) {
                    if (attack_owner == NULL) {
                        Node *p = get_parent();
                        while (p) {
                            if (Character *c = Object::cast_to<Character>(p)) {
                                attack_owner = c;
                                break;
                            }
                            p = p->get_parent();
                        }
                        ERR_FAIL_COND(!attack_owner);
                    }
                    attack(attack_owner);
                }
            }
            if (will_remove_areas.size() > 0) {
                for (int i = 0, t = will_remove_areas.size(); i < t; ++i) {
                    HitArea *area = will_remove_areas[i];
                    int idx = hit_areas.find(area);
                    if (idx >= 0) {
                        hit_areas.remove(idx);
                    }
                }
                will_remove_areas.clear();
            }
        } break;
    }
}

void AttackArea::_bind_methods() {

    ClassDB::bind_method(D_METHOD("set_always_attack", "always_attack"), &AttackArea::set_always_attack);
    ClassDB::bind_method(D_METHOD("get_always_attack"), &AttackArea::get_always_attack);

    ClassDB::bind_method(D_METHOD("set_attack_enable", "attack_enable"), &AttackArea::set_attack_enable);
    ClassDB::bind_method(D_METHOD("get_attack_enable"), &AttackArea::get_attack_enable);

    ClassDB::bind_method(D_METHOD("set_hit_status", "hit_status"), &AttackArea::set_hit_status);
    ClassDB::bind_method(D_METHOD("get_hit_status"), &AttackArea::get_hit_status);

    ClassDB::bind_method(D_METHOD("set_spark_scene", "spark_scene"), &AttackArea::set_spark_scene);
    ClassDB::bind_method(D_METHOD("get_spark_scene"), &AttackArea::get_spark_scene);

    ClassDB::bind_method(D_METHOD("set_hit_count", "hit_count"), &AttackArea::set_hit_count);
    ClassDB::bind_method(D_METHOD("get_hit_count"), &AttackArea::get_hit_count);

    ClassDB::bind_method(D_METHOD("set_attack_span", "attack_span"), &AttackArea::set_attack_span);
    ClassDB::bind_method(D_METHOD("get_attack_span"), &AttackArea::get_attack_span);

    ClassDB::bind_method(D_METHOD("set_force_invert", "force_invert"), &AttackArea::set_force_invert);
    ClassDB::bind_method(D_METHOD("get_force_invert"), &AttackArea::get_force_invert);

    ClassDB::bind_method(D_METHOD("set_face_relative", "face_relative"), &AttackArea::set_face_relative);
    ClassDB::bind_method(D_METHOD("get_face_relative"), &AttackArea::get_face_relative);

    ClassDB::bind_method(D_METHOD("set_spark_range", "spark_range"), &AttackArea::set_spark_range);
    ClassDB::bind_method(D_METHOD("get_spark_range"), &AttackArea::get_spark_range);

    ClassDB::bind_method(D_METHOD("set_can_graze", "can_graze"), &AttackArea::set_can_graze);
    ClassDB::bind_method(D_METHOD("get_can_graze"), &AttackArea::get_can_graze);

    ClassDB::bind_method(D_METHOD("_on_area_enter", "area"), &AttackArea::_on_area_enter);
    ClassDB::bind_method(D_METHOD("_on_area_exit", "area"), &AttackArea::_on_area_exit);

    ClassDB::bind_method(D_METHOD("attack", "character:Character"), &AttackArea::bind_attack);

    BIND_VMETHOD(MethodInfo("_attack_to", PropertyInfo(Variant::OBJECT, "hit", PROPERTY_HINT_RESOURCE_TYPE, "HitStatus"), PropertyInfo(Variant::OBJECT, "to") ));

    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "always_attack" ), "set_always_attack","get_always_attack");

    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "attack/force_invert" ), "set_force_invert","get_force_invert");

    ADD_PROPERTY( PropertyInfo( Variant::INT, "attack/hit_count" ), "set_hit_count","get_hit_count");
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "attack/attack_span" ), "set_attack_span","get_attack_span");
    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "attack/attack_enable" ), "set_attack_enable","get_attack_enable");
    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "attack/face_relative" ), "set_face_relative","get_face_relative");
    ADD_PROPERTYNZ( PropertyInfo( Variant::OBJECT, "attack/hit_status",PROPERTY_HINT_RESOURCE_TYPE,"HitStatus"), "set_hit_status","get_hit_status");
    ADD_PROPERTYNZ( PropertyInfo( Variant::OBJECT, "attack/spark_scene",PROPERTY_HINT_RESOURCE_TYPE,"PackedScene"), "set_spark_scene","get_spark_scene");
    ADD_PROPERTY( PropertyInfo( Variant::VECTOR2, "attack/spark_range"), "set_spark_range", "get_spark_range");
    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "can_graze"), "set_can_graze", "get_can_graze");
}