//
// Created by gen on 15-5-24.
//

#include "action.h"
#include "../../behaviornode/linkerbnode.h"
#include "../../../scene/animation/animation_player.h"
#include "../anim_controller.h"
#include "../../../core/math/math_defs.h"
#include "../../../core/math/math_2d.h"

void Action::refresh_cancel_list() {
    cancel_list.clear();
    for (int i = 0, t = get_child_count(); i < t; ++i) {
        CancelNode* cancelNode = get_child(i)->cast_to<CancelNode>();
        if (cancelNode && cancelNode->get_link_target()) {
            if (cancelNode->get_link_target()->cast_to<Action>())
                cancel_nodes.push_back(cancelNode);
        }else {
            LinkerBNode *linkerBNode = get_child(i)->cast_to<LinkerBNode>();
            if (linkerBNode && linkerBNode->get_link_target()) {
                Action *action = linkerBNode->get_link_target()->cast_to<Action>();
                if (action) {
                    cancel_list.push_back(action);
                }
            }
        }
    }
    checked_cancel_list = true;
}

BehaviorNode::Status Action::_traversal_children(const Variant& target, Dictionary &env) {
    return STATUS_RUNNING;
}

void Action::_notification(int p_notification) {
    switch (p_notification) {
        case NOTIFICATION_ENTER_TREE: {
            update_animation_path();
            update_next_action();
            break;
        }
    }
}

BehaviorNode::Status Action::_step(const Variant& target, Dictionary &env) {
    if (_time <= 0 && timeout && force_enter) {
        force_enter = false;
        Status childrenStatus =  _traversal_children(target, env);
        Status status = (Status)((int)call(StringName("behavior"),target, Variant(env)));
        if (status == STATUS_DEPEND_ON_CHILDREN)
            return childrenStatus;
        else
            return status;
    }else {
        return TimerBNode::_step(target, env);
    }
}

void Action::_during_behavior(const Variant &target, Dictionary &env) {
    Vector2 v2 = env["move"];
    float abs_x = Math::abs(v2.x);
    if (abs_x > max_move) {
        if (v2.x > 0) {
            if (old_move > max_move) {
                v2.x = (old_move-drag) > max_move ? (old_move-drag):max_move;
            }else {
                v2.x = max_move;
            }
        }else {
            if (old_move < -max_move) {
                v2.x = (old_move+drag) < -max_move ? (old_move+drag):-max_move;
            }else {
                v2.x = -max_move;
            }
        }
    }
    env["move"] = v2;
    old_move = v2.x;

    if (!checked_cancel_list) {
        refresh_cancel_list();
    }
#define setforce(NODE)  NODE->set_focus();\
    reset_from_cancel = true;\
    reset(target);\
    reset_from_cancel = false;\
    return;

    if (cancel_list.size() != 0) {
        bool check_cancel = false;
        switch (cancel_type) {
            case CANCEL_TYPE_TIME: {
                if (get_delay()-get_time() > cancel_time) check_cancel = true;
                break;
            };
            case CANCEL_TYPE_HIT: {
                if (_is_hit) check_cancel = true;
                break;
            };
            default:
                break;
        }
        if (check_cancel) {
            for (int i = 0, t = cancel_list.size(); i < t; ++i) {
                Action* action = cancel_list[i];
                if ((bool)action->call("pre_behavior", target, env)) {
                    setforce(action);
                }
            }
        }
    }
    if (cancel_nodes.size() != 0){
        for (int i = 0, t = cancel_nodes.size(); i < t; ++i) {
            CancelNode *node = cancel_nodes[i];
            switch (node->get_cancel_type()) {
                case CancelNode::HIT: {
                    if (_is_hit && node->get_link_target()->call("pre_behavior", target, env)) {
                        setforce(node->get_link_target());
                    }
                } break;
                case CancelNode::TIME: {
                    if (get_delay()-get_time() > node->get_cancel_time() && node->get_link_target()->call("pre_behavior", target, env)) {
                        setforce(node->get_link_target());
                    }
                } break;
            }
        }
    }
#undef setforce
}

void Action::set_animation_path(NodePath path) {
    animation_path = path;
    update_animation_path();
}

void Action::update_animation_path() {
    if (is_inside_tree() && animation_path != NodePath() && has_node(animation_path)) {
        animation_node = get_node(animation_path);
    }else {
        animation_node = NULL;
    }
}

void Action::update_next_action() {
    if (is_inside_tree() && next_action_path != NodePath() && has_node(next_action_path)) {
        next_action = get_node(next_action_path)->cast_to<Action>();
    }else {
        next_action = NULL;
    }
}

