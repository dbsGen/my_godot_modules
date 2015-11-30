
#include "behaviornode.h"
#include "../../scene/scene_string_names.h"
#include "../../core/math/math_2d.h"
#include "../platform_game/behaviornodes/action.h"

//void BehaviorNode::_notification(int p_notification) {
//    case NOTIFICATION_READY: {
//    }
//}

//StringName BehaviorNode::C_PreBehavior = ;
//StringName BehaviorNode::C_Behavior = ;

BehaviorNode::Status BehaviorNode::_traversal_children(const Variant& target, Dictionary& env) {
    int t = get_child_count();
    Status res = STATUS_FAILURE;
    BehaviorNode *checked = NULL;
    do {
        if (_behavior_node_type == TYPE_CONDITION && _focus_node_path != String("") && has_node(_focus_node_path)) {
            BehaviorNode *child = get_node(_focus_node_path)->cast_to<BehaviorNode>();
            if (child && child != this) {
                if (!child->get_will_focus())
                    _focus_node_path = NodePath();
                NodePath old_path = _focus_node_path;
                int ret = (int)child->call("step", target, env);
                if (ret == STATUS_RUNNING) {
                    return STATUS_RUNNING;
                }else {
                    checked = child;
                    if (old_path == _focus_node_path)
                        _focus_node_path = NodePath();
                    else {
                        continue;
                    }
                }
            }
        }
        for (int i = 0; i < t; ++i) {
            BehaviorNode * child = get_child(i)->cast_to<BehaviorNode>();
            if (child == checked) continue;
            bool is_focus;
            if (child) {
                if ((int)child->call("step", target, env) == STATUS_RUNNING) {
                    res = STATUS_RUNNING;
                    is_focus = child->get_will_focus();
                    if (_behavior_node_type == TYPE_CONDITION) {
                        if (is_focus) {
                            _focus_node_path = get_path_to(child);
                        }
                        return res;
                    }
                }
            }
        }
        if (_behavior_node_type == TYPE_CONDITION) {
            _focus_node_path = NodePath();
        }
        return res;
    }while(true);
}

BehaviorNode::Status BehaviorNode::_step(const Variant& target, Dictionary &env) {
    if (!_behavior_enable)
        return STATUS_FAILURE;
    if ((bool)call("pre_behavior", target, env)) {
        Status childrenStatus =  _traversal_children(target, env);
        Status status = (Status)((int)call(StringName("behavior"),target, Variant(env)));
        if (status == STATUS_DEPEND_ON_CHILDREN)
            return childrenStatus;
        else
            return status;
    }else {
        return STATUS_FAILURE;
    }
}

BehaviorNode::Status BehaviorNode::step(const Variant& target, Dictionary env) {
    return _step(target, env);
}

void BehaviorNode::set_focus() {
    BehaviorNode *parent = get_parent()->cast_to<BehaviorNode>();
    if (parent) {
        parent->_focus_node_path = parent->get_path_to(this);
        print_line("Focus to " + String(parent->_focus_node_path));
    }
}

void BehaviorNode::clear_focus() {
    BehaviorNode *parent = get_parent()->cast_to<BehaviorNode>();
    if (parent && parent->_focus_node_path == parent->get_path_to(this)) {
        parent->_focus_node_path = NodePath();
    }
}

void BehaviorNode::reset(const Variant &target) {
    _reset(target);
    _script_reset(target);

    int t = get_child_count();
    for (int i = 0; i < t; ++i) {
        BehaviorNode * child = get_child(i)->cast_to<BehaviorNode>();
        if (child) {
            child->reset(target);
        }
    }
}

void BehaviorNode::_script_reset(const Variant &target) {
    if (get_script_instance()) {
        const Variant* ptr[1]={&target};
        get_script_instance()->call_multilevel(StringName("_reset"),ptr,1);
    }
}

void BehaviorNode::_reset(const Variant &target) {
    _focus_node_path = NodePath();
}


void BehaviorNode::_bind_methods() {

    ObjectTypeDB::bind_method(_MD("set_behavior_enable","enable"),&BehaviorNode::set_behavior_enable);
    ObjectTypeDB::bind_method(_MD("get_behavior_enable"),&BehaviorNode::get_behavior_enable);

    ObjectTypeDB::bind_method(_MD("set_behavior_node_type","behavior_node_type"),&BehaviorNode::set_behavior_node_type);
    ObjectTypeDB::bind_method(_MD("get_behavior_node_type"),&BehaviorNode::get_behavior_node_type);

    ObjectTypeDB::bind_method(_MD("set_will_focus","will_focus"),&BehaviorNode::set_will_focus);
    ObjectTypeDB::bind_method(_MD("get_will_focus"),&BehaviorNode::get_will_focus);

    ObjectTypeDB::bind_method(_MD("pre_behavior:bool", "target", "env"),&BehaviorNode::_pre_behavior);
    ObjectTypeDB::bind_method(_MD("behavior", "target", "env"), &BehaviorNode::_behavior);

    ObjectTypeDB::bind_method(_MD("set_focus"),&BehaviorNode::set_focus);

    ObjectTypeDB::bind_method(_MD("step"),&BehaviorNode::step);
    ObjectTypeDB::bind_method(_MD("reset"),&BehaviorNode::reset);

    ADD_PROPERTY( PropertyInfo( Variant::INT, "behavior/type",PROPERTY_HINT_ENUM,"Sequence,Condition" ), _SCS("set_behavior_node_type"),_SCS("get_behavior_node_type" ) );
    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "behavior/enable" ), _SCS("set_behavior_enable"),_SCS("get_behavior_enable" ) );
    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "behavior/focus" ), _SCS("set_will_focus"),_SCS("get_will_focus" ) );

    BIND_VMETHOD( MethodInfo("pre_behavior", PropertyInfo(Variant::OBJECT,"target"), PropertyInfo(Variant::DICTIONARY,"env")) );
    BIND_VMETHOD( MethodInfo("behavior", PropertyInfo(Variant::OBJECT,"target"), PropertyInfo(Variant::DICTIONARY,"env")) );
    BIND_VMETHOD( MethodInfo("step", PropertyInfo(Variant::OBJECT,"target"), PropertyInfo(Variant::DICTIONARY,"env")) );
    BIND_VMETHOD( MethodInfo("_reset", PropertyInfo(Variant::OBJECT,"target")) );
    BIND_CONSTANT(TYPE_SEQUENCE);
    BIND_CONSTANT(TYPE_CONDITION);
    BIND_CONSTANT(STATUS_DEPEND_ON_CHILDREN);
    BIND_CONSTANT(STATUS_FAILURE);
    BIND_CONSTANT(STATUS_RUNNING);
}

