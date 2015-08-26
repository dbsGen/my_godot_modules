//
// Created by gen on 15-5-2.
//

#include "animation_manager.h"
#include "../../core/script_language.h"

Variant AnimationManager::getvar(const Variant& p_key, bool *r_valid) const {
    Variant ret;
    if (anim_nodes.has(p_key)) {
        *r_valid = true;
        ret = anim_nodes[p_key];
    }else {
        ret = Node::getvar(p_key, r_valid);
    }
    return ret;
}

void AnimationManager::run() {
    _play();
    if (get_script_instance()) {
        get_script_instance()->call_multilevel("_play");
    }
}

void AnimationManager::_notification(int p_notification) {
    switch (p_notification) {
        case NOTIFICATION_FIXED_PROCESS:
        {
            run();
        };
    }
}

void AnimationManager::play(String p_key, String p_name) {
    current_anims[p_key]= p_name;
}

void AnimationManager::stop(String p_key) {
    if (current_anims.has(p_key))
        current_anims.erase(p_key);
}

void AnimationManager::stop_with_name(String p_key, String p_name) {
    if (current_anims.has(p_key) && current_anims[p_key] == p_name)
        current_anims.erase(p_key);
}

void AnimationManager::stop_all() {
    current_anims.clear();
}

Variant AnimationManager::get_anim(String p_key) {
    if (current_anims.has(p_key)) {
        return current_anims[p_key];
    } else {
        return Variant();
    }
}

void AnimationManager::add_child_notify(Node *p_child) {
    if (p_child->is_type("AnimationPlayer")) {
        anim_nodes[p_child->get_name()] = p_child;
    }
}

void AnimationManager::remove_child_notify(Node *p_child) {
    if (p_child->is_type("AnimationPlayer") && anim_nodes.has(p_child->get_name())) {
        anim_nodes.erase(p_child->get_name());
    }
}

void AnimationManager::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("run"),&AnimationManager::run);
    ObjectTypeDB::bind_method(_MD("get_anim", "key"),&AnimationManager::get_anim);
    ObjectTypeDB::bind_method(_MD("stop_all"),&AnimationManager::stop_all);
    ObjectTypeDB::bind_method(_MD("stop_with_name", "key", "name"),&AnimationManager::stop_with_name);
    ObjectTypeDB::bind_method(_MD("stop", "key"),&AnimationManager::stop);
    ObjectTypeDB::bind_method(_MD("play", "key", "name"),&AnimationManager::play);

    BIND_VMETHOD( MethodInfo("_play") );
}
