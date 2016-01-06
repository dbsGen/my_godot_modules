#include "inputstorage.h"
#include "../../core/os/input.h"
#include "../../core/os/os.h"
#include "../../scene/main/viewport.h"
#include "../../scene/main/scene_main_loop.h"

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
bool InputNode::is_pressed(const StringArray& actions) const {
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
bool InputNode::is_down(const StringArray &actions) const {
    for (int i = 0, t = actions.size(); i < t; ++i) {
        if (!is_down(actions[i])) {
            return false;
        }
    }
    return true;
}
int InputNode::queue_down(const StringArray &actions, int offset) const {
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
    SceneTree *tree = main_loop->cast_to<SceneTree>();
    ERR_FAIL_COND(tree == NULL);

    tree->disconnect("idle_frame", this, "_add_node");
    tree->get_root()->add_child(node->cast_to<InputStorageNode>());
}

void InputStorage::start(StringArray events) {
    const String node_key = "input_storage_node";
    MainLoop *main_loop = OS::get_singleton()->get_main_loop();
    SceneTree *tree = main_loop->cast_to<SceneTree>();
    ERR_FAIL_COND(tree == NULL);

    Viewport *viewport = tree->get_root();
    ERR_FAIL_COND(viewport == NULL);
    if (storageNode == NULL) {
        storageNode = memnew(InputStorageNode);
        storageNode->set_name(node_key);
        storageNode->_storage = Ref<InputStorage>(this);

        Vector<Variant> vector;
        vector.push_back(Variant(storageNode));

        tree->connect(StringName("idle_frame"), this, StringName("_add_node"), vector, 0);

    }
    storageNode->set_events(events);
}

void InputStorage::close() {
    if  (storageNode != NULL) {
        storageNode->set_process_input(false);
        storageNode->set_fixed_process(false);
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

bool InputStorage::test_down(const StringArray &events, int in_frame) {
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
    }else if ((type == Variant::STRING_ARRAY || type == Variant::ARRAY) && node.is_down((const StringArray&)input)) {
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
    }else if ((type == Variant::STRING_ARRAY || type == Variant::ARRAY) && node.is_pressed((const StringArray&)input)) {
        return true;
    }
    return false;
}

void InputStorage::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("start", "events"), &InputStorage::start);
    ObjectTypeDB::bind_method(_MD("close"), &InputStorage::close);
    ObjectTypeDB::bind_method(_MD("frame_begin"), &InputStorage::frame_begin);
    ObjectTypeDB::bind_method(_MD("pressed_event", "event"), &InputStorage::pressed_event);
    ObjectTypeDB::bind_method(_MD("down_event", "event"), &InputStorage::down_event);
    ObjectTypeDB::bind_method(_MD("_add_node", "node"), &InputStorage::_add_node);

    ObjectTypeDB::bind_method(_MD("get_storage_size"), &InputStorage::get_storage_size);

    ObjectTypeDB::bind_method(_MD("pressed_at", "events", "at_frame"), &InputStorage::pressed_at, Variant(""), 0);
    ObjectTypeDB::bind_method(_MD("down_frame", "event", "in_frame"), &InputStorage::down_frame, Variant(""), 1);
    ObjectTypeDB::bind_method(_MD("test_down", "events", "in_frame"), &InputStorage::test_down, Variant(StringArray()), 1);
    ObjectTypeDB::bind_method(_MD("is_pressed", "event", "in_frame"), &InputStorage::is_pressed, Variant(""), 1);
    ObjectTypeDB::bind_method(_MD("is_down", "event", "in_frame"), &InputStorage::is_down, Variant(""), 1);

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

void InputStorageNode::_input(const InputEvent& p_event) {
    if (_storage != NULL) {
        for (int i = 0, t = events.size(); i < t; ++i) {
            if (p_event.is_action(events[i]) && !p_event.is_echo()) {
                if (p_event.is_pressed()) {
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
    ObjectTypeDB::bind_method(_MD("_input"),&InputStorageNode::_input);
    ObjectTypeDB::bind_method(_MD("get_storage_size", "storage_size"), &InputStorageNode::get_storage_size);
    ObjectTypeDB::bind_method(_MD("set_storage_size"), &InputStorageNode::set_storage_size);
    ObjectTypeDB::bind_method(_MD("get_events", "events"), &InputStorageNode::get_events);
    ObjectTypeDB::bind_method(_MD("set_events"), &InputStorageNode::set_events);

    ADD_PROPERTY( PropertyInfo( Variant::INT, "input_storage_node/storage_size" ), _SCS("set_storage_size"),_SCS("get_storage_size" ) );
    ADD_PROPERTY( PropertyInfo( Variant::STRING_ARRAY, "input_storage_node/events" ), _SCS("set_events"),_SCS("get_events" ) );
}