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
            Character *cha = ((Object*)(bodies[i]))->cast_to<Character>();
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
    Character *cha = area->get_owner();
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
        Ref<HitStatus> hs = hit_status->duplicate(true)->cast_to<HitStatus>();
        if (face_relative) {
            bool right = cha->get_global_pos().x > from->get_global_pos().x;
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
                Node2D* spark = spark_scene->instance()->cast_to<Node2D>();
                if (spark) {
                    cha->get_parent()->add_child(spark);
                    spark->set_global_pos(cha->get_global_pos() + Vector2(spark_range.x * (Math::randf()*2-1), spark_range.y * (Math::randf()*2-1)));
                    spark->set_scale(Vector2(from->get_face_left()?-1:1, 1));
                }
            }

            _attack_to(hs, cha);
            if (get_script_instance()) {
                Variant v1 = hs;
                Variant v2 = cha->cast_to<Object>();
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
        Ref<HitStatus> hs = hit_status->duplicate(true)->cast_to<HitStatus>();
        if (face_relative) {
            bool right = cha->get_global_pos().x > from->get_global_pos().x;
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
                Node2D* spark = spark_scene->instance()->cast_to<Node2D>();
                if (spark) {
                    cha->get_parent()->add_child(spark);
                    spark->set_global_pos(cha->get_global_pos() + Vector2(spark_range.x * (Math::randf()*2-1), spark_range.y * (Math::randf()*2-1)));
                    spark->set_scale(Vector2(from->get_face_left()?-1:1, 1));
                }
            }

            _attack_to(hs, cha);
            if (get_script_instance()) {
                Variant v1 = hs;
                Variant v2 = cha->cast_to<Object>();
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
                            if (Character *c = p->cast_to<Character>()) {
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

    ObjectTypeDB::bind_method(_MD("set_always_attack", "always_attack"), &AttackArea::set_always_attack);
    ObjectTypeDB::bind_method(_MD("get_always_attack"), &AttackArea::get_always_attack);

    ObjectTypeDB::bind_method(_MD("set_attack_enable", "attack_enable"), &AttackArea::set_attack_enable);
    ObjectTypeDB::bind_method(_MD("get_attack_enable"), &AttackArea::get_attack_enable);

    ObjectTypeDB::bind_method(_MD("set_hit_status", "hit_status"), &AttackArea::set_hit_status);
    ObjectTypeDB::bind_method(_MD("get_hit_status:HitStatus"), &AttackArea::get_hit_status);

    ObjectTypeDB::bind_method(_MD("set_spark_scene", "spark_scene"), &AttackArea::set_spark_scene);
    ObjectTypeDB::bind_method(_MD("get_spark_scene:PackedScene"), &AttackArea::get_spark_scene);

    ObjectTypeDB::bind_method(_MD("set_hit_count", "hit_count"), &AttackArea::set_hit_count);
    ObjectTypeDB::bind_method(_MD("get_hit_count"), &AttackArea::get_hit_count);

    ObjectTypeDB::bind_method(_MD("set_attack_span", "attack_span"), &AttackArea::set_attack_span);
    ObjectTypeDB::bind_method(_MD("get_attack_span"), &AttackArea::get_attack_span);

    ObjectTypeDB::bind_method(_MD("set_force_invert", "force_invert"), &AttackArea::set_force_invert);
    ObjectTypeDB::bind_method(_MD("get_force_invert"), &AttackArea::get_force_invert);

    ObjectTypeDB::bind_method(_MD("set_face_relative", "face_relative"), &AttackArea::set_face_relative);
    ObjectTypeDB::bind_method(_MD("get_face_relative"), &AttackArea::get_face_relative);

    ObjectTypeDB::bind_method(_MD("set_spark_range", "spark_range"), &AttackArea::set_spark_range);
    ObjectTypeDB::bind_method(_MD("get_spark_range"), &AttackArea::get_spark_range);

    ObjectTypeDB::bind_method(_MD("_on_area_enter", "area"), &AttackArea::_on_area_enter);
    ObjectTypeDB::bind_method(_MD("_on_area_exit", "area"), &AttackArea::_on_area_exit);

    ObjectTypeDB::bind_method(_MD("attack", "character:Character"), &AttackArea::bind_attack);

    BIND_VMETHOD(MethodInfo("_attack_to", PropertyInfo(Variant::OBJECT, "hit", PROPERTY_HINT_RESOURCE_TYPE, "HitStatus"), PropertyInfo(Variant::OBJECT, "to") ));

    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "always_attack" ), _SCS("set_always_attack"),_SCS("get_always_attack" ) );

    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "attack/force_invert" ), _SCS("set_force_invert"),_SCS("get_force_invert" ) );

    ADD_PROPERTY( PropertyInfo( Variant::INT, "attack/hit_count" ), _SCS("set_hit_count"),_SCS("get_hit_count" ) );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "attack/attack_span" ), _SCS("set_attack_span"),_SCS("get_attack_span" ) );
    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "attack/attack_enable" ), _SCS("set_attack_enable"),_SCS("get_attack_enable" ) );
    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "attack/face_relative" ), _SCS("set_face_relative"),_SCS("get_face_relative" ) );
    ADD_PROPERTYNZ( PropertyInfo( Variant::OBJECT, "attack/hit_status",PROPERTY_HINT_RESOURCE_TYPE,"HitStatus"), _SCS("set_hit_status"),_SCS("get_hit_status" ) );
    ADD_PROPERTYNZ( PropertyInfo( Variant::OBJECT, "attack/spark_scene",PROPERTY_HINT_RESOURCE_TYPE,"PackedScene"), _SCS("set_spark_scene"),_SCS("get_spark_scene" ) );
    ADD_PROPERTY( PropertyInfo( Variant::VECTOR2, "attack/spark_range"), _SCS("set_spark_range"), _SCS("get_spark_range"));
}