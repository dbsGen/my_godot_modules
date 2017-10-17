//
// Created by gen on 15-6-13.
//

#ifndef GODOT_MASTER_SCRIPT_H
#define GODOT_MASTER_SCRIPT_H

#include "../../core/resource.h"
#include "../../core/ustring.h"
#include "../../scene/main/node.h"

class StoryScript;

class StoryScriptNode : public Node {
    GDCLASS(StoryScriptNode, Node);
private:
    NodePath _camera_path;
    Node *_camera;
    Dictionary _params;

    Ref<StoryScript> script_resource;

    void update_camera();
protected:
    void _notification(int p_notification);
    static void _bind_methods();
public:

    _FORCE_INLINE_ Node *camera() {return _camera;}
    _FORCE_INLINE_ Dictionary params() {return _params;}

    _FORCE_INLINE_ void set_camera_path(NodePath p_path) {_camera_path=p_path;update_camera();}
    _FORCE_INLINE_ NodePath get_camera_path() {return _camera_path;}

    _FORCE_INLINE_ void set_script_resource(Ref<StoryScript> p_resource) {script_resource=p_resource;}
    _FORCE_INLINE_ Ref<StoryScript> get_script_resource() {return script_resource;}

    bool run();
    void over();

    StoryScriptNode() {}
};

class StoryScript : public Resource {
    GDCLASS(StoryScript, Resource);
private:
    Node *_camera;
    Dictionary _params;
    StoryScriptNode *_node;

    friend class StoryScriptNode;
protected:
    static void _bind_methods();

public:

    _FORCE_INLINE_ Node* camera() {return _camera;}
    _FORCE_INLINE_ Dictionary params() {return _params;}

    void run();
    _FORCE_INLINE_ void over(){_node->over();}

    _FORCE_INLINE_ StoryScript() {}
};


#endif //GODOT_MASTER_SCRIPT_H
