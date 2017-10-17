#include "inputstorage.h"
#include "../../core/os/input.h"
#include "../../core/os/os.h"
#include <core/os/main_loop.h>
#include "../../scene/main/viewport.h"

InputStorage *InputStorage::singleton = NULL;

InputStorage *InputStorage::get_singleton() {
    if (!singleton) {
        singleton = memnew(InputStorage);
    }
    return singleton;
}

void InputNode::pressed_action(const String &action) {
    if (_pressed_actions.find(action) == -1)
        _pressed_actions.append(action);
}
void InputNode::down_action(const String &action) {
    _down_actions.append(action);
}
bool InputNode::is_pressed(const String &action) const {
    return _pressed_actions.find(action) >= 0;
}
bool InputNode::is_pressed(const PoolStringArray& actions) const {
    for (int i = 0, t = actions.size(); i < t; ++i) {
        if (!is_pressed(actions[i])) {
            return false;
        }
    }
    return true;
}
bool InputNode::is_down(const String &action) const {
    return _down_actions.find(action) >= 0;
}
bool InputNode::is_down(const PoolStringArray &actions) const {
    for (int i = 0, t = actions.size(); i < t; ++i) {
        if (!is_down(actions[i])) {
            return false;
        }
    }
    return true;
}
int InputNode::queue_down(const PoolStringArray &actions, int offset) const {
    int ac = actions.size();
    ERR_FAIL_COND_V(offset >= ac, 0);
    int af = ac - offset - 1, dc = _down_actions.size(), df = dc - 1, res = 0;
    for (; af >= 0; af--) {
        for (; df >= 0; df --) {
            if (actions[af] == _down_actions[df]) {
                --df;
                res++;
                break;
            }
        }
    }
    return res;
}

void InputStorage::_add_node(Object *node) {
    MainLoop *main_loop = OS::get_singleton()->get_main_loop();
    SceneTree *tree = Object::cast_to<SceneTree>(main_loop);
    ERR_FAIL_COND(tree == NULL);

    tree->disconnect("idle_frame", this, "_add_node");
    tree->get_root()->add_child(Object::cast_to<InputStorageNode>(node));
}

void InputStorage::start(PoolStringArray events) {
    const String node_key = "input_storage_node";
    MainLoop *main_loop = OS::get_singleton()->get_main_loop();
    SceneTree *tree = Object::cast_to<SceneTree>(main_loop);
    ERR_FAIL_COND(tree == NULL);

    Viewport *viewport = tree->get_root();
    ERR_FAIL_COND(viewport == NULL);
    if (storage_node == NULL) {
        storage_node = memnew(InputStorageNode);
        storage_node->set_name(node_key);
        storage_node->_storage = Ref<InputStorage>(this);

        Vector<Variant> vector;
        vector.push_back(Variant(storage_node));

        tree->connect(StringName("idle_frame"), this, StringName("_add_node"), vector, 0);

    }
    storage_node->set_events(events);
}

void InputStorage::resume() {
    if  (storage_node != NULL) {
        storage_node->set_process_input(true);
        storage_node->set_fixed_process(true);
    }
}

void InputStorage::close() {
    if  (storage_node != NULL) {
        storage_node->set_process_input(false);
        storage_node->set_fixed_process(false);
        storage_node->pressed.clear();
        storage_events.clear();
    }
}

void InputStorage::frame_begin() {
    _this_frame = storage_events.push(InputNode());
}

void InputStorage::pressed_event(const String &event) {
    if (_this_frame != NULL)
        _this_frame->pressed_action(event);
}

void InputStorage::down_event(const String &event) {
    if (_this_frame != NULL)
        _this_frame->down_action(event);
}

bool InputStorage::test_down(const PoolStringArray &events, int in_frame) {
    int count = storage_events.size(), t = in_frame < count ? count - 1 - in_frame: 0, offset = 0, e_count = events.size();
    for (int j = count - 1; j >= t; --j) {
        const InputNode& node = storage_events[j];
        offset += node.queue_down(events, offset);
        if (e_count <= offset) {
            return true;
        }
    }
    return false;
}

bool InputStorage::is_down(const Variant &event, int in_frame) {
    int count = storage_events.size(), t = in_frame < count ? in_frame: count;
    for (int i = 0; i < t; ++i) {
        if (_down_in_frame(event, count-i-1)) {
            return true;
        }
    }
    return false;
}

bool InputStorage::is_pressed(const Variant &event, int in_frame) {
    int count = storage_events.size(), t = in_frame < count ? in_frame: count;
    for (int i = 0; i < t; ++i) {
        if (_pressed_in_frame(event, count-i-1)) {
            return true;
        }
    }
    return false;
}

