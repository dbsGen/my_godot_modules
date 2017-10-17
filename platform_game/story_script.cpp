//
// Created by gen on 15-6-13.
//

#include "story_script.h"
#include "../../core/os/file_access.h"

void StoryScriptNode::update_camera() {
    if (is_inside_tree()) {
        if (_camera_path != NodePath() && has_node(_camera_path)) {
            _camera = get_node(_camera_path);
        }
    }
}

void StoryScriptNode::over() {
    emit_signal("over");
}

void StoryScriptNode::_bind_methods() {
    ClassDB::bind_method(D_METHOD("run"), &StoryScriptNode::run);
    ClassDB::bind_method(D_METHOD("camera:Node"), &StoryScriptNode::camera);
    ClassDB::bind_method(D_METHOD("params:Dictionary"),&StoryScriptNode::params);

    ClassDB::bind_method(D_METHOD("set_camera_path", "camera_path:NodePath"), &StoryScriptNode::set_camera_path);
    ClassDB::bind_method(D_METHOD("get_camera_path:NodePath"), &StoryScriptNode::get_camera_path);

    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "camera_path"), "set_camera_path", "get_camera_path");

    ADD_SIGNAL(MethodInfo("over"));
}

void StoryScriptNode::_notification(int p_notification) {
    switch (p_notification) {
        case NOTIFICATION_ENTER_TREE: {
            update_camera();
            break;
        }
        default:
            break;

    }
}

bool StoryScriptNode::run() {
    if (script_resource == NULL) {
        return false;
    }else {
        script_resource->_camera = _camera;
        script_resource->_params = _params;
        script_resource->_node = this;
        script_resource->run();
        return true;
    }
}

void StoryScript::run() {
    if (get_script_instance()) {
        get_script_instance()->call("run");
    }
}

void StoryScript::_bind_methods() {
    ClassDB::bind_method(D_METHOD("camera:Node"),&StoryScript::camera);
    ClassDB::bind_method(D_METHOD("params:Dictionary"),&StoryScript::params);
    ClassDB::bind_method(D_METHOD("over"),&StoryScript::over);

    BIND_VMETHOD(MethodInfo("run"));


}