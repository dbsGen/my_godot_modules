//
// Created by gen on 15-5-24.
//

#ifndef GODOT_MASTER_ATTACK_NODE_H
#define GODOT_MASTER_ATTACK_NODE_H

#include "action.h"
#include "../attack_area.h"

class AttackNode : public Action {
OBJ_TYPE(AttackNode, Action);
private:
    AttackArea  *attack_area;
    NodePath    attack_area_path;
    void    update_attack_area();
protected:
    void _notification(int p_notification);
    static void _bind_methods();
    virtual void _during_behavior(const Variant& target, Dictionary& env);
    virtual void _timeout_behavior(const Variant& target, Dictionary& env);
    virtual void _on_attack(const Variant& target, Dictionary& env) {}
    virtual void _reset(const Variant& target);
public:
    void set_attack_area_path(NodePath path);
    _FORCE_INLINE_ NodePath get_attack_area_path() {return attack_area_path;}

    _FORCE_INLINE_ AttackArea *get_attack_area() {return attack_area;}

    AttackNode() {attack_area=NULL;}
};


#endif //GODOT_MASTER_ATTACK_NODE_H
