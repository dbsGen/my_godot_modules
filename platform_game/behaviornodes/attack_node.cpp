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
        attack_area = get_node(attack_area_path)->cast_to<AttackArea>();
    }else {
        attack_area = NULL;
    }
}

void AttackNode::_during_behavior(const Variant &target, Dictionary &env) {
    if (attack_area) {
        Character *from = ((Object*)target)->cast_to<Character>();
        if (from) {
            bool hit = attack_area->attack(from);
            if (hit) {
                set_hit(hit);
                if (get_script_instance()) {
                    Variant var_env = Variant(env);
                    const Variant* ptr[2]={&target,&var_env};
                    get_script_instance()->call_multilevel(StringName("_on_attack"),ptr,2);
                }
            }
        }
    }
    Action::_during_behavior(target, env);
}

void AttackNode::_timeout_behavior(const Variant& target, Dictionary& env) {
    Action::_timeout_behavior(target, env);
    if (attack_area) {
        attack_area->set_attack_enable(false);
    }
}

void AttackNode::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("set_attack_area_path", "attack_area"), &AttackNode::set_attack_area_path);
    ObjectTypeDB::bind_method(_MD("get_attack_area_path"), &AttackNode::get_attack_area_path);

    ObjectTypeDB::bind_method(_MD("get_attack_area:AttackArea"), &AttackNode::get_attack_area);

    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "attack/attack_area"), _SCS("set_attack_area_path"),_SCS("get_attack_area_path" ));
    BIND_VMETHOD( MethodInfo("_on_attack", PropertyInfo(Variant::OBJECT,"target"), PropertyInfo(Variant::DICTIONARY,"env")) );
}