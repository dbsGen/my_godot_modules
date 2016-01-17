//
// Created by gen on 15-5-2.
//

#ifndef GODOT_MASTER_ANIMATION_MANAGER_H
#define GODOT_MASTER_ANIMATION_MANAGER_H

#include "../../core/reference.h"
#include "../../scene/main/node.h"
#include "../../core/ustring.h"

class AnimationPlayer;

class AnimController : public Node {
    OBJ_TYPE(AnimController, Node);
private:

    struct AnimationStatus {
        StringName name;
        float position;
        AnimationPlayer *player;

        AnimationStatus() {
            position = 0;
            player = NULL;
        }
    };

    Map<StringName, Node*> anim_nodes;
    List<AnimationStatus> status_cache;
    HashMap<String, String> current_anims;
    Map<String, float > remove_counts;
    Dictionary removed_anims;
    Dictionary added_anims;
    bool _freezing;

    bool _changed;

    _FORCE_INLINE_ void add_anim(String p_key, String p_name) {
        if (removed_anims.has(p_key) && removed_anims[p_key] == p_name) removed_anims.erase(p_key);
        added_anims[p_key] = p_name;
    }

    _FORCE_INLINE_ void remove_anim(String p_key, String p_name) {
        if (added_anims.has(p_key) && added_anims[p_key] == p_name) added_anims.erase(p_key);
        if (!removed_anims.has(p_key)) removed_anims[p_key] = p_name;
    }
protected:
    virtual Variant getvar(const Variant& p_key, bool *r_valid) const;
    virtual void _status_changed(Dictionary added, Dictionary removed) {}
    virtual void add_child_notify(Node *p_child);
    virtual void remove_child_notify(Node *p_child);
    virtual void _animation_finished(Node *node){};
    void _notification(int p_what);
    static void _bind_methods();

public:

    Variant get_status(String p_key);
    void set_status(String p_key, String p_name, float time = 0);
    void remove_status(String p_key);
    void remove_status_with(const String& p_key, const String& p_name);
    void remove_all();
    void freeze();
    void resume();

    AnimController() {set_fixed_process(true);_changed= false;_freezing=false;}
    ~AnimController();
};


#endif //GODOT_MASTER_ANIMATION_MANAGER_H
