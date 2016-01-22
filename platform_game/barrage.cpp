//
// Created by Gen on 2016/1/17.
//

#include "barrage.h"
#include "../../servers/physics_2d_server.h"
#include "character.h"

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
    ObjectTypeDB::bind_method(_MD("_area_inout"),&Bullet::_area_inout);
}

void Bullet::_body_inout(int p_status,const RID& p_body, int p_instance, int p_body_shape,int p_area_shape) {
    if (!live) return;
    bool body_in = p_status==Physics2DServer::AREA_BODY_ADDED;
    if (body_in) {
        ObjectID objid=p_instance;

        Object *obj = ObjectDB::get_instance(objid);
        Node *node = obj ? obj->cast_to<Node>() : NULL;
        owner->hit(this, id, node);
    }
}
void Bullet::_area_inout(int p_status,const RID& p_area, int p_instance, int p_area_shape,int p_self_shape) {
    if (!live) return;
    bool area_in = p_status==Physics2DServer::AREA_BODY_ADDED;
    if (area_in) {
        ObjectID objid=p_instance;

        Object *obj = ObjectDB::get_instance(objid);
        HitArea *area = obj ? obj->cast_to<HitArea>() : NULL;
        if (area) {
            owner->hit(this, id, area);
        }else {
            GrazeArea *grz_area = obj ? obj->cast_to<GrazeArea>() : NULL;
            if (grz_area) {
                owner->graze(this, grz_area);
            }
        }
    }
}

#define DEFAULT_SIZE 8.0

bool Barrage::shape_created = false;
RID Barrage::shape;

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
    bullet->index = max_index++;
    Matrix32 xform = get_global_transform();
    bullet->position = xform.xform(p_position);
    bullet->frame = p_frame;
    bullet->rotation = p_rotation;
    bullet->speed = p_speed;//xform.basis_xform(p_speed);
    bullet->data = customer_data;
    bullet->owner = this;
    bullet->live = true;
    bullet->checker = Physics2DServer::get_singleton()->area_create();
    Physics2DServer::get_singleton()->area_set_space(bullet->checker, get_world_2d()->get_space());
    Physics2DServer::get_singleton()->area_add_shape(bullet->checker, shape);
    Physics2DServer::get_singleton()->area_set_transform(bullet->checker, Matrix32(0, bullet->position));
    Physics2DServer::get_singleton()->area_set_monitor_callback(bullet->checker, bullet, "_body_inout");
    Physics2DServer::get_singleton()->area_set_area_monitor_callback(bullet->checker, bullet, "_area_inout");
    Physics2DServer::get_singleton()->area_set_layer_mask(bullet->checker, _layer_mask);
    Physics2DServer::get_singleton()->area_set_collision_mask(bullet->checker, _collision_mask);
    Physics2DServer::get_singleton()->area_set_monitorable(bullet->checker, true);
    return bullet;
}

void Barrage::graze(Bullet *bullet, GrazeArea* area) {
    area->graze(bullet->checker);
}

void Barrage::hit(Bullet *bullet, int index, Node* target) {
    if (is_inside_tree()) {
        Character *cha = get_character();
        if (!hit_status.is_null() && target) {
            Character *cha_tar = target->cast_to<Character>();
            if (cha_tar) {
                if (cha_tar->attack_by(hit_status->duplicate(), cha)) {
                    if (!explosion_scene.is_null()) {
                        Node *node = explosion_scene->instance();
                        Node2D *node2d = node->cast_to<Node2D>();
                        if (node2d) {
                            cha->get_parent()->add_child(node2d);
                            node2d->set_global_pos(bullet->position);
                        }
                    }
                    kill(bullet, index);
                }
                return;
            }
        }
        if (!explosion_scene.is_null()) {
            Node *node = explosion_scene->instance();
            Node2D *node2d = node->cast_to<Node2D>();
            if (node2d) {
                cha->get_parent()->add_child(node2d);
                node2d->set_global_pos(bullet->position);
            }
        }
        kill(bullet,index);
    }
}

