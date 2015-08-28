//
// Created by Gen on 15-5-19.
//

#include "character.h"
#include "../../scene/main/node.h"
#include "../../core/math/math_2d.h"

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
        _visibility_notifier = get_node(_visibility_path)->cast_to<VisibilityNotifier2D>();
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
        behavior_root = get_node(_behavior_tree_path)->cast_to<BehaviorNode>();
    }else {
        behavior_root = NULL;
    }
}

void Character::update_sprite() {
    if (!is_inside_tree() || get_tree()->is_editor_hint()) return;
    if (is_inside_tree() && _sprite_path != NodePath() && has_node(_sprite_path)) {
        _cha_sprite = get_node(_sprite_path)->cast_to<Node2D>();
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
        _left_ray = get_node(_left_ray_path)->cast_to<RayCast2D>();
        _left_ray->set_layer_mask(get_collision_mask());
        _left_ray->add_exception(this);
    }else {
        _left_ray = NULL;
    }
}

void Character::update_right_ray() {
    if (is_inside_tree() && _right_ray_path != NodePath() && has_node(_right_ray_path)) {
        _right_ray = get_node(_right_ray_path)->cast_to<RayCast2D>();
        _right_ray->set_layer_mask(get_collision_mask());
        _right_ray->add_exception(this);
    }else {
        _right_ray = NULL;
    }
}

void Character::set_face_left(bool p_face_left) {
    if (!can_turn || face_left == p_face_left) return;
    face_left = p_face_left;
    if (is_inside_tree() &&  !get_tree()->is_editor_hint() && _cha_sprite) {
        if (face_left == default_face_left) {
            _cha_sprite->set_scale(_source_scale);
        }else {
            Vector2 v2 = _source_scale;
            v2.x = -v2.x;
            _cha_sprite->set_scale(v2);
        }
    }
}

void Character::_notification(int p_notification) {
    switch (p_notification) {
        case NOTIFICATION_ENTER_TREE: {
            if (!is_inside_tree() || get_tree()->is_editor_hint())
                break;
            update_behavior_node();
            update_visibility_notifier();
            update_right_ray();
            update_left_ray();
            update_sprite();
            break;
        }
        case NOTIFICATION_FIXED_PROCESS: {
            if (!is_inside_tree() || get_tree()->is_editor_hint())
                break;
            float fixed_process_time = get_fixed_process_delta_time();
            on_floor = (_left_ray != NULL && _left_ray->is_colliding()) || (_right_ray != NULL && _right_ray->is_colliding());
            if (freeze_time <= 0 && behavior_root != NULL) {
                Dictionary dic = call("behavior_data");
                behavior_root->step(this, dic);
                if (_move_vec == Vector2()) {
                    _move = dic["move"];
                }else {
                    _move = dic["move"];
                    _move.x = _move_vec.x == 0 ? _move.x : _move_vec.x;
                    _move.y = _move_vec.y == 0 ? _move.y : _move_vec.y;
                    _move_duration -= fixed_process_time;
                    if (_move_duration < 0)
                        _move_vec = Vector2();
                }
                Vector2 inv = move(_move);

                Vector2 floor_velocity;
                colliding.left = false;
                colliding.right = false;
                colliding.top = false;
                colliding.bottom = false;
                colliding.normal = Vector2();
                if (is_colliding()) {
                    Vector2 n = get_collision_normal();
                    colliding.normal = n;
                    if ( Math::rad2deg(Math::acos(n.dot( Vector2(0,-1)))) < 40 ) {
                        floor_velocity=get_collider_velocity();
                    }

                    float alpha = get_collision_normal().dot(Vector2(0,1));
                    if (alpha > 0.6) {
                        colliding.top = true;
                    }else if (alpha < -0.6) {
                        colliding.bottom = true;
                    }
                    alpha = get_collision_normal().dot(Vector2(1,0));
                    if (alpha > 0.6) {
                        colliding.left = true;
                    }else if (alpha < -0.6) {
                        colliding.right = true;
                    }

                    _move = n.slide(_move);
                    move(n.slide(inv));
                }
                if (floor_velocity != Vector2()) {
                    move(floor_velocity);
                }
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
                        hit_status = Ref<HitStatus>(NULL);
                    }
                }else if (guard_point < 0){
                    guard_point = reset_guard_point;
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
            }
            break;
        }
    }
}

