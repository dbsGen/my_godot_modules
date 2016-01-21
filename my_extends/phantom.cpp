//
// Created by Gen on 16/1/21.
//

#include "phantom.h"
#include "../../servers/visual_server.h"

void Phantom::_update_target() {
    if  (is_inside_tree()) {
        if (target_path != NodePath() && has_node(target_path))
            target = get_node(target_path)->cast_to<Sprite>();
        else target = NULL;
    }else {
        target = NULL;
    }
}

void Phantom::make_item() {
    Item item;
    item.life = life_frame;
    item.position = target->get_global_pos();
    item.offset = target->get_offset();
    Matrix32 m = target->get_global_transform();
    item.scale = m.get_scale();
    item.rotation = m.get_rotation();
    if (m[0][0] * m[1][1] < 0) {
        item.scale.x = -item.scale.x;
        item.rotation = Math_PI*2-item.rotation;
    }
    item.texture = target->get_texture();
    if (target->is_region()) {
        item.src_rect = target->get_region_rect();
    }else {
        Size2 size = item.texture->get_size();
        size.width/=target->get_hframes();
        size.height/=target->get_vframes();
        float left = (target->get_frame()%target->get_hframes())*size.width;
        float top = (target->get_frame()/target->get_hframes())*size.height;
        item.src_rect = Rect2(left, top, size.width, size.height);
    }
    if (target->is_centered())
        item.offset -= item.src_rect.size/2;

    items.push(item);
}

void Phantom::_update_size() {
    items.alloc(life_frame/(frame_interval+1) + 1);
}

void Phantom::_update_fixed_frame() {
    for (int i = items.size() - 1; i >= 0; --i) {
        Item &item = items[i];
        if (item.life > 0) {
            item.life -= 1;
        }else break;
    }
    if (phantom_enable && target) {
        if (frame_count <= 0) {
            frame_count = frame_interval;
            make_item();
        }else {
            frame_count -= 1;
        }
    }
}

void Phantom::_update_and_draw() {
    RID ci = get_canvas_item();
    Matrix32 m = get_global_transform().affine_inverse();
    int t = items.size(), count = 0;
    Item **_tmp = (Item **)memalloc(sizeof(Phantom::Item*) * t);

    for (int i = t - 1; i >= 0; --i) {
        Item &item = items[i];
        if (item.life > 0) {
            if (!item.texture.is_null()) {
                _tmp[count++] = &item;
            }
        }else
            break;
    }
    for (int j = count-1; j >= 0; --j) {
        Item *item = _tmp[j];
        Matrix32 xform;
        xform.set_rotation(item->rotation);
        xform.elements[2]+=item->position;
        xform = m*xform;
        xform.scale_basis(item->scale);
        VisualServer::get_singleton()->canvas_item_add_set_transform(ci, xform);
        float p = item->life/(float)life_frame;
        Color color(1,1,1,p);
        if (!color_ramp.is_null())
            color = color_ramp->get_color_at_offset(1-p);
        item->texture->draw_rect_region(ci, Rect2(item->offset,item->src_rect.size), item->src_rect, color);
    }
}

void Phantom::_notification(int p_notification) {
    switch (p_notification) {
        case NOTIFICATION_ENTER_TREE: {
            _update_target();
        } break;
        case NOTIFICATION_FIXED_PROCESS: {
            _update_fixed_frame();
        } break;
        case NOTIFICATION_PROCESS: {
            update();
        } break;
        case NOTIFICATION_DRAW: {
            _update_and_draw();
        } break;
    }
}

void Phantom::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("set_target_path", "target_path"), &Phantom::set_target_path);
    ObjectTypeDB::bind_method(_MD("get_target_path"), &Phantom::get_target_path);
    ObjectTypeDB::bind_method(_MD("get_target:Sprite"), &Phantom::get_target);

    ObjectTypeDB::bind_method(_MD("set_color_ramp", "color_ramp:ColorRamp"), &Phantom::set_color_ramp);
    ObjectTypeDB::bind_method(_MD("get_color_ramp"), &Phantom::get_color_ramp);

    ObjectTypeDB::bind_method(_MD("set_life_frame", "life_frame"), &Phantom::set_life_frame);
    ObjectTypeDB::bind_method(_MD("get_life_frame"), &Phantom::get_life_frame);

    ObjectTypeDB::bind_method(_MD("set_frame_interval", "frame_interval"), &Phantom::set_frame_interval);
    ObjectTypeDB::bind_method(_MD("get_frame_interval"), &Phantom::get_frame_interval);

    ObjectTypeDB::bind_method(_MD("set_phantom_enable", "enable"), &Phantom::set_phantom_enable);
    ObjectTypeDB::bind_method(_MD("get_phantom_enable"), &Phantom::get_phantom_enable);

    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "target_path"), _SCS("set_target_path"), _SCS("get_target_path"));
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "color_ramp", PROPERTY_HINT_RESOURCE_TYPE, "ColorRamp"), _SCS("set_color_ramp"), _SCS("get_color_ramp"));
    ADD_PROPERTY(PropertyInfo(Variant::INT, "life_time"), _SCS("set_life_frame"), _SCS("get_life_frame"));
    ADD_PROPERTY(PropertyInfo(Variant::INT, "frame_interval"), _SCS("set_frame_interval"), _SCS("get_frame_interval"));
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "phantom_enable"), _SCS("set_phantom_enable"), _SCS("get_phantom_enable"));


}