//
// Created by gen on 15-5-24.
//

#include "attack_node.h"
#include "../character.h"

void AttackNode::_notification(int p_notification) {
    switch (p_notification) {
        case NOTIFICATION_ENTER_TREE: {
            update_attack_area();
        };
    }
}

void AttackNode::set_attack_area_path(NodePath path) {
    attack_area_path = path;
    update_attack_area();
}

void AttackNode::update_attack_area() {
    if (is_inside_tree() && attack_area_path != String("") && has_node(attack_area_path)) {
        attack_area = Object::cast_to<AttackArea>(get_node(attack_area_path));
    }else {
        attack_area = NULL;
    }
}

void AttackNode::_during_behavior(const Variant &target, Dictionary &env) {
    bool is_hit = false;
    if (attack_area) {
        Character *from = Object::cast_to<Character>((Object*)target);
        if (from) {
            bool hit = attack_area->attack(from);
            if (hit) {
                is_hit = true;
                if (get_script_instance()) {
                    Variant var_env = Variant(env);
                    const Variant* ptr[2]={&target,&var_env};
                    get_script_instance()->call_multilevel(StringName("_on_attack"),ptr,2);
                }
            }
        }
    }
    Action::_during_behavior(target, env);
    if (is_hit) {
        set_hit(is_hit);
    }
}

void AttackNode::_reset(const Variant& target) {
    Action::_reset(target);
    get_attack_area()->reset();
}

void AttackNode::_timeout_behavior(const Variant& target, Dictionary& env) {
    Action::_timeout_behavior(target, env);
    if (attack_area) {
        attack_area->set_attack_enable(false);
    }
}

void AttackNode::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_attack_area_path", "attack_area"), &AttackNode::set_attack_area_path);
    ClassDB::bind_method(D_METHOD("get_attack_area_path"), &AttackNode::get_attack_area_path);

    ClassDB::bind_method(D_METHOD("get_attack_area:AttackArea"), &AttackNode::get_attack_area);

    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "attack/attack_area"), "set_attack_area_path","get_attack_area_path");
    BIND_VMETHOD( MethodInfo("_on_attack", PropertyInfo(Variant::OBJECT,"target"), PropertyInfo(Variant::DICTIONARY,"env")) );
}