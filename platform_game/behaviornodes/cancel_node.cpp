//
// Created by Gen on 2015/9/4.
//

#include "cancel_node.h"

void CancelNode::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("set_cancel_type"), &CancelNode::set_cancel_type);
    ObjectTypeDB::bind_method(_MD("get_cancel_type"), &CancelNode::get_cancel_type);

    ObjectTypeDB::bind_method(_MD("set_cancel_time"), &CancelNode::set_cancel_time);
    ObjectTypeDB::bind_method(_MD("get_cancel_time"), &CancelNode::get_cancel_time);

    ADD_PROPERTYNZ(PropertyInfo(Variant::INT, "cancel_type", PROPERTY_HINT_ENUM, "NONE,TIME,HIT"), _SCS("set_cancel_type"), _SCS("get_cancel_type"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "cancel_time"), _SCS("set_cancel_time"), _SCS("get_cancel_time"));

    BIND_CONSTANT(NONE);
    BIND_CONSTANT(TIME);
    BIND_CONSTANT(HIT);
}