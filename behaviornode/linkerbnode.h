//
// Created by gen on 15-5-24.
//

#ifndef GODOT_MASTER_LINKERBNODE_H
#define GODOT_MASTER_LINKERBNODE_H

#include "behaviornode.h"

class LinkerBNode : public BehaviorNode {
    OBJ_TYPE(LinkerBNode, BehaviorNode);
private:
    BehaviorNode *link_target;
    NodePath    link_path;
    void update_link_path();
protected:
    void _notification(int p_notification);
    virtual Status  _step(const Variant& target, const Dictionary &env);;
    static void _bind_methods();
public:
    _FORCE_INLINE_ BehaviorNode *get_link_target() { return link_target;}

    void set_link_path(NodePath path);
    _FORCE_INLINE_ NodePath get_link_path() {return link_path;}
};


#endif //GODOT_MASTER_LINKERBNODE_H
