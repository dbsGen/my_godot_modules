//
// Created by Gen on 2015/9/4.
//

#ifndef GODOT_MASTER_CANCEL_NODE_H
#define GODOT_MASTER_CANCEL_NODE_H

#include "../../behaviornode/linkerbnode.h"
#include "../../../core/object.h"

class CancelNode : public LinkerBNode {
    OBJ_TYPE(CancelNode, LinkerBNode);

public:
    enum CANCEL_TYPE{
        NONE,
        TIME,
        HIT
    };

private:

    CANCEL_TYPE cancel_type;
    float cancel_time;

protected:
    static void _bind_methods();

public:
    _FORCE_INLINE_ void set_cancel_type(CANCEL_TYPE p_type) {cancel_type = p_type;}
    _FORCE_INLINE_ int get_cancel_type() {return cancel_type;}

    _FORCE_INLINE_ void set_cancel_time(float p_time) {cancel_time = p_time;}
    _FORCE_INLINE_ float get_cancel_time() {return cancel_time;}

    CancelNode() {cancel_time = 0; cancel_type = NONE;}
};

VARIANT_ENUM_CAST(CancelNode::CANCEL_TYPE);

#endif //GODOT_MASTER_CANCEL_NODE_H