void Barrage::hit(Bullet *bullet, int index, HitArea *target) {
    if (is_inside_tree()) {
        Character *cha = get_character();
        if (!hit_status.is_null()) {
            Ref<HitStatus> hs = hit_status->duplicate(true)->cast_to<HitStatus>();
            Vector2 force = hs->get_force();
            if (bullet->speed.x > 0)
                force.x =  Math::abs(force.x);
            else if (bullet->speed.x < 0)
                force.x = -Math::abs(force.x);
            hs->set_force(force);
            hs->set_velocity(force);
            hs->set_stun_velocity(Vector2(force.x, 0));
            if (target->attack_by(hs, cha)) {
                if (!explosion_scene.is_null()) {
                    Node *node = explosion_scene->instance();
                    Node2D *node2d = node->cast_to<Node2D>();
                    if (node2d) {
                        cha->get_parent()->add_child(node2d);
                        node2d->set_global_pos(bullet->position);
                    }
                }
                kill(bullet, index);
            }
            return;
        }
        if (!explosion_scene.is_null() && cha) {
            Node *node = explosion_scene->instance();
            Node2D *node2d = node->cast_to<Node2D>();
            if (node2d) {
                cha->get_parent()->add_child(node2d);
                node2d->set_global_pos(bullet->position);
            }
        }
        kill(bullet,index);
    }
}

void Barrage::kill(Bullet *bullet, int index) {
    bullet->live = false;
    dead_cache.push_back(index);
    Physics2DServer::get_singleton()->area_set_layer_mask(bullet->checker, 0);
    Physics2DServer::get_singleton()->area_set_collision_mask(bullet->checker, 0);
}

void Barrage::clear() {
    for (int i = 0, t = bullets.size(); i < t; ++i) {
        kill(bullets[i], i);
    }
}

void Barrage::_fixed_process_bullets(float delta_time) {
    ScriptInstance *instance = get_script_instance();
    for (int i = 0, t = bullets.size(); i < t; ++i) {
        Bullet *bullet = bullets[i];
        if (bullet->live) {
            _process_bullet(bullet, delta_time);
            if (instance) {
                Variant v1(bullet);
                Variant v2(delta_time);
                const Variant* ptr[2]={&v1, &v2};
                instance->call_multilevel(StringName("_process_bullet"),ptr,2);
            }
        }
    }
}

void Barrage::_process_and_draw_bullets(float delta_time) {
    if (!texture.is_null()) {
        RID ci=get_canvas_item();
        Size2 size = texture->get_size();
        Rect2 vis = get_viewport_transform().xform_inv(get_viewport_rect());
        size = Size2(size.x / h_frames, size.y / v_frames);
        Matrix32 m = get_global_transform().affine_inverse();
        for (int i = 0, t = bullets.size(); i < t; ++i) {
            Bullet *bullet = bullets[i];
            if (bullet->live) {
                Matrix32 xform;

                if (gravity != Vector2()) bullet->speed += gravity;
                bullet->position += bullet->speed*delta_time;
                if (vis.has_point(bullet->position)) {

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
                }else {
                    kill(bullet, i);
                }
            }
        }
    }
}

