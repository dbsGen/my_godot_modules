//
// Created by Gen on 2015/9/4.
//

#include "cancel_node.h"

void CancelNode::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_cancel_type"), &CancelNode::set_cancel_type);
    ClassDB::bind_method(D_METHOD("get_cancel_type"), &CancelNode::get_cancel_type);

    ClassDB::bind_method(D_METHOD("set_cancel_time"), &CancelNode::set_cancel_time);
    ClassDB::bind_method(D_METHOD("get_cancel_time"), &CancelNode::get_cancel_time);

    ADD_PROPERTYNZ(PropertyInfo(Variant::INT, "cancel_type", PROPERTY_HINT_ENUM, "NONE,TIME,HIT"), "set_cancel_type", "get_cancel_type");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "cancel_time"), "set_cancel_time", "get_cancel_time");

    BIND_CONSTANT(NONE);
    BIND_CONSTANT(TIME);
    BIND_CONSTANT(HIT);
}