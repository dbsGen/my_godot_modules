//
// Created by gen on 15-5-24.
//

#include "linkerbnode.h"

BehaviorNode::Status LinkerBNode::_step(const Variant& target, Dictionary &env) {
    if (get_behavior_enable() && link_target) {
        int ret = link_target->step(target, env);
        return (BehaviorNode::Status)ret;
    }
    else {
        return STATUS_FAILURE;
    }
}

void LinkerBNode::set_link_path(NodePath path) {
    link_path = path;
    update_link_path();
}

bool LinkerBNode::_pre_behavior(const Variant &target, Dictionary env) {
    if (link_target)
        return link_target->call("pre_behavior", target, env);
    return false;
}

void LinkerBNode::_reset(const Variant &target) {
    link_target->reset(target);
}

void LinkerBNode::update_link_path() {
    if (is_inside_tree() && link_path != NodePath() && has_node(link_path)) {
        link_target = Object::cast_to<BehaviorNode>(get_node(link_path));
    }else {
        link_target = NULL;
    }
}

void LinkerBNode::_notification(int p_notification) {
    switch (p_notification) {
        case NOTIFICATION_ENTER_TREE: {
            update_link_path();
            break;
        }
    }
}

void LinkerBNode::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_link_target"),&LinkerBNode::get_link_target);

    ClassDB::bind_method(D_METHOD("set_link_path","link_path"),&LinkerBNode::set_link_path);
    ClassDB::bind_method(D_METHOD("get_link_path"),&LinkerBNode::get_link_path);

    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "link/path" ), "set_link_path","get_link_path");
}