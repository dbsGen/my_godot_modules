//
// Created by Gen on 15-5-19.
//

#include "character.h"
#include "../../scene/main/node.h"
#include "../../core/math/math_2d.h"
#include <core/bind/core_bind.h>

StringName Character::COMBO_BEGIN_NAME;
StringName Character::COMBO_END_NAME;
StringName Character::COMBO_CHANGE_NAME;

void Character::set_behavior_tree_path(NodePath path) {
    _behavior_tree_path = path;
    update_behavior_node();
}

void Character::set_visibility_path(NodePath path) {
    _visibility_path=path;
    update_visibility_notifier();
}

void Character::_on_enter_screen() {
    set_fixed_process(true);
}

void Character::_on_exit_screen() {
    set_fixed_process(false);
}

void Character::update_visibility_notifier() {
    if (_visibility_notifier) {
        _visibility_notifier->disconnect("enter_screen", this, "_on_enter_screen");
        _visibility_notifier->disconnect("exit_screen", this, "_on_exit_screen");
    }
    if (is_inside_tree() && _visibility_path != NodePath() && has_node(_visibility_path)) {
        _visibility_notifier = Object::cast_to<VisibilityNotifier2D>(get_node(_visibility_path));
        if (_visibility_notifier) {
            _visibility_notifier->connect("enter_screen", this, "_on_enter_screen");
            _visibility_notifier->connect("exit_screen", this, "_on_exit_screen");
            set_fixed_process(_visibility_notifier->is_on_screen());
        }
    }else {
        _visibility_notifier = NULL;
    }
}

void Character::update_behavior_node() {
    if (is_inside_tree() && _behavior_tree_path != NodePath() && has_node(_behavior_tree_path)) {
        behavior_root = Object::cast_to<BehaviorNode>(get_node(_behavior_tree_path));
    }else {
        behavior_root = NULL;
    }
}

void Character::update_sprite() {
    if (!is_inside_tree() || _Engine::get_singleton()->is_editor_hint()) return;
    if (is_inside_tree() && _sprite_path != NodePath() && has_node(_sprite_path)) {
        _cha_sprite = Object::cast_to<Node2D>(get_node(_sprite_path));
        if (_cha_sprite) {
            if (first_set) {
                first_set = false;
                default_face_left=face_left;
                _source_scale=_cha_sprite->get_scale();
            }else {
                if (face_left == default_face_left) {
                    _cha_sprite->set_scale(_source_scale);
                }else {
                    Vector2 v2 = _source_scale;
                    v2.x = -v2.x;
                    _cha_sprite->set_scale(v2);
                }
            }
        }
    }else {
        _cha_sprite = NULL;
    }
}

void Character::update_left_ray() {
    if (is_inside_tree() && _left_ray_path != NodePath() && has_node(_left_ray_path)) {
        _left_ray = Object::cast_to<RayCast2D>(get_node(_left_ray_path));
        _left_ray->set_collision_layer(get_collision_mask());
        _left_ray->add_exception(this);
    }else {
        _left_ray = NULL;
    }
}

void Character::update_right_ray() {
    if (is_inside_tree() && _right_ray_path != NodePath() && has_node(_right_ray_path)) {
        _right_ray = Object::cast_to<RayCast2D>(get_node(_right_ray_path));
        _right_ray->set_collision_layer(get_collision_mask());
        _right_ray->add_exception(this);
    }else {
        _right_ray = NULL;
    }
}

void Character::update_anim_controller() {
    if (is_inside_tree() && _anim_path != NodePath() && has_node(_anim_path)) {
        anim_controller = Object::cast_to<AnimController>(get_node(_anim_path));
    }else {
        anim_controller = NULL;
    }
}

void Character::set_face_left(bool p_face_left) {
    if (!can_turn || face_left == p_face_left) return;
    face_left = p_face_left;
    if (is_inside_tree() &&  !_Engine::get_singleton()->is_editor_hint() && _cha_sprite) {
        if (face_left == default_face_left) {
            _cha_sprite->set_scale(_source_scale);
        }else {
            Vector2 v2 = _source_scale;
            v2.x = -v2.x;
            _cha_sprite->set_scale(v2);
        }
    }
}

void Character::set_health(float p_health) {
    float h = p_health;
    if (h > health) {
        emit_signal("health_recovery", h);
    }else if (h < health){
        emit_signal("health_down", h);
    }
    health = h;
}

void Character::_notification(int p_notification) {
    switch (p_notification) {
        case NOTIFICATION_ENTER_TREE: {
            if (!is_inside_tree() || _Engine::get_singleton()->is_editor_hint())
                break;
            update_behavior_node();
            update_visibility_notifier();
            update_right_ray();
            update_left_ray();
            update_sprite();
            update_anim_controller();
            break;
        }
        case NOTIFICATION_FIXED_PROCESS: {
            if (!is_inside_tree() || _Engine::get_singleton()->is_editor_hint())
                break;
            float fixed_process_time = get_fixed_process_delta_time();
            on_floor = (_left_ray != NULL && _left_ray->is_colliding()) || (_right_ray != NULL && _right_ray->is_colliding());
            if (freeze_time <= 0 && behavior_root != NULL) {
                Dictionary dic = call("behavior_data");
                if (_unlock_face) {
                    _unlock_face = false;
                    can_turn = true;
                }
                behavior_root->step(this, dic);
                if (_stop_moving) {
                    _stop_moving = false;
                    _move_duration = 0;
                    _move = _move_vec = Vector2();
                }else {
                    if (_move_vec == Vector2()) {
                        _move = dic["move"];
                    }else {
                        _move = dic["move"];
                        _move.x = _move_vec.x == 0 ? _move.x : _move_vec.x;
                        _move.y = _move_vec.y == 0 ? _move.y : _move_vec.y;
                        _move_duration -= fixed_process_time;
                        if (_move_duration < 0)
                            _move_vec = Vector2();
                        else {
                            _move_vec *= _move_reduction;
                        }
                    }
                }
                move_and_slide(_move);

                for (int i = 0, t = buffs.size(); i < t; i++) {
                    float left_time = buffs[i]->get_life_time() - fixed_process_time;
                    buffs[i]->set_life_time(left_time);
                    if (left_time <= 0) {
                        buffs.remove(i);
                        i--;
                        t--;
                    }
                }
                if (hit_status != NULL) {
                    float left_time = hit_status->get_life_time() - fixed_process_time;
                    hit_status->set_life_time(left_time);
                    if (left_time <= 0) {
                        set_hit_status(NULL);
                    }
                }else if (guard_point <= 0 && reset_guard_point > 0){
                    guard_point = reset_guard_point;
                    emit_signal("guard_point_change", guard_point);
                }
                _step(dic);
                if (get_script_instance()) {
                    Variant v1 = dic;
                    const Variant* ptr[1]={&v1};
                    get_script_instance()->call_multilevel(StringName("_step"),ptr,1);
                }
            }
            if (freeze_time > 0) {
                freeze_time -= fixed_process_time;
                if (freeze_time <= 0 && anim_controller) anim_controller->resume();
            }
            break;
        }
    }
}

Dictionary Character::behavior_data() {
    Dictionary data;
    Object *object = NULL;
    if (colliding.target_id != 0) {
        object = ObjectDB::get_instance(colliding.target_id);
    }
    if (object) {
        data["colliding_top"] = colliding.top;
        data["colliding_bottom"] = colliding.bottom;
        data["colliding_right"] = colliding.right;
        data["colliding_left"] = colliding.left;
        data["colliding_normal"] = colliding.normal;
        data["colliding_object"] = object;
    }else {
        data["colliding_top"] = false;
        data["colliding_bottom"] = false;
        data["colliding_right"] = false;
        data["colliding_left"] = false;
        data["colliding_normal"] = Vector2();
        data["colliding_object"] = NULL;
    }
    data["timestep"] = get_fixed_process_delta_time();
    data["move"] = _move;
    return data;
}

void Character::add_buff(Ref<Buff> &buff) {
    if (!get_can_buff())
        return;
    if (buff->get_unique() != "") {
        clear_buff_unique(buff->get_unique());
    }
    buffs.push_back(buff);
}

void Character::clear_buff_name(String buff_name) {
    for (int i = 0, t = buffs.size(); i < t; ++i) {
        if (buffs[i]->get_buff_name() == buff_name) {
            buffs.remove(i);
            i--;
            t--;
        }
    }
}

void Character::clear_buff_unique(String buff_unique) {
    ERR_FAIL_COND(buff_unique=="");
    for (int i = 0, t = buffs.size(); i < t; ++i) {
        if (buffs[i]->get_unique() == buff_unique) {
            buffs.remove(i);
            i--;
            t--;
            break;
        }
    }
}

bool Character::attack_by(Ref<HitStatus> p_hit_status, Character *from, bool from_hit_area) {
    if (has_hit_area && !from_hit_area) {
        return false;
    }else {
        bool ret = call("attack_by", p_hit_status, Object::cast_to<Object>(from));
        return ret;
    }
}

