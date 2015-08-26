//
// Created by gen on 15-5-21.
//

#ifndef GODOT_MASTER_BUFFPROCESS_H
#define GODOT_MASTER_BUFFPROCESS_H

#include "../../behaviornode/behaviornode.h"

class BuffProcess : public BehaviorNode {
    OBJ_TYPE(BuffProcess, BehaviorNode);
protected:
    virtual Status _behavior(const Variant& target, Dictionary env);

public:
    BuffProcess(){}
};


#endif //GODOT_MASTER_BUFFPROCESS_H
