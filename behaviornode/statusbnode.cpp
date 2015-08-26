//
// Created by gen on 15-5-9.
//

#include "statusbnode.h"

BehaviorNode::Status StatusBNode::_step(const Variant &target, Dictionary &env) {
    if (!get_behavior_enable() || get_child_count() == 0)
        return STATUS_FAILURE;
    if ((bool)call(StringName("pre_behavior"),target, Variant(env))) {
        if (_selected < 0) {
            _selected = 0;
        }else if (_selected >= get_child_count()) {
            _selected = get_child_count() - 1;
        }
        BehaviorNode *b_node = get_child(_selected)->cast_to<BehaviorNode>();\
        Status childrenStatus = STATUS_FAILURE;
        if (b_node) {
            childrenStatus =   b_node->step(target, env);
        }
        Status status = (Status)((int)call(StringName("behavior"),target, Variant(env)));
        if (status == STATUS_DEPEND_ON_CHILDREN)
            return childrenStatus;
        else
            return status;
    }else {
        return STATUS_FAILURE;
    }
}

void StatusBNode::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("set_select", "status"), &StatusBNode::set_select);
    ObjectTypeDB::bind_method(_MD("get_select"), &StatusBNode::get_select);

    ADD_PROPERTY( PropertyInfo( Variant::REAL, "status/status" ), _SCS("set_select"),_SCS("get_select" ) );
}