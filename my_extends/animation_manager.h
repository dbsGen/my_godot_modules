//
// Created by gen on 15-5-2.
//

#ifndef GODOT_MASTER_ANIMATION_MANAGER_H
#define GODOT_MASTER_ANIMATION_MANAGER_H

#include "../../core/reference.h"
#include "../../scene/main/node.h"
#include "../../core/ustring.h"

class AnimationManager : public Node {
    OBJ_TYPE(AnimationManager, Node);
private:
    Dictionary anim_nodes;
    HashMap<String, String> current_anims;
    void reload_child();

protected:
    virtual Variant getvar(const Variant& p_key, bool *r_valid) const;
    virtual void _play() {}
    virtual void add_child_notify(Node *p_child);
    virtual void remove_child_notify(Node *p_child);
    void _notification(int p_what);
    static void _bind_methods();

public:
    void run();

    Variant get_anim(String p_key);
    void play(String p_key, String p_name);
    void stop(String p_key);
    void stop_with_name(String p_key, String p_name);
    void stop_all();

    AnimationManager() {set_fixed_process(true);}
};


#endif //GODOT_MASTER_ANIMATION_MANAGER_H