void Barrage::_update_character() {
    if (is_inside_tree()) {
        if (character_path != NodePath() && has_node(character_path)) {
            Node *node = get_node(character_path);
            Character *character = node->cast_to<Character>();
            if (character) {
                character_id = character->get_instance_ID();
            }else {
                character_id = 0;
            }
        }else {
            character_id = 0;
        }
    }else {
        character_id = 0;
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
            _update_character();
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
    max_index=0;
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
    ObjectTypeDB::bind_method(_MD("create_bullet:Bullet", "position", "rotation", "speed", "frame", "customer_dataa"), &Barrage::create_bullet, DEFVAL(NULL));

    ObjectTypeDB::bind_method(_MD("set_texture", "texture:Texture"), &Barrage::set_texture);
    ObjectTypeDB::bind_method(_MD("get_texture:Texture"), &Barrage::get_texture);

    ObjectTypeDB::bind_method(_MD("set_layer_mask", "layer_mask"), &Barrage::set_layer_mask);
    ObjectTypeDB::bind_method(_MD("get_layer_mask"), &Barrage::get_layer_mask);

    ObjectTypeDB::bind_method(_MD("set_collision_mask", "collision_mask"), &Barrage::set_collision_mask);
    ObjectTypeDB::bind_method(_MD("get_collision_mask"), &Barrage::get_collision_mask);

    ObjectTypeDB::bind_method(_MD("set_character_path", "character_path"), &Barrage::set_character_path);
    ObjectTypeDB::bind_method(_MD("get_character_path"), &Barrage::get_character_path);
    ObjectTypeDB::bind_method(_MD("get_character"), &Barrage::get_character);

    ObjectTypeDB::bind_method(_MD("set_hit_status", "hit_status"), &Barrage::set_hit_status);
    ObjectTypeDB::bind_method(_MD("get_hit_status"), &Barrage::get_hit_status);

    ObjectTypeDB::bind_method(_MD("set_explosion_scene", "explosion_scene"), &Barrage::set_explosion_scene);
    ObjectTypeDB::bind_method(_MD("get_explosion_scene"), &Barrage::get_explosion_scene);

    ObjectTypeDB::bind_method(_MD("set_gravity", "gravity"), &Barrage::set_gravity);
    ObjectTypeDB::bind_method(_MD("get_gravity"), &Barrage::get_gravity);

    ObjectTypeDB::bind_method(_MD("clear"), &Barrage::clear);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), _SCS("set_texture"), _SCS("get_texture"));

    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "character_path"), _SCS("set_character_path"), _SCS("get_character_path"));
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "hit_status", PROPERTY_HINT_RESOURCE_TYPE, "HitStatus"), _SCS("set_hit_status"), _SCS("get_hit_status"));
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "explosion_scene", PROPERTY_HINT_RESOURCE_TYPE, "PackedScene"), _SCS("set_explosion_scene"), _SCS("get_explosion_scene"));
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "gravity"), _SCS("set_gravity"), _SCS("get_gravity"));

    ADD_PROPERTY(PropertyInfo(Variant::INT, "layers", PROPERTY_HINT_ALL_FLAGS), _SCS("set_layer_mask"), _SCS("get_layer_mask"));
    ADD_PROPERTY(PropertyInfo(Variant::INT, "masks", PROPERTY_HINT_ALL_FLAGS), _SCS("set_collision_mask"), _SCS("get_collision_mask"));

    BIND_VMETHOD(MethodInfo("_process_bullet", PropertyInfo(Variant::OBJECT, "bullet", PROPERTY_HINT_RESOURCE_TYPE, "Bullet"), PropertyInfo(Variant::REAL, "delta")));
}
//==================ShootBarrage==========================


void ShootBarrage::_bind_methods() {

    ObjectTypeDB::bind_method(_MD("set_speed", "speed"), &ShootBarrage::set_speed);
    ObjectTypeDB::bind_method(_MD("get_speed"), &ShootBarrage::get_speed);

    ObjectTypeDB::bind_method(_MD("set_radius", "radius"), &ShootBarrage::set_radius);
    ObjectTypeDB::bind_method(_MD("get_radius"), &ShootBarrage::get_radius);

    ObjectTypeDB::bind_method(_MD("set_body_size", "body_size"), &ShootBarrage::set_body_size);
    ObjectTypeDB::bind_method(_MD("get_body_size"), &ShootBarrage::get_body_size);

    ObjectTypeDB::bind_method(_MD("set_bullet_scale", "bullet_scale"), &ShootBarrage::set_bullet_scale);
    ObjectTypeDB::bind_method(_MD("get_bullet_scale"), &ShootBarrage::get_bullet_scale);

    ADD_PROPERTY(PropertyInfo(Variant::REAL, "speed"), _SCS("set_speed"), _SCS("get_speed"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "radius"), _SCS("set_radius"), _SCS("get_radius"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "body_size"), _SCS("set_body_size"), _SCS("get_body_size"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "bullet_scale"), _SCS("set_bullet_scale"), _SCS("get_bullet_scale"));
}

//==================ScatterBarrage==========================

void ScatterBarrage::shoot(Point2 target, int count, int frame) {
    Vector2 v2 = target - get_global_pos();
    float in = Math::deg2rad(angle_interval);
    float angle = Math::atan2(v2.y, v2.x);
    float start = angle - in*((count-1)/2.0);
    if (distance_interval != 0) {
        Vector2 step(Math::cos(angle+Math_PI/2)*distance_interval, Math::sin(angle+Math_PI/2)*distance_interval);
        Vector2 start_pos = -step*((count-1)/2.0);

        for (int i = 0; i < count; ++i) {
            float alpha = start + in*i;
            Vector2 vn(Math::cos(alpha), Math::sin(alpha));
            Bullet *bullet = create_bullet(vn*radius + (start_pos+i*step), alpha, vn*speed, frame, NULL);
            bullet->set_body_size(body_size);
            bullet->set_scale(bullet_scale);
        }
    }else {
        for (int i = 0; i < count; ++i) {
            float alpha = start + in*i;
            Vector2 vn(Math::cos(alpha), Math::sin(alpha));
            Bullet *bullet = create_bullet(vn*radius, alpha, vn*speed, frame, NULL);
            bullet->set_body_size(body_size);
            bullet->set_scale(bullet_scale);
        }
    }
}

