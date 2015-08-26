//
// Created by gen on 15-5-9.
//

#ifndef GODOT_MASTER_STATUSBNODE_H
#define GODOT_MASTER_STATUSBNODE_H

#include "behaviornode.h"
#include "../../core/typedefs.h"

class StatusBNode : public BehaviorNode {
    OBJ_TYPE(StatusBNode, BehaviorNode);
private:
    int _selected;
protected:

    virtual Status _step(const Variant& target, Dictionary &env);
    static void _bind_methods();
public:
    _FORCE_INLINE_ void set_select(int select) {_selected = select;}
    _FORCE_INLINE_ int get_select() {return _selected;}

    StatusBNode() {_selected = 0;}
};


#endif //GODOT_MASTER_STATUSBNODE_H
