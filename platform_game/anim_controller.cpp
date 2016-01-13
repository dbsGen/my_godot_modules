//
// Created by gen on 15-5-2.
//

#include "anim_controller.h"
#include "../../core/script_language.h"
#include "../../scene/animation/animation_player.h"

Variant AnimController::getvar(const Variant& p_key, bool *r_valid) const {
    Variant ret;
    if (anim_nodes.has(p_key)) {
        *r_valid = true;
        ret = anim_nodes[p_key];
    }else {
        ret = Node::getvar(p_key, r_valid);
    }
    return ret;
}

void AnimController::_notification(int p_notification) {
    switch (p_notification) {
        case NOTIFICATION_FIXED_PROCESS:
        {
            float fixed_delta = get_fixed_process_delta_time();
            Vector<String> keys;
            for (Map<String, float >::Element *E = remove_counts.front(); E; E = E->next()) {
                float time = E->value();
                time -= fixed_delta;
                if (time <= 0) {
                    keys.push_back(E->key());
                }else {
                    E->value() = time;
                }
            }
            for (int n = 0, t = keys.size(); n < t; n++) {
                remove_status(keys[n]);
            }

            if (_changed) {
                _changed = false;
                _status_changed(added_anims, removed_anims);
                if (get_script_instance()) {
                    get_script_instance()->call_multilevel("_status_changed", added_anims, removed_anims);
                }
                added_anims.clear();
                removed_anims.clear();
            }
            if (_freeze_time > 0) {
                _freeze_time -= get_fixed_process_delta_time();
                if (_freeze_time <= 0) {
                    _freeze_time = 0;
                    resume();
                }
            }
        };
    }
}

void AnimController::resume() {
    for (List<AnimationStatus >::Element *E = status_cache.front(); E; E=E->next()) {
        AnimationStatus status = E->get();
        if (status.player) {
            status.player->set_active(true);
            status.player->play(status.name);
            status.player->seek(status.position);
        }
    }
    status_cache.clear();
}

void AnimController::freeze(float time) {
    _freeze_time = time;
    if (_freeze_time <= 0) {
        status_cache.clear();
        for (Map<StringName, Node*>::Element *E = anim_nodes.front(); E; E=E->next()) {
            AnimationPlayer *player = E->get()->cast_to<AnimationPlayer>();
            if (player->is_playing()) {
                AnimationStatus status;
                status.name = player->get_current_animation();
                status.position = player->get_current_animation_pos();
                status.player = player;
                status_cache.push_back(status);
                player->stop(false);
                player->set_active(false);
            }
        }
    }
}

void AnimController::set_status(String p_key, String p_name, float p_time) {
    if (!current_anims.has(p_key) || current_anims[p_key] != p_name) {
        if (current_anims.has(p_key)) {
            remove_anim(p_key, current_anims[p_key]);
        }
        current_anims[p_key] = p_name;
        add_anim(p_key, p_name);
        if (p_time > 0) {
            remove_counts[p_key] = p_time;
        }
        _changed = true;
        if (get_script_instance()) {
            get_script_instance()->call(StringName("_add_status"), p_key, p_name);
        }
    }
}

void AnimController::remove_status(String p_key) {
    if (current_anims.has(p_key)) {
        String name = current_anims[p_key];
        remove_anim(p_key, name);
        if (remove_counts.has(p_key)) {
            remove_counts.erase(p_key);
        }
        current_anims.erase(p_key);
        _changed = true;
        if (get_script_instance()) {
            get_script_instance()->call(StringName("_remove_status"), p_key, name);
        }
    }
}

void AnimController::remove_status_with(const String& p_key, const String& p_name) {
    if (current_anims.has(p_key) && current_anims[p_key] == p_name) {
        remove_anim(p_key, p_name);
        if (remove_counts.has(p_key)) {
            remove_counts.erase(p_key);
        }
        current_anims.erase(p_key);
        _changed = true;
        if (get_script_instance()) {
            get_script_instance()->call(StringName("_remove_status"), p_key, p_name);
        }
    }
}

void AnimController::remove_all() {
    if (current_anims.size() != 0) {
        current_anims.clear();
        _changed = true;
    }
}

Variant AnimController::get_status(String p_key) {
    if (current_anims.has(p_key)) {
        return current_anims[p_key];
    } else {
        return Variant();
    }
}

void AnimController::add_child_notify(Node *p_child) {
    if (p_child->is_type("AnimationPlayer")) {
        anim_nodes[p_child->get_name()] = p_child;
        Vector<Variant> binds;
        binds.push_back(p_child);
        p_child->connect(StringName("finished"), this, StringName("_animation_finished"), binds);
    }
}

void AnimController::remove_child_notify(Node *p_child) {
    if (p_child->is_type("AnimationPlayer") && anim_nodes.has(p_child->get_name())) {
        anim_nodes.erase(p_child->get_name());
        p_child->disconnect(StringName("finished"), this, StringName("_animation_finished"));
    }
}

void AnimController::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("get_status", "key"),&AnimController::get_status);
    ObjectTypeDB::bind_method(_MD("remove_all"),&AnimController::remove_all);
    ObjectTypeDB::bind_method(_MD("remove_status_with", "key", "name"),&AnimController::remove_status_with);
    ObjectTypeDB::bind_method(_MD("remove_status", "key"),&AnimController::remove_status);
    ObjectTypeDB::bind_method(_MD("set_status", "key", "name", "time"),&AnimController::set_status, DEFVAL(0));
    ObjectTypeDB::bind_method(_MD("_animation_finished", "anim"),&AnimController::_animation_finished);

    BIND_VMETHOD( MethodInfo("_status_changed", PropertyInfo(Variant::DICTIONARY, "added"), PropertyInfo(Variant::DICTIONARY, "removed")));
    BIND_VMETHOD( MethodInfo("_animation_finished", PropertyInfo(Variant::OBJECT, "anim")));

    BIND_VMETHOD( MethodInfo("_add_status", PropertyInfo(Variant::STRING, "key"), PropertyInfo(Variant::STRING, "name")));
    BIND_VMETHOD( MethodInfo("_remove_status", PropertyInfo(Variant::STRING, "key"), PropertyInfo(Variant::STRING, "name")));
}
AnimController::~AnimController() {
    for (Map<StringName, Node*>::Element *E = anim_nodes.front(); E; E=E->next()) {
        E->get()->disconnect("finished", this, "_animation_finished");
    }
}
