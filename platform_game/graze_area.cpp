//
// Created by Gen on 2016/1/21.
//

#include "graze_area.h"
#include "../../servers/physics_2d_server.h"

void GrazeArea::graze(RID area) {
    if (!graze_enable) return;
    uint32_t id = area.get_id();
    if (grazed.find(id) < 0) {
        Vector2 pos = Physics2DServer::get_singleton()->area_get_transform(area).get_origin();
        _on_graze(pos);
        if (get_script_instance()) {
            Variant v1(pos);
            const Variant* ptr[1]={&v1};
            get_script_instance()->call_multilevel(StringName("_on_graze"),ptr,1);
        }
        emit_signal(StringName("_on_graze"), pos);
    }
}

void GrazeArea::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_graze_enable", "graze_enable"), &GrazeArea::set_graze_enable);
    ClassDB::bind_method(D_METHOD("get_graze_enable"), &GrazeArea::get_graze_enable);

    ClassDB::bind_method(D_METHOD("graze", "area:RID"), &GrazeArea::graze);

    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "graze_enable"), "set_graze_enable", "get_graze_enable");

    BIND_VMETHOD(MethodInfo("_on_graze", PropertyInfo(Variant::VECTOR2, "pos")));

    ADD_SIGNAL(MethodInfo("_on_graze", PropertyInfo(Variant::VECTOR2, "pos")));
}