void Character::set_hit_status(Ref<HitStatus> hs) {
    if (hit_status != hs) {
        if (hs == NULL) {
            ProjectSettings::get_singleton()->emit_signal(COMBO_END_NAME, this);
            combo_info.damage_reduction = 1;
            combo_info.stun_reduction = 1;
            combo_info.damage = 0;
            combo_info.hit_count = 0;
            combo_info.hit_stack.clear();
        }
    }
    hit_status = hs;
}

bool Character::_attack_by(Ref<HitStatus> p_hit_status, Object *from) {
    if (guard_point>0) {
        float gp = guard_point - p_hit_status->get_power();
        emit_signal("guard_point_change", gp);
        guard_point = gp;
        if (guard_point <= 0) {
            emit_signal("guard_break", p_hit_status);
        }
    }
    bool guard = guard_point > 0;
    float damage = p_hit_status->get_damage() * combo_info.damage_reduction;
    p_hit_status->set_stun_time(p_hit_status->get_stun_time() * combo_info.stun_reduction);
    p_hit_status->set_launcher_time(p_hit_status->get_launcher_time() * combo_info.stun_reduction);
    float nh = health - damage;

    if (guard) {
        nh = health - damage * (1-guard_percent);
        p_hit_status->set_hit_type(HitStatus::HS_GUARD);
        freeze(p_hit_status->get_freeze_time() * (1-guard_percent));
    }else {
        behavior_root->reset(this);
        if (p_hit_status->get_face_me()) {
            if (p_hit_status->get_force().x > 0) {
                set_face_left(true);
            }else if (p_hit_status->get_force().x < 0) {
                set_face_left(false);
            }
        }
        combo_info.damage += damage;
        combo_info.hit_count += 1;
        if (combo_info.hit_stack.size() > 0) {
            if (combo_info.hit_stack[combo_info.hit_stack.size() - 1]->get_original_id() == p_hit_status->get_original_id()) {
                goto combo_end;
            }
            for (int i = 0, t = combo_info.hit_stack.size(); i < t; ++i) {
                if (combo_info.hit_stack[i]->get_hit_id() == p_hit_status->get_hit_id()) {
                    combo_info.damage_reduction *= p_hit_status->get_damage_reduction();
                    if (p_hit_status->get_damage_reduction() < 1) 
                        combo_info.damage_reduction *= p_hit_status->get_damage_reduction();
                    combo_info.stun_reduction *= p_hit_status->get_stun_reduction();
                    if (p_hit_status->get_stun_reduction() < 1)
                        combo_info.stun_reduction *= p_hit_status->get_stun_reduction();
                    combo_info.hit_stack.push_back(p_hit_status);
                    goto combo_end;
                }
            }
        }
        combo_info.damage_reduction *= p_hit_status->get_damage_reduction();
        combo_info.stun_reduction *= p_hit_status->get_stun_reduction();
        combo_info.hit_stack.push_back(p_hit_status);
        
        combo_end:
        if (hit_status != p_hit_status) {
            
            // freeze(p_hit_status->get_freeze_time());
            switch (p_hit_status->get_hit_type()) {
                case HitStatus::HS_HIT_STUN:
                    p_hit_status->set_life_time(p_hit_status->get_stun_time());
                    break;
                case HitStatus::HS_PURSUIT:
                case HitStatus::HS_LAUNCHER:
                    p_hit_status->set_life_time(p_hit_status->get_launcher_time());
                    break;
            }
            
            if (hit_status != NULL) {
                HitStatus::HSType type = p_hit_status->get_hit_type();
                HitStatus::HSType hit_type = hit_status->get_hit_type();
                if (type <= HitStatus::HS_HIT_STUN) {
                    if (hit_type > HitStatus::HS_HIT_STUN || !get_on_floor()) {
                        p_hit_status->set_life_time(p_hit_status->get_launcher_time());
                    }
                }
            }
            if (hit_status == NULL) {
                set_hit_status(p_hit_status);
                ProjectSettings::get_singleton()->emit_signal(COMBO_BEGIN_NAME, this, from);
            }else {
                set_hit_status(p_hit_status);
                ProjectSettings::get_singleton()->emit_signal(COMBO_CHANGE_NAME, this, from);
            }
        }
//        freeze(p_hit_status->get_freeze_time());
    }
    if (nh < health) {
        emit_signal("health_down", nh);
        health = nh;
    }
    return true;
}

Array Character::get_buffs() {
    Array list;
    for (int i = 0, t = buffs.size(); i < t; ++i) {
        list.push_back(buffs[i]);
    }
    return list;
}

void Character::_bind_methods() {

    ProjectSettings::get_singleton()->add_user_signal(MethodInfo(COMBO_BEGIN_NAME, PropertyInfo(Variant::OBJECT, "hit_by"), PropertyInfo(Variant::OBJECT, "hit_to")));
    ProjectSettings::get_singleton()->add_user_signal(MethodInfo(COMBO_CHANGE_NAME, PropertyInfo(Variant::OBJECT, "hit_by"), PropertyInfo(Variant::OBJECT, "hit_to")));
    ProjectSettings::get_singleton()->add_user_signal(MethodInfo(COMBO_END_NAME, PropertyInfo(Variant::OBJECT, "hit_by")));

    ClassDB::bind_method(D_METHOD("set_visibility_path", "visibility_path"), &Character::set_visibility_path);
    ClassDB::bind_method(D_METHOD("get_visibility_path"), &Character::get_visibility_path);
    ClassDB::bind_method(D_METHOD("get_visibility:VisibilityNotifier2D"), &Character::get_visibility);

    ClassDB::bind_method(D_METHOD("set_behavior_tree_path", "tree_path"), &Character::set_behavior_tree_path);
    ClassDB::bind_method(D_METHOD("get_behavior_tree_path"), &Character::get_behavior_tree_path);
    ClassDB::bind_method(D_METHOD("get_behavior_tree:BehaviorNode"), &Character::get_behavior_tree);
    ClassDB::bind_method(D_METHOD("behavior_data:Dictionary"), &Character::behavior_data);

    ClassDB::bind_method(D_METHOD("get_on_floor"), &Character::get_on_floor);

    ClassDB::bind_method(D_METHOD("set_move_vec", "mvoe_vec", "duration"), &Character::set_move_vec, DEFVAL(0));

    ClassDB::bind_method(D_METHOD("set_move_vec_ex", "mvoe_vec", "reduction", "duration"), &Character::set_move_vec_ex, DEFVAL(0));

    ClassDB::bind_method(D_METHOD("set_move", "move"), &Character::set_move);
    ClassDB::bind_method(D_METHOD("get_move"), &Character::get_move);

    ClassDB::bind_method(D_METHOD("stop_moving"), &Character::stop_moving);

    ClassDB::bind_method(D_METHOD("set_anim_path", "anim_path"), &Character::set_anim_path);
    ClassDB::bind_method(D_METHOD("get_anim_path"), &Character::get_anim_path);
    ClassDB::bind_method(D_METHOD("get_anim_controller:AnimController"), &Character::get_anim_controller);

    ClassDB::bind_method(D_METHOD("set_sprite_path", "sprite_path"), &Character::set_sprite_path);
    ClassDB::bind_method(D_METHOD("get_sprite_path"), &Character::get_sprite_path);
    ClassDB::bind_method(D_METHOD("get_sprite:Node2D"), &Character::get_sprite);

    ClassDB::bind_method(D_METHOD("set_left_ray_path", "left_ray_path"), &Character::set_left_ray_path);
    ClassDB::bind_method(D_METHOD("get_left_ray_path"), &Character::get_left_ray_path);
    ClassDB::bind_method(D_METHOD("get_left_ray:RayCast2D"), &Character::get_left_ray);

    ClassDB::bind_method(D_METHOD("set_right_ray_path", "right_ray_path"), &Character::set_right_ray_path);
    ClassDB::bind_method(D_METHOD("get_right_ray_path"), &Character::get_right_ray_path);
    ClassDB::bind_method(D_METHOD("get_right_ray:RayCast2D"), &Character::get_right_ray);

    ClassDB::bind_method(D_METHOD("set_hit_status", "hit_status"), &Character::set_hit_status);
    ClassDB::bind_method(D_METHOD("get_hit_status:HitStatus"), &Character::get_hit_status);

    ClassDB::bind_method(D_METHOD("set_can_buff", "can_buff"), &Character::set_can_buff);
    ClassDB::bind_method(D_METHOD("get_can_buff"), &Character::get_can_buff);

    ClassDB::bind_method(D_METHOD("set_can_turn", "can_turn"), &Character::set_can_turn);
    ClassDB::bind_method(D_METHOD("get_can_turn"), &Character::get_can_turn);
    ClassDB::bind_method(D_METHOD("lock_face"), &Character::lock_face);
    ClassDB::bind_method(D_METHOD("unlock_face"), &Character::unlock_face);

    ClassDB::bind_method(D_METHOD("set_face_left", "face_left"), &Character::set_face_left);
    ClassDB::bind_method(D_METHOD("get_face_left"), &Character::get_face_left);

    ClassDB::bind_method(D_METHOD("freeze", "time"), &Character::freeze);
    ClassDB::bind_method(D_METHOD("get_freeze_time"), &Character::get_freeze_time);

    ClassDB::bind_method(D_METHOD("set_guard_point", "guard_point"), &Character::set_guard_point);
    ClassDB::bind_method(D_METHOD("get_guard_point"), &Character::get_guard_point);

    ClassDB::bind_method(D_METHOD("set_reset_guard_point", "reset_guard_point"), &Character::set_reset_guard_point);
    ClassDB::bind_method(D_METHOD("get_reset_guard_point"), &Character::get_reset_guard_point);

    ClassDB::bind_method(D_METHOD("set_max_guard_point", "max_guard_point"), &Character::set_max_guard_point);
    ClassDB::bind_method(D_METHOD("get_max_guard_point"), &Character::get_max_guard_point);

    ClassDB::bind_method(D_METHOD("set_guard_percent", "guard_percent"), &Character::set_guard_percent);
    ClassDB::bind_method(D_METHOD("get_guard_percent"), &Character::get_guard_percent);

    ClassDB::bind_method(D_METHOD("set_can_attack", "can_attack"), &Character::set_can_attack);
    ClassDB::bind_method(D_METHOD("get_can_attack"), &Character::get_can_attack);

    ClassDB::bind_method(D_METHOD("set_health", "health"), &Character::set_health);
    ClassDB::bind_method(D_METHOD("get_health"), &Character::get_health);

    ClassDB::bind_method(D_METHOD("set_max_health", "max_health"), &Character::set_max_health);
    ClassDB::bind_method(D_METHOD("get_max_health"), &Character::get_max_health);

    ClassDB::bind_method(D_METHOD("attack_by", "hit_status:HitStatus", "from:Character"), &Character::_attack_by);

    ClassDB::bind_method(D_METHOD("_on_enter_screen"), &Character::_on_enter_screen);
    ClassDB::bind_method(D_METHOD("_on_exit_screen"), &Character::_on_exit_screen);
    
    ClassDB::bind_method(D_METHOD("get_combo_damage"), &Character::get_combo_damage);
    ClassDB::bind_method(D_METHOD("get_combo_hit"), &Character::get_combo_hit);

    ClassDB::bind_method(D_METHOD("kill"), &Character::kill);

    ADD_SIGNAL(MethodInfo("health_recovery", PropertyInfo(Variant::REAL, "new_health")));
    ADD_SIGNAL(MethodInfo("health_down", PropertyInfo(Variant::REAL, "new_health")));
    ADD_SIGNAL(MethodInfo("guard_point_change", PropertyInfo(Variant::REAL, "guard_point")));
    ADD_SIGNAL(MethodInfo("guard_break", PropertyInfo(Variant::REAL, "hit_status", PROPERTY_HINT_RESOURCE_TYPE, "HitStatus")));

    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "platform/sprite" ), "set_sprite_path","get_sprite_path");
    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "platform/behavior_tree" ), "set_behavior_tree_path","get_behavior_tree_path");
    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "platform/visibility_path" ), "set_visibility_path","get_visibility_path");
    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "platform/left_ray" ), "set_left_ray_path","get_left_ray_path");
    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "platform/right_ray" ), "set_right_ray_path","get_right_ray_path");
    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "platform/anim_path" ), "set_anim_path","get_anim_path");

    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "character/can_buff" ), "set_can_buff","get_can_buff");
    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "character/can_attack" ), "set_can_attack","get_can_attack");
    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "character/can_turn" ), "set_can_turn","get_can_turn");
    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "character/face_left" ), "set_face_left","get_face_left");
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "guard/point" ), "set_guard_point","get_guard_point");
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "guard/reset" ), "set_reset_guard_point","get_reset_guard_point");
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "guard/max" ), "set_max_guard_point","get_max_guard_point");
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "guard/guard_percent" ), "set_guard_percent","get_guard_percent");

    ADD_PROPERTY( PropertyInfo( Variant::REAL, "status/health" ), "set_health","get_health");
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "status/max_health" ), "set_max_health","get_max_health");

    ADD_PROPERTY( PropertyInfo( Variant::VECTOR2, "move" ), "set_move", "get_move");

    BIND_VMETHOD( MethodInfo("kill") );
    BIND_VMETHOD( MethodInfo("_step", PropertyInfo(Variant::DICTIONARY,"env")) );
    BIND_VMETHOD( MethodInfo("attack_by", PropertyInfo(Variant::OBJECT,"hit_status"), PropertyInfo(Variant::OBJECT,"from")) );
}