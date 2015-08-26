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
    ObjectTypeDB::bind_method(_MD("run"), &StoryScriptNode::run);
    ObjectTypeDB::bind_method(_MD("camera:Node"), &StoryScriptNode::camera);
    ObjectTypeDB::bind_method(_MD("params:Dictionary"),&StoryScriptNode::params);

    ObjectTypeDB::bind_method(_MD("set_camera_path", "camera_path:NodePath"), &StoryScriptNode::set_camera_path);
    ObjectTypeDB::bind_method(_MD("get_camera_path:NodePath"), &StoryScriptNode::get_camera_path);

    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "camera_path"), _SCS("set_camera_path"), _SCS("get_camera_path"));

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
    ObjectTypeDB::bind_method(_MD("camera:Node"),&StoryScript::camera);
    ObjectTypeDB::bind_method(_MD("params:Dictionary"),&StoryScript::params);
    ObjectTypeDB::bind_method(_MD("over"),&StoryScript::over);

    BIND_VMETHOD(MethodInfo("run"));


}