bool InputStorage::pressed_at(const Variant &event, int at_frame) {
    int count = storage_events.size();
    if (at_frame < count) {
        return _pressed_in_frame(event, count-at_frame-1);
    }
    return false;
}

int InputStorage::down_frame(const Variant &event, int in_frame) {
    int count = storage_events.size(), t = in_frame < count ? in_frame: count;
    for (int i = 0; i < t; ++i) {
        if (_down_in_frame(event, count-i-1)) {
            return i;
        }
    }
    return -1;
}

bool InputStorage::_down_in_frame(const Variant &input, int frame) {
    ERR_FAIL_COND_V(!(frame >= 0 && frame < storage_events.size()), false);
    const InputNode& node = storage_events[frame];
    Variant::Type type = input.get_type();
    if (type == Variant::STRING && node.is_down((const String&)input)) {
        return true;
    }else if ((type == Variant::POOL_STRING_ARRAY || type == Variant::ARRAY) && node.is_down((const PoolStringArray&)input)) {
        return true;
    }
    return false;
}

bool InputStorage::_pressed_in_frame(const Variant &input, int frame) {
    ERR_FAIL_COND_V(!(frame >= 0 && frame < storage_events.size()), false);
    const InputNode&  node = storage_events[frame];
    Variant::Type type = input.get_type();
    if (type == Variant::STRING && node.is_pressed((const String&)input)) {
        return true;
    }else if ((type == Variant::POOL_STRING_ARRAY || type == Variant::ARRAY) && node.is_pressed((const PoolStringArray&)input)) {
        return true;
    }
    return false;
}

void InputStorage::_bind_methods() {
    ClassDB::bind_method(D_METHOD("start", "events"), &InputStorage::start);
    ClassDB::bind_method(D_METHOD("close"), &InputStorage::close);
    ClassDB::bind_method(D_METHOD("resume"), &InputStorage::resume);
    ClassDB::bind_method(D_METHOD("frame_begin"), &InputStorage::frame_begin);
    ClassDB::bind_method(D_METHOD("pressed_event", "event"), &InputStorage::pressed_event);
    ClassDB::bind_method(D_METHOD("down_event", "event"), &InputStorage::down_event);
    ClassDB::bind_method(D_METHOD("_add_node", "node"), &InputStorage::_add_node);

    ClassDB::bind_method(D_METHOD("get_storage_size"), &InputStorage::get_storage_size);

    ClassDB::bind_method(D_METHOD("pressed_at", "events", "at_frame"), &InputStorage::pressed_at, Variant(""), 0);
    ClassDB::bind_method(D_METHOD("down_frame", "event", "in_frame"), &InputStorage::down_frame, Variant(""), 1);
    ClassDB::bind_method(D_METHOD("test_down", "events", "in_frame"), &InputStorage::test_down, Variant(PoolStringArray()), 1);
    ClassDB::bind_method(D_METHOD("is_pressed", "event", "in_frame"), &InputStorage::is_pressed, Variant(""), 1);
    ClassDB::bind_method(D_METHOD("is_down", "event", "in_frame"), &InputStorage::is_down, Variant(""), 1);

}

void InputStorageNode::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_FIXED_PROCESS:
            if (_storage != NULL) {

                _storage->frame_begin();
                for (int i = 0, t = pressed.size(); i < t; ++i) {
                    _storage->pressed_event(pressed[i]);
                }
            }
            break;
    }
}

void InputStorageNode::_input(const Ref<InputEvent>& p_event) {
    if (_storage != NULL) {
        for (int i = 0, t = events.size(); i < t; ++i) {
            if (p_event->is_action(events[i]) && !p_event->is_echo()) {
                if (p_event->is_pressed()) {
                    _storage->down_event(events[i]);
                    _storage->pressed_event(events[i]);
                    if (pressed.find(events[i]) < 0)
                        pressed.push_back(events[i]);
                }else {
                    pressed.erase(events[i]);
                }
            }
        }
    }
}

void InputStorageNode::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_input"),&InputStorageNode::_input);
    ClassDB::bind_method(D_METHOD("set_storage_size", "storage_size"), &InputStorageNode::set_storage_size);
    ClassDB::bind_method(D_METHOD("get_storage_size"), &InputStorageNode::get_storage_size);
    ClassDB::bind_method(D_METHOD("set_events", "events"), &InputStorageNode::set_events);
    ClassDB::bind_method(D_METHOD("get_events"), &InputStorageNode::get_events);

    ADD_PROPERTY( PropertyInfo( Variant::INT, "input_storage_node/storage_size" ), "set_storage_size","get_storage_size");
    ADD_PROPERTY( PropertyInfo( Variant::POOL_STRING_ARRAY, "input_storage_node/events" ), "set_events","get_events");
}