Dictionary Character::behavior_data() {
    Dictionary data(true);
    data["colliding_top"] = colliding.top;
    data["colliding_bottom"] = colliding.bottom;
    data["colliding_right"] = colliding.right;
    data["colliding_left"] = colliding.left;
    data["colliding_normal"] = colliding.normal;
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

bool Character::attack_by(Ref<HitStatus> p_hit_status, Character *from) {
    bool ret = call("attack_by", p_hit_status, from->cast_to<Object>());
    if (ret) {
        hit_status = p_hit_status;
    }
    return ret;
}

bool Character::_attack_by(Ref<HitStatus> p_hit_status, Object *from) {
    guard_point -= p_hit_status->get_power();
    bool res = guard_point < 0;
    float nh = health - p_hit_status->get_damage();

    if (res) {
        set_hit_status(p_hit_status);
        behavior_root->reset(this);
    }else {
        nh = health - p_hit_status->get_damage() * (1-guard_percent);
    }
    if (p_hit_status->get_face_me()) {
        if (p_hit_status->get_force().x > 0) {
            set_face_left(true);
        }else if (p_hit_status->get_force().x < 0) {
            set_face_left(false);
        }
    }
    if (nh < health) {
        emit_signal("health_down", nh);
        health = nh;
    }
    return res;
}

Array Character::get_buffs() {
    Array list;
    for (int i = 0, t = buffs.size(); i < t; ++i) {
        list.push_back(buffs[i]);
    }
    return list;
}

void Character::_bind_methods() {

    ObjectTypeDB::bind_method(_MD("set_visibility_path", "visibility_path"), &Character::set_visibility_path);
    ObjectTypeDB::bind_method(_MD("get_visibility_path"), &Character::get_visibility_path);

    ObjectTypeDB::bind_method(_MD("set_behavior_tree_path", "tree_path"), &Character::set_behavior_tree_path);
    ObjectTypeDB::bind_method(_MD("get_behavior_tree_path"), &Character::get_behavior_tree_path);
    ObjectTypeDB::bind_method(_MD("behavior_data"), &Character::behavior_data);

    ObjectTypeDB::bind_method(_MD("get_on_floor"), &Character::get_on_floor);

    ObjectTypeDB::bind_method(_MD("set_move_vec", "mvoe_vec", "duration"), &Character::set_move_vec, DEFVAL(0));

    ObjectTypeDB::bind_method(_MD("set_move", "move"), &Character::set_move);
    ObjectTypeDB::bind_method(_MD("get_move"), &Character::get_move);

    ObjectTypeDB::bind_method(_MD("set_sprite_path", "sprite_path"), &Character::set_sprite_path);
    ObjectTypeDB::bind_method(_MD("get_sprite_path"), &Character::get_sprite_path);
    ObjectTypeDB::bind_method(_MD("get_sprite:Node2D"), &Character::get_sprite);

    ObjectTypeDB::bind_method(_MD("set_left_ray_path", "left_ray_path"), &Character::set_left_ray_path);
    ObjectTypeDB::bind_method(_MD("get_left_ray_path"), &Character::get_left_ray_path);
    ObjectTypeDB::bind_method(_MD("get_left_ray:RayCast2D"), &Character::get_left_ray);

    ObjectTypeDB::bind_method(_MD("set_right_ray_path", "right_ray_path"), &Character::set_right_ray_path);
    ObjectTypeDB::bind_method(_MD("get_right_ray_path"), &Character::get_right_ray_path);
    ObjectTypeDB::bind_method(_MD("get_right_ray:RayCast2D"), &Character::get_right_ray);

    ObjectTypeDB::bind_method(_MD("set_hit_status", "hit_status"), &Character::set_hit_status);
    ObjectTypeDB::bind_method(_MD("get_hit_status"), &Character::get_hit_status);

    ObjectTypeDB::bind_method(_MD("set_can_buff", "can_buff"), &Character::set_can_buff);
    ObjectTypeDB::bind_method(_MD("get_can_buff"), &Character::get_can_buff);

    ObjectTypeDB::bind_method(_MD("set_can_turn", "can_turn"), &Character::set_can_turn);
    ObjectTypeDB::bind_method(_MD("get_can_turn"), &Character::get_can_turn);

    ObjectTypeDB::bind_method(_MD("set_face_left", "face_left"), &Character::set_face_left);
    ObjectTypeDB::bind_method(_MD("get_face_left"), &Character::get_face_left);

    ObjectTypeDB::bind_method(_MD("freeze", "time"), &Character::freeze);
    ObjectTypeDB::bind_method(_MD("get_freeze_time"), &Character::get_freeze_time);

    ObjectTypeDB::bind_method(_MD("set_guard_point", "guard_point"), &Character::set_guard_point);
    ObjectTypeDB::bind_method(_MD("get_guard_point"), &Character::get_guard_point);

    ObjectTypeDB::bind_method(_MD("set_reset_guard_point", "reset_guard_point"), &Character::set_reset_guard_point);
    ObjectTypeDB::bind_method(_MD("get_reset_guard_point"), &Character::get_reset_guard_point);

    ObjectTypeDB::bind_method(_MD("set_max_guard_point", "max_guard_point"), &Character::set_max_guard_point);
    ObjectTypeDB::bind_method(_MD("get_max_guard_point"), &Character::get_max_guard_point);

    ObjectTypeDB::bind_method(_MD("set_guard_percent", "guard_percent"), &Character::set_guard_percent);
    ObjectTypeDB::bind_method(_MD("get_guard_percent"), &Character::get_guard_percent);

    ObjectTypeDB::bind_method(_MD("set_can_attack", "can_attack"), &Character::set_can_attack);
    ObjectTypeDB::bind_method(_MD("get_can_attack"), &Character::get_can_attack);

    ObjectTypeDB::bind_method(_MD("set_health", "health"), &Character::set_health);
    ObjectTypeDB::bind_method(_MD("get_health"), &Character::get_health);

    ObjectTypeDB::bind_method(_MD("set_max_health", "max_health"), &Character::set_max_health);
    ObjectTypeDB::bind_method(_MD("get_max_health"), &Character::get_max_health);

    ObjectTypeDB::bind_method(_MD("attack_by", "hit_status:HitStatus", "from:Character"), &Character::_attack_by);

    ObjectTypeDB::bind_method(_MD("_on_enter_screen"), &Character::_on_enter_screen);
    ObjectTypeDB::bind_method(_MD("_on_exit_screen"), &Character::_on_exit_screen);

    ADD_SIGNAL(MethodInfo("health_recovery", PropertyInfo(Variant::REAL, "new_health")));
    ADD_SIGNAL(MethodInfo("health_down", PropertyInfo(Variant::REAL, "new_health")));

    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "platform/sprite" ), _SCS("set_sprite_path"),_SCS("get_sprite_path") );
    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "platform/behavior_tree" ), _SCS("set_behavior_tree_path"),_SCS("get_behavior_tree_path") );
    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "platform/visibility_path" ), _SCS("set_visibility_path"),_SCS("get_visibility_path") );
    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "platform/left_ray" ), _SCS("set_left_ray_path"),_SCS("get_left_ray_path") );
    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "platform/right_ray" ), _SCS("set_right_ray_path"),_SCS("get_right_ray_path") );

    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "character/can_buff" ), _SCS("set_can_buff"),_SCS("get_can_buff" ) );
    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "character/can_attack" ), _SCS("set_can_attack"),_SCS("get_can_attack" ) );
    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "character/can_turn" ), _SCS("set_can_turn"),_SCS("get_can_turn" ) );
    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "character/face_left" ), _SCS("set_face_left"),_SCS("get_face_left" ) );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "guard/point" ), _SCS("set_guard_point"),_SCS("get_guard_point" ) );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "guard/reset" ), _SCS("set_reset_guard_point"),_SCS("get_reset_guard_point" ) );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "guard/max" ), _SCS("set_max_guard_point"),_SCS("get_max_guard_point" ) );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "guard/guard_percent" ), _SCS("set_guard_percent"),_SCS("get_guard_percent" ) );

    ADD_PROPERTY( PropertyInfo( Variant::REAL, "status/health" ), _SCS("set_health"),_SCS("get_health" ) );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "status/max_health" ), _SCS("set_max_health"),_SCS("get_max_health" ) );

    ADD_PROPERTY( PropertyInfo( Variant::VECTOR2, "move" ), _SCS("set_move"), _SCS("get_move") );

    BIND_VMETHOD( MethodInfo("_step", PropertyInfo(Variant::DICTIONARY,"env")) );
    BIND_VMETHOD( MethodInfo("attack_by", PropertyInfo(Variant::OBJECT,"hit_status"), PropertyInfo(Variant::OBJECT,"from")) );
}