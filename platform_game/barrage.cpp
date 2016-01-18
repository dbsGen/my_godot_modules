//
// Created by Gen on 2016/1/17.
//

#include "barrage.h"
#include "../../servers/physics_2d_server.h"

#define DEFAULT_SIZE 8

int Barrage::create_bullet(Point2 p_position, float p_rotation, Vector2 p_speed, int p_frame,  const Variant &customer_data) {
    if (!shape_created)
        return -1;
    int index = bullets.size();

    Bullet *bullet;
    if (dead_cache.size() > 0) {
        int idx = dead_cache.size()-1;
        bullet = &bullets[dead_cache[idx]];
        dead_cache.remove(idx);
    }else {
        bullets.push_back(Bullet());
        bullet = &bullets[bullets.size()-1];
    }
    bullet->matrix = Matrix32(p_rotation, p_position)*get_global_transform();
    bullet->frame = p_frame;
    bullet->id = index;
    bullet->speed = p_speed;
    bullet->data = customer_data;
    bullet->checker = Physics2DServer::get_singleton()->area_create();
    Physics2DServer::get_singleton()->area_set_space(bullet->checker, get_world_2d()->get_space());
    Physics2DServer::get_singleton()->area_add_shape(bullet->checker, shape);
    Physics2DServer::get_singleton()->area_set_transform(bullet->checker, bullet->matrix);
    Physics2DServer::get_singleton()->area_set_monitor_callback(bullet->checker, this, "_body_inout");
    Physics2DServer::get_singleton()->area_set_layer_mask(bullet->checker, _layer_mask);
    Physics2DServer::get_singleton()->area_set_collision_mask(bullet->checker, _collision_mask);
    return index;
}

void Barrage::kill(Bullet &bullet, int index) {
    bullet.live = false;
    dead_cache.push_back(index);
    Physics2DServer::get_singleton()->area_set_layer_mask(bullet.checker, 0);
    Physics2DServer::get_singleton()->area_set_collision_mask(bullet.checker, 0);
}

void Barrage::_body_inout(int p_status,const RID& p_body, int p_instance, int p_body_shape,int p_area_shape) {
    bool body_in = p_status==Physics2DServer::AREA_BODY_ADDED;
    print_line(body_in?"body in":"body out");
    if (body_in) {
        for (int i = 0, t = bullets.size(); i < t; ++i) {
            Bullet &bullet = bullets[i];
            if (bullet.checker == p_body) {
                kill(bullet, i);
                break;
            }
        }
    }
}

void Barrage::_fixed_process_bullets(float delta_time) {
    for (int i = 0, t = bullets.size(); i < t; ++i) {
        Bullet &bullet = bullets[i];
        if (bullet.live) {
            _process_bullet(&bullet, delta_time);
        }
    }
}

void Barrage::_process_and_draw_bullets(float delta_time) {
    if (!texture.is_null()) {
        RID ci=get_canvas_item();
        Size2 size = texture->get_size();
        Matrix32 m = get_global_transform().affine_inverse();
        for (int i = 0, t = bullets.size(); i < t; ++i) {
            Bullet &bullet = bullets[i];
            if (bullet.live) {
                bullet.matrix.translate(bullet.speed * delta_time);
                Physics2DServer::get_singleton()->area_set_transform(bullet.checker, bullet.matrix);
                VisualServer::get_singleton()->canvas_item_add_set_transform(ci,bullet.matrix*m);
                texture->draw_rect_region(ci, Rect2(Point2(),size), Rect2(Point2(), size), Color(1,1,1,1));
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

void Barrage::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("create_bullet", "position", "rotation", "speed", "frame", "customer_dataa"), &Barrage::create_bullet);

    ObjectTypeDB::bind_method(_MD("set_texture", "texture:Texture"), &Barrage::set_texture);
    ObjectTypeDB::bind_method(_MD("get_texture:Texture"), &Barrage::get_texture);

    ObjectTypeDB::bind_method(_MD("set_layer_mask", "layer_mask"), &Barrage::set_layer_mask);
    ObjectTypeDB::bind_method(_MD("get_layer_mask"), &Barrage::get_layer_mask);

    ObjectTypeDB::bind_method(_MD("set_collision_mask", "layer_mask"), &Barrage::set_collision_mask);
    ObjectTypeDB::bind_method(_MD("get_collision_mask"), &Barrage::get_collision_mask);

    ObjectTypeDB::bind_method(_MD("_body_inout"),&Barrage::_body_inout);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), _SCS("set_texture"), _SCS("get_texture"));

    ADD_PROPERTY(PropertyInfo(Variant::INT, "layer", PROPERTY_HINT_ALL_FLAGS), _SCS("set_layer_mask"), _SCS("get_layer_mask"));
    ADD_PROPERTY(PropertyInfo(Variant::INT, "mask", PROPERTY_HINT_ALL_FLAGS), _SCS("set_collision_mask"), _SCS("get_collision_mask"));

}

//==================ScatterBarrage==========================

void ScatterBarrage::shoot(Point2 target, int count) {

}