void ScatterBarrage::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("set_interval", "interval"), &ScatterBarrage::set_interval);
    ObjectTypeDB::bind_method(_MD("get_interval"), &ScatterBarrage::get_interval);

    ObjectTypeDB::bind_method(_MD("set_distance_interval", "interval"), &ScatterBarrage::set_distance_interval);
    ObjectTypeDB::bind_method(_MD("get_distance_interval"), &ScatterBarrage::get_distance_interval);

    ObjectTypeDB::bind_method(_MD("shoot", "target", "count", "frame"), &ScatterBarrage::shoot);

    ADD_PROPERTY(PropertyInfo(Variant::REAL, "interval"), _SCS("set_interval"), _SCS("get_interval"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "distance_interval"), _SCS("set_distance_interval"), _SCS("get_distance_interval"));
}

//==================RandomBarrage==========================

void RandomBarrage::_notification(int p_notification) {
    switch (p_notification) {

        case NOTIFICATION_FIXED_PROCESS: {
            if (shoot_count > 0) {
                if (frame_count <= 0) {
                    shoot_count -= 1;
                    frame_count = frame_interval;
                    for (int i = 0; i < bullet_once_count; ++i) {
                        make_bullet();
                    }
                }else {
                    frame_count-=1;
                }
            }
        }
    }
}

float RandomBarrage::randf() {
    return Math::randf()*2-1;
}

Bullet *RandomBarrage::make_bullet() {

    float angle = Math::deg2rad(shoot_angle+angle_range*randf());
    Bullet *bullet = create_bullet(Vector2(pos_range.x*randf(), pos_range.y*randf()), angle, Vector2(Math::cos(angle), Math::sin(angle))*(speed+speed_range*randf()), sprite_frame);
    bullet->set_body_size(body_size);
    bullet->set_scale(bullet_scale);
    return bullet;
}

void RandomBarrage::shoot(float angle, int frame) {
    shoot_angle = angle;
    shoot_count = shoot_time;
    frame_count = 0;
    sprite_frame = frame;
}

void RandomBarrage::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("set_pos_range", "pos_range"), &RandomBarrage::set_pos_range);
    ObjectTypeDB::bind_method(_MD("get_pos_range"), &RandomBarrage::get_pos_range);

    ObjectTypeDB::bind_method(_MD("set_angle_range", "angle_range"), &RandomBarrage::set_angle_range);
    ObjectTypeDB::bind_method(_MD("get_angle_range"), &RandomBarrage::get_angle_range);

    ObjectTypeDB::bind_method(_MD("set_speed_range", "speed_range"), &RandomBarrage::set_speed_range);
    ObjectTypeDB::bind_method(_MD("get_speed_range"), &RandomBarrage::get_speed_range);

    ObjectTypeDB::bind_method(_MD("set_shoot_time", "shoot_time"), &RandomBarrage::set_shoot_time);
    ObjectTypeDB::bind_method(_MD("get_shoot_time"), &RandomBarrage::get_shoot_time);

    ObjectTypeDB::bind_method(_MD("set_frame_interval", "frame_interval"), &RandomBarrage::set_frame_interval);
    ObjectTypeDB::bind_method(_MD("get_frame_interval"), &RandomBarrage::get_frame_interval);

    ObjectTypeDB::bind_method(_MD("shoot", "angle", "frame"), &RandomBarrage::shoot);

    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "pos_range"), _SCS("set_pos_range"), _SCS("get_pos_range"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "angle_range"), _SCS("set_angle_range"), _SCS("get_angle_range"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "speed_range"), _SCS("set_speed_range"), _SCS("get_speed_range"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "shoot_time"), _SCS("set_shoot_time"), _SCS("get_shoot_time"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "frame_interval"), _SCS("set_frame_interval"), _SCS("get_frame_interval"));
}