BehaviorNode::Status Action::_behavior(const Variant& target, Dictionary env) {
    TimerBNode::_behavior(target, env);
    old_move = ((Vector2)env["move"]).x;
    _is_hit = false;
    if (animation_node) {
        AnimController *manager = animation_node->cast_to<AnimController>();
        if (manager) {
            if (animation_type != "" && animation_name != "")
                manager->set_status(animation_type, animation_name);
        }else {
            AnimationPlayer *player = animation_node->cast_to<AnimationPlayer>();
            if (player && animation_name != "")
                player->play(animation_name);
        }
    }
    return BehaviorNode::STATUS_RUNNING;
}

void Action::cancel_animation() {
    if (animation_node) {
        AnimController *manager = animation_node->cast_to<AnimController>();
        if (manager) {
            if (animation_type != "" && animation_name != "") {
                manager->remove_status_with(animation_type, animation_name);
            }
        }
    }
}

void Action::_reset(const Variant &target) {
    TimerBNode::_reset(target);
    if (reset_from_cancel) {
        _cancel_behavior(target);
        if (get_script_instance()) {
            const Variant* ptr[1]={&target};
            get_script_instance()->call_multilevel(StringName("_cancel_behavior"),ptr,1);
        }
    }
}

void Action::_timeout_behavior(const Variant& target, Dictionary& env) {
    cancel_animation();
    if (next_action) {
        next_action->reset(target);
        next_action->force_enter = true;
        next_action->set_focus();
    }
}

void Action::_cancel_behavior(const Variant& target) {
    cancel_animation();
}

void Action::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("refresh_cancel_list"), &Action::refresh_cancel_list);

    ObjectTypeDB::bind_method(_MD("set_hit", "hit"), &Action::set_hit);
    ObjectTypeDB::bind_method(_MD("get_hit"), &Action::get_hit);

    ObjectTypeDB::bind_method(_MD("set_max_move", "max_move"), &Action::set_max_move);
    ObjectTypeDB::bind_method(_MD("get_max_move"), &Action::get_max_move);

    ObjectTypeDB::bind_method(_MD("set_drag", "drag"), &Action::set_drag);
    ObjectTypeDB::bind_method(_MD("get_drag"), &Action::get_drag);

    ObjectTypeDB::bind_method(_MD("set_cancel_time", "cancel_time"), &Action::set_cancel_time);
    ObjectTypeDB::bind_method(_MD("get_cancel_time"), &Action::get_cancel_time);

    ObjectTypeDB::bind_method(_MD("set_cancel_type", "cancel_type"), &Action::set_cancel_type);
    ObjectTypeDB::bind_method(_MD("get_cancel_type"), &Action::get_cancel_type);

    ObjectTypeDB::bind_method(_MD("set_animation_path", "animation_path"), &Action::set_animation_path);
    ObjectTypeDB::bind_method(_MD("get_animation_path"), &Action::get_animation_path);

    ObjectTypeDB::bind_method(_MD("get_animation_node"), &Action::get_animation_node);

    ObjectTypeDB::bind_method(_MD("set_animation_type", "animation_type"), &Action::set_animation_type);
    ObjectTypeDB::bind_method(_MD("get_animation_type"), &Action::get_animation_type);

    ObjectTypeDB::bind_method(_MD("set_animation_name", "animation_name"), &Action::set_animation_name);
    ObjectTypeDB::bind_method(_MD("get_animation_name"), &Action::get_animation_name);

    ObjectTypeDB::bind_method(_MD("set_next_action_path", "naxt_action_path"), &Action::set_next_action_path);
    ObjectTypeDB::bind_method(_MD("get_next_action_path"), &Action::get_next_action_path);

    ObjectTypeDB::bind_method(_MD("get_next_action"), &Action::get_next_action);

    ADD_PROPERTY(PropertyInfo(Variant::REAL, "move/max_move"), _SCS("set_max_move"), _SCS("get_max_move"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "move/drag"), _SCS("set_drag"), _SCS("get_drag"));

    ADD_PROPERTY(PropertyInfo(Variant::REAL, "action/cancel_time"), _SCS("set_cancel_time"), _SCS("get_cancel_time"));
    ADD_PROPERTY(PropertyInfo(Variant::INT, "action/cancel_type",PROPERTY_HINT_ENUM,"Time,Hit,None"), _SCS("set_cancel_type"), _SCS("get_cancel_type"));
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "action/animation_path"), _SCS("set_animation_path"), _SCS("get_animation_path"));
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "action/animation_type"), _SCS("set_animation_type"), _SCS("get_animation_type"));
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "action/animation_name"), _SCS("set_animation_name"), _SCS("get_animation_name"));

    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "next_action_path"), _SCS("set_next_action_path"), _SCS("get_next_action_path"));

    BIND_VMETHOD( MethodInfo("_cancel_behavior", PropertyInfo(Variant::OBJECT,"target")) );

    BIND_CONSTANT(CANCEL_TYPE_HIT);
    BIND_CONSTANT(CANCEL_TYPE_TIME);
    BIND_CONSTANT(CANCEL_TYPE_NONE);
}