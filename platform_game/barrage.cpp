//
// Created by Gen on 2016/1/17.
//

#include "barrage.h"
#include "../../servers/physics_2d_server.h"

//=========================Bullet================

void Bullet::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("set_frame", "frame"), &Bullet::set_frame);
    ObjectTypeDB::bind_method(_MD("get_frame"), &Bullet::get_frame);

    ObjectTypeDB::bind_method(_MD("set_speed", "speed"), &Bullet::set_speed);
    ObjectTypeDB::bind_method(_MD("get_speed"), &Bullet::get_speed);

    ObjectTypeDB::bind_method(_MD("set_rotation", "rotation"), &Bullet::set_rotation);
    ObjectTypeDB::bind_method(_MD("get_rotation"), &Bullet::get_rotation);

    ObjectTypeDB::bind_method(_MD("set_scale", "scale"), &Bullet::set_scale);
    ObjectTypeDB::bind_method(_MD("get_scale"), &Bullet::get_scale);

    ObjectTypeDB::bind_method(_MD("set_position", "position"), &Bullet::set_position);
    ObjectTypeDB::bind_method(_MD("get_position"), &Bullet::get_position);

    ObjectTypeDB::bind_method(_MD("set_data", "data:Variant"), &Bullet::set_data);
    ObjectTypeDB::bind_method(_MD("get_data"), &Bullet::get_data);

    ObjectTypeDB::bind_method(_MD("set_body_size", "body_size"), &Bullet::set_body_size);
    ObjectTypeDB::bind_method(_MD("get_body_size"), &Bullet::get_body_size);

    ObjectTypeDB::bind_method(_MD("_body_inout"),&Bullet::_body_inout);
}

void Bullet::_body_inout(int p_status,const RID& p_body, int p_instance, int p_body_shape,int p_area_shape) {
    if (!live) return;
    bool body_in = p_status==Physics2DServer::AREA_BODY_ADDED;
    if (body_in) {
        owner->kill(this, id);
    }
}

#define DEFAULT_SIZE 8.0

Bullet *Barrage::create_bullet(Point2 p_position, float p_rotation, Vector2 p_speed, int p_frame,  const Variant &customer_data) {
    if (!shape_created)
        return NULL;
    int index = bullets.size();

    Bullet *bullet;
    if (dead_cache.size() > 0) {
        int idx = dead_cache.size()-1;
        bullet = bullets[dead_cache[idx]];
        dead_cache.remove(idx);
    }else {
        bullets.push_back(memnew(Bullet));
        bullet = bullets[bullets.size()-1];
        bullet->id = index;
    }
    Matrix32 xform = get_global_transform();
    bullet->position = xform.xform(p_position);
    bullet->frame = p_frame;
    bullet->rotation = p_rotation;
    bullet->speed = xform.basis_xform(p_speed);
    bullet->data = customer_data;
    bullet->owner = this;
    bullet->live = true;
    bullet->checker = Physics2DServer::get_singleton()->area_create();
    Physics2DServer::get_singleton()->area_set_space(bullet->checker, get_world_2d()->get_space());
    Physics2DServer::get_singleton()->area_add_shape(bullet->checker, shape);
    Physics2DServer::get_singleton()->area_set_transform(bullet->checker, Matrix32(0, bullet->position));
    Physics2DServer::get_singleton()->area_set_monitor_callback(bullet->checker, bullet, "_body_inout");
    Physics2DServer::get_singleton()->area_set_layer_mask(bullet->checker, _layer_mask);
    Physics2DServer::get_singleton()->area_set_collision_mask(bullet->checker, _collision_mask);
    Physics2DServer::get_singleton()->area_set_monitorable(bullet->checker, true);
    return bullet;
}

void Barrage::kill(Bullet *bullet, int index) {
    bullet->live = false;
    dead_cache.push_back(index);
    Physics2DServer::get_singleton()->area_set_layer_mask(bullet->checker, 0);
    Physics2DServer::get_singleton()->area_set_collision_mask(bullet->checker, 0);
}

void Barrage::_fixed_process_bullets(float delta_time) {
    for (int i = 0, t = bullets.size(); i < t; ++i) {
        Bullet *bullet = bullets[i];
        if (bullet->live) {
            _process_bullet(bullet, delta_time);
        }
    }
}

void Barrage::_process_and_draw_bullets(float delta_time) {
    if (!texture.is_null()) {
        RID ci=get_canvas_item();
        Size2 size = texture->get_size();
        size = Size2(size.x / h_frames, size.y / v_frames);
        Matrix32 m = get_global_transform().affine_inverse();
        for (int i = 0, t = bullets.size(); i < t; ++i) {
            Bullet *bullet = bullets[i];
            if (bullet->live) {
                Matrix32 xform;

                bullet->position += bullet->speed*delta_time;
                if (bullet->rotation) {

                    xform.set_rotation(bullet->rotation);
                    xform.translate(-size/2.0);
                    xform.elements[2]+=bullet->position;
                } else {
                    xform.elements[2]=-size/2.0;
                    xform.elements[2]+=bullet->position;
                }
                xform = m * xform;
                xform.scale_basis(Size2(bullet->scale,bullet->scale));
                VisualServer::get_singleton()->canvas_item_add_set_transform(ci,xform);

                Matrix32 matrix = Matrix32(0, bullet->position);
                float scale = bullet->body_size/DEFAULT_SIZE;
                matrix.scale_basis(Size2(scale, scale));
                Physics2DServer::get_singleton()->area_set_transform(bullet->checker, matrix);
                texture->draw_rect_region(ci, Rect2(Point2(),size), Rect2(Point2(size.x * (bullet->frame % h_frames), size.y * (bullet->frame / h_frames)), size), Color(1,1,1,1));
            }
        }
    }
}

void Barrage::_notification(int p_notification) {
    switch (p_notification) {
        case NOTIFICATION_ENTER_TREE: {
            if (!shape_created) {
                shape = Physics2DServer::get_singleton()->shape_create(Physics2DServer::SHAPE_CIRCLE);
                Physics2DServer::get_singleton()->shape_set_data(shape, DEFAULT_SIZE);
                shape_created = true;
            }
        } break;
        case NOTIFICATION_PROCESS: {
            update();
        } break;
        case NOTIFICATION_FIXED_PROCESS: {
            _fixed_process_bullets(get_fixed_process_delta_time());
        } break;
        case NOTIFICATION_DRAW: {
            _process_and_draw_bullets(get_process_delta_time());
        } break;
    }
}

Barrage::Barrage() {
    h_frames = 1;
    v_frames = 1;
    _layer_mask = 0;
    _collision_mask = 0;
    shape_created = false;
    set_fixed_process(true);
    set_process(true);
}

Barrage::~Barrage() {
    for (int i = 0, t = bullets.size(); i < t; ++i) {
        Bullet *b = bullets[i];
        Physics2DServer::get_singleton()->free(b->checker);
        memdelete(b);
    }
}

void Barrage::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("create_bullet:Bullet", "position", "rotation", "speed", "frame", "customer_dataa"), &Barrage::create_bullet);

    ObjectTypeDB::bind_method(_MD("set_texture", "texture:Texture"), &Barrage::set_texture);
    ObjectTypeDB::bind_method(_MD("get_texture:Texture"), &Barrage::get_texture);

    ObjectTypeDB::bind_method(_MD("set_layer_mask", "layer_mask"), &Barrage::set_layer_mask);
    ObjectTypeDB::bind_method(_MD("get_layer_mask"), &Barrage::get_layer_mask);

    ObjectTypeDB::bind_method(_MD("set_collision_mask", "collision_mask"), &Barrage::set_collision_mask);
    ObjectTypeDB::bind_method(_MD("get_collision_mask"), &Barrage::get_collision_mask);


    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), _SCS("set_texture"), _SCS("get_texture"));

    ADD_PROPERTY(PropertyInfo(Variant::INT, "layers", PROPERTY_HINT_ALL_FLAGS), _SCS("set_layer_mask"), _SCS("get_layer_mask"));
    ADD_PROPERTY(PropertyInfo(Variant::INT, "masks", PROPERTY_HINT_ALL_FLAGS), _SCS("set_collision_mask"), _SCS("get_collision_mask"));

}

//==================ScatterBarrage==========================

void ScatterBarrage::shoot(Point2 target, int count) {

}