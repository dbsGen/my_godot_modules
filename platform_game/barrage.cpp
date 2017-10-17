//
// Created by Gen on 2016/1/17.
//

#include "barrage.h"
#include "../../servers/physics_2d_server.h"
#include "character.h"

//=========================Bullet================

void Bullet::_process(float delta_time) {
    total_time += delta_time;
    // rotation = start_rotation + total_time * rotation_accel;
    // if (rotation_accel == 0) {
    //     float cosr = cos(start_rotation);
    //     float sinr = sin(start_rotation);
    //     position.x = start_position.x + accel * cosr * total_time * total_time / 2 + speed * cosr * total_time;
    //     position.y = start_position.y + accel * sinr * total_time * total_time / 2 + speed * sinr * total_time;
    // }else {
    //     float s = accel * total_time + speed;
    //     float cx = -speed/rotation_accel*sin(start_rotation) - accel/rotation_accel/rotation_accel*cos(start_rotation);
    //     float cy = speed/rotation_accel*cos(start_rotation) - accel/rotation_accel/rotation_accel*sin(start_rotation);
    //     float cosr = cos(rotation);
    //     float sinr = sin(rotation);
    //     position.x = start_position.x + s/rotation_accel*sinr + accel/rotation_accel/rotation_accel * cosr + cx;
    //     position.y = start_position.y - s/rotation_accel*cosr + accel/rotation_accel/rotation_accel * sinr + cy;
    // }

    speed += accel * delta_time;
    float move = speed * delta_time;
    if (!target_set) {
        rotation += rotation_accel * delta_time;
        position.x += cos(rotation) * move;
        position.y += sin(rotation) * move;
    }else {
        Vector2 vec2(target_position - position);
        if (vec2.length() < move) {
            move = vec2.length();
        }
        position += vec2.normalized() * move;
    }
}

void Bullet::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_frame", "frame"), &Bullet::set_frame);
    ClassDB::bind_method(D_METHOD("get_frame"), &Bullet::get_frame);

    ClassDB::bind_method(D_METHOD("set_speed", "speed"), &Bullet::set_speed);
    ClassDB::bind_method(D_METHOD("get_speed"), &Bullet::get_speed);

    ClassDB::bind_method(D_METHOD("set_rotation", "rotation"), &Bullet::set_rotation);
    ClassDB::bind_method(D_METHOD("get_rotation"), &Bullet::get_rotation);

    ClassDB::bind_method(D_METHOD("set_scale", "scale"), &Bullet::set_scale);
    ClassDB::bind_method(D_METHOD("get_scale"), &Bullet::get_scale);

    ClassDB::bind_method(D_METHOD("set_position", "position"), &Bullet::set_position);
    ClassDB::bind_method(D_METHOD("get_position"), &Bullet::get_position);

    ClassDB::bind_method(D_METHOD("set_data", "data:Variant"), &Bullet::set_data);
    ClassDB::bind_method(D_METHOD("get_data"), &Bullet::get_data);

    ClassDB::bind_method(D_METHOD("set_body_size", "body_size"), &Bullet::set_body_size);
    ClassDB::bind_method(D_METHOD("get_body_size"), &Bullet::get_body_size);

    ClassDB::bind_method(D_METHOD("_body_inout"),&Bullet::_body_inout);
    ClassDB::bind_method(D_METHOD("_area_inout"),&Bullet::_area_inout);

    ClassDB::bind_method(D_METHOD("set_target_set", "target_set"), &Bullet::set_target_set);
    ClassDB::bind_method(D_METHOD("get_target_set"), &Bullet::get_target_set);
    
    ClassDB::bind_method(D_METHOD("set_target_position", "target_position"), &Bullet::set_target_position);
    ClassDB::bind_method(D_METHOD("get_target_position"), &Bullet::set_target_position);
    
    ClassDB::bind_method(D_METHOD("set_accel", "accel"), &Bullet::set_accel);
    ClassDB::bind_method(D_METHOD("get_accel"), &Bullet::get_accel);
    
    ClassDB::bind_method(D_METHOD("set_rotation_accel", "accel"), &Bullet::set_rotation_accel);
    ClassDB::bind_method(D_METHOD("get_rotation_accel"), &Bullet::get_rotation_accel);
}

void Bullet::_body_inout(int p_status,const RID& p_body, int p_instance, int p_body_shape,int p_area_shape) {
    if (!live) return;
    bool body_in = p_status==Physics2DServer::AREA_BODY_ADDED;
    if (body_in) {
        ObjectID objid=p_instance;

        Object *obj = ObjectDB::get_instance(objid);
        Node *node = obj ? Object::cast_to<Node>(obj) : NULL;
        owner->hit(this, id, node);
    }
}
void Bullet::_area_inout(int p_status,const RID& p_area, int p_instance, int p_area_shape,int p_self_shape) {
    if (!live) return;
    bool area_in = p_status==Physics2DServer::AREA_BODY_ADDED;
    if (area_in) {
        ObjectID objid=p_instance;

        Object *obj = ObjectDB::get_instance(objid);
        HitArea *area = obj ? Object::cast_to<HitArea>(obj) : NULL;
        if (area) {
            owner->hit(this, id, area);
        }else {
            GrazeArea *grz_area = obj ? Object::cast_to<GrazeArea>(obj) : NULL;
            if (grz_area) {
                owner->graze(this, grz_area);
            }
        }
    }
}

#define DEFAULT_SIZE 8.0

bool Barrage::shape_created = false;
RID Barrage::shape;

Bullet *Barrage::create_bullet(const Point2 &p_position) {
    if (!shape_created)
        return NULL;
    int index = bullets.size();

    Bullet *bullet;
    if (dead_cache.size() > 0) {
        int idx = dead_cache.size()-1;
        bullet = *bullets[dead_cache[idx]];
        dead_cache.remove(idx);
    }else {
        bullets.push_back(memnew(Bullet));
        bullet = *bullets[index];
        bullet->id = index;
        bullet->checker = Physics2DServer::get_singleton()->area_create();
        Physics2DServer::get_singleton()->area_set_space(bullet->checker, get_world_2d()->get_space());
        Physics2DServer::get_singleton()->area_add_shape(bullet->checker, shape);
    }
    bullet->index = max_index++;
    Transform2D xform = get_global_transform();
    bullet->start_position = xform.xform(p_position);
    bullet->position = xform.xform(p_position);
    bullet->owner = this;
    bullet->live = true;
    bullet->total_time = 0;
    Physics2DServer::get_singleton()->area_set_transform(bullet->checker, Transform2D(0, bullet->position));
    Physics2DServer::get_singleton()->area_set_monitor_callback(bullet->checker, bullet, "_body_inout");
    Physics2DServer::get_singleton()->area_set_area_monitor_callback(bullet->checker, bullet, "_area_inout");
    Physics2DServer::get_singleton()->area_set_collision_layer(bullet->checker, _layer_mask);
    Physics2DServer::get_singleton()->area_set_collision_mask(bullet->checker, _collision_mask);
    Physics2DServer::get_singleton()->area_set_monitorable(bullet->checker, true);
    return bullet;
}

void Barrage::graze(const Ref<Bullet> &bullet, GrazeArea* area) {
    area->graze(bullet->checker);
}

void Barrage::hit(const Ref<Bullet> &bullet, int index, Node* target) {
    if (is_inside_tree()) {
        Character *cha = get_character();
        if (!hit_status.is_null() && target) {
            Character *cha_tar = Object::cast_to<Character>(target);
            if (cha_tar) {
                if (cha_tar->attack_by(hit_status->duplicate(), cha)) {
                    if (!explosion_scene.is_null()) {
                        Node *node = explosion_scene->instance();
                        Node2D *node2d = Object::cast_to<Node2D>(node);
                        if (node2d) {
                            cha->get_parent()->add_child(node2d);
                            node2d->set_global_position(bullet->position);
                        }
                    }
                    kill(bullet, index);
                }
                return;
            }
        }
        if (!explosion_scene.is_null()) {
            Node *node = explosion_scene->instance();
            Node2D *node2d = Object::cast_to<Node2D>(node);
            if (node2d) {
                cha->get_parent()->add_child(node2d);
                node2d->set_global_position(bullet->position);
            }
        }
        kill(bullet,index);
    }
}

void Barrage::hit(const Ref<Bullet> &bullet, int index, HitArea *target) {
    if (is_inside_tree()) {
        Character *cha = get_character();
        if (!hit_status.is_null()) {
            Ref<HitStatus> hs = hit_status->duplicate(true);
            Vector2 force = hs->get_force();
            float cx = cos(bullet->rotation);
            if (cx > 0)
                force.x =  Math::abs(force.x);
            else if (cx < 0)
                force.x = -Math::abs(force.x);
            hs->set_force(force);
            hs->set_velocity(force);
            hs->set_stun_velocity(Vector2(force.x, 0));
            if (target->attack_by(hs, cha)) {
                if (!explosion_scene.is_null()) {
                    Node *node = explosion_scene->instance();
                    Node2D *node2d = Object::cast_to<Node2D>(node);
                    if (node2d) {
                        cha->get_parent()->add_child(node2d);
                        node2d->set_global_position(bullet->position);
                    }
                }
                kill(bullet, index);
            }
            return;
        }
        if (!explosion_scene.is_null() && cha) {
            Node *node = explosion_scene->instance();
            Node2D *node2d = Object::cast_to<Node2D>(node);
            if (node2d) {
                cha->get_parent()->add_child(node2d);
                node2d->set_global_position(bullet->position);
            }
        }
        kill(bullet,index);
    }
}

void Barrage::kill(const Ref<Bullet> &bullet, int index) {
    if (bullet->live) {
        const_cast<Bullet*>(*bullet)->live = false;
        dead_cache.push_back(index);
        Physics2DServer::get_singleton()->area_set_collision_layer(bullet->checker, 0);
        Physics2DServer::get_singleton()->area_set_collision_mask(bullet->checker, 0);
    }
}

void Barrage::clear() {
    for (int i = 0, t = bullets.size(); i < t; ++i) {
        kill(bullets[i], i);
    }
}

void Barrage::_fixed_process_bullets(float delta_time) {
    ScriptInstance *instance = get_script_instance();

    Transform2D mx = get_viewport_transform();
    Rect2 vis = get_viewport_rect();
    vis.position = -Point2(mx[2][0]/mx[0][0], mx[2][1]/mx[1][1]);
    vis = vis.grow(kill_range);

    for (int i = 0, t = bullets.size(); i < t; ++i) {
        Bullet *bullet = *bullets[i];
        if (bullet->live) {
            _process_bullet(bullet, delta_time);
            if (instance) {
                Variant v1(bullet);
                Variant v2(delta_time);
                const Variant* ptr[2]={&v1, &v2};
                instance->call_multilevel(StringName("_process_bullet"),ptr,2);
            }
            bullet->_process(delta_time);
            if (vis.has_point(bullet->position))   { 
                Transform2D matrix = Transform2D(0, bullet->position);
                float scale = bullet->body_size/DEFAULT_SIZE;
                matrix.scale_basis(Size2(scale, scale));
                Physics2DServer::get_singleton()->area_set_transform(bullet->checker, matrix);
            }else{
                kill(bullet, i);
            }
        }
    }
}


void Barrage::_process_draw_bullets() {
    Size2 size = texture->get_size();
    size = Size2(size.x / h_frames, size.y / v_frames);
    Transform2D m = get_global_transform().affine_inverse();
    if (!texture.is_null()) {
        RID ci=get_canvas_item();
        for (int i = 0, t = bullets.size(); i < t; ++i) {
            Bullet *bullet = *bullets[i];
            if (bullet->live) {
                Transform2D xform(-bullet->rotation, bullet->position);
                xform = m * xform;
                xform.scale_basis(Size2(bullet->scale,bullet->scale));
                VisualServer::get_singleton()->canvas_item_add_set_transform(ci,xform);
                texture->draw_rect_region(ci, Rect2(-size/2.0,size), Rect2(Point2(size.x * (bullet->frame % h_frames), size.y * (bullet->frame / h_frames)), size), Color(1,1,1,1));
            }
        }
    }
}

void Barrage::_update_character() {
    if (is_inside_tree()) {
        if (character_path != NodePath() && has_node(character_path)) {
            Node *node = get_node(character_path);
            Character *character = Object::cast_to<Character>(node);
            if (character) {
                character_id = character->get_instance_id();
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
            _fixed_process_bullets(get_process_delta_time());
            update();
        } break;
        case NOTIFICATION_DRAW: {
            _process_draw_bullets();
        } break;
    }
}

Barrage::Barrage() {
    h_frames = 1;
    v_frames = 1;
    _layer_mask = 0;
    _collision_mask = 0;
    max_index=0;
    kill_range = 600;
    kill_out_screen = true;
    set_process(true);
}

Barrage::~Barrage() {
    for (int i = 0, t = bullets.size(); i < t; ++i) {
        Physics2DServer::get_singleton()->free(bullets[i]->checker);
    }
}

void Barrage::_bind_methods() {
    ClassDB::bind_method(D_METHOD("create_bullet:Bullet", "position"), &Barrage::create_bullet);

    ClassDB::bind_method(D_METHOD("set_texture", "texture:Texture"), &Barrage::set_texture);
    ClassDB::bind_method(D_METHOD("get_texture"), &Barrage::get_texture);

    ClassDB::bind_method(D_METHOD("set_layer_mask", "layer_mask"), &Barrage::set_layer_mask);
    ClassDB::bind_method(D_METHOD("get_layer_mask"), &Barrage::get_layer_mask);

    ClassDB::bind_method(D_METHOD("set_collision_mask", "collision_mask"), &Barrage::set_collision_mask);
    ClassDB::bind_method(D_METHOD("get_collision_mask"), &Barrage::get_collision_mask);

    ClassDB::bind_method(D_METHOD("set_character_path", "character_path"), &Barrage::set_character_path);
    ClassDB::bind_method(D_METHOD("get_character_path"), &Barrage::get_character_path);
    ClassDB::bind_method(D_METHOD("get_character"), &Barrage::get_character);

    ClassDB::bind_method(D_METHOD("set_hit_status", "hit_status"), &Barrage::set_hit_status);
    ClassDB::bind_method(D_METHOD("get_hit_status"), &Barrage::get_hit_status);

    ClassDB::bind_method(D_METHOD("set_explosion_scene", "explosion_scene"), &Barrage::set_explosion_scene);
    ClassDB::bind_method(D_METHOD("get_explosion_scene"), &Barrage::get_explosion_scene);

    ClassDB::bind_method(D_METHOD("set_kill_out_screen", "kill_out_screen"), &Barrage::set_kill_out_screen);
    ClassDB::bind_method(D_METHOD("get_kill_out_screen"), &Barrage::get_kill_out_screen);

    ClassDB::bind_method(D_METHOD("set_kill_range", "kill_range"), &Barrage::set_kill_range);
    ClassDB::bind_method(D_METHOD("get_kill_range"), &Barrage::get_kill_range);

    ClassDB::bind_method(D_METHOD("clear"), &Barrage::clear);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), "set_texture", "get_texture");

    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "character_path"), "set_character_path", "get_character_path");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "hit_status", PROPERTY_HINT_RESOURCE_TYPE, "HitStatus"), "set_hit_status", "get_hit_status");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "explosion_scene", PROPERTY_HINT_RESOURCE_TYPE, "PackedScene"), "set_explosion_scene", "get_explosion_scene");

    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "kill_out_screen"), "set_kill_out_screen", "get_kill_out_screen");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "kill_range"), "set_kill_range", "get_kill_range");

    ADD_PROPERTY(PropertyInfo(Variant::INT, "layers", PROPERTY_HINT_FLAGS), "set_layer_mask", "get_layer_mask");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "masks", PROPERTY_HINT_FLAGS), "set_collision_mask", "get_collision_mask");

    BIND_VMETHOD(MethodInfo("_process_bullet", PropertyInfo(Variant::OBJECT, "bullet", PROPERTY_HINT_RESOURCE_TYPE, "Bullet"), PropertyInfo(Variant::REAL, "delta")));
}
//==================ShootBarrage==========================

void ShootBarrage::_notification(int p_notification) {

    switch (p_notification) {

        case NOTIFICATION_PROCESS: {
            if (shoot_count > 0) { 
                if (frame_count <= 0) {
                    shoot_count -= 1;
                    frame_count += frame_interval;
                    _shoot();
                    ScriptInstance *instance = get_script_instance();
                    if (instance) {
                        static const StringName shoot = StringName("_shoot");
                        instance->call_multilevel(shoot);
                    }
                }
                frame_count-=get_process_delta_time();
            }
        }
    }
}

void ShootBarrage::_bind_methods() {

    ClassDB::bind_method(D_METHOD("set_speed", "speed"), &ShootBarrage::set_speed);
    ClassDB::bind_method(D_METHOD("get_speed"), &ShootBarrage::get_speed);

    ClassDB::bind_method(D_METHOD("set_radius", "radius"), &ShootBarrage::set_radius);
    ClassDB::bind_method(D_METHOD("get_radius"), &ShootBarrage::get_radius);
    
    ClassDB::bind_method(D_METHOD("set_accel", "accel"), &ShootBarrage::set_accel);
    ClassDB::bind_method(D_METHOD("get_accel"), &ShootBarrage::get_accel);
        
    ClassDB::bind_method(D_METHOD("set_rotation_accel", "accel"), &ShootBarrage::set_rotation_accel);
    ClassDB::bind_method(D_METHOD("get_rotation_accel"), &ShootBarrage::get_rotation_accel);

    ClassDB::bind_method(D_METHOD("set_body_size", "body_size"), &ShootBarrage::set_body_size);
    ClassDB::bind_method(D_METHOD("get_body_size"), &ShootBarrage::get_body_size);

    ClassDB::bind_method(D_METHOD("set_bullet_scale", "bullet_scale"), &ShootBarrage::set_bullet_scale);
    ClassDB::bind_method(D_METHOD("get_bullet_scale"), &ShootBarrage::get_bullet_scale);

    ClassDB::bind_method(D_METHOD("set_shoot_time", "shoot_time"), &ShootBarrage::set_shoot_time);
    ClassDB::bind_method(D_METHOD("get_shoot_time"), &ShootBarrage::get_shoot_time);

    ClassDB::bind_method(D_METHOD("set_frame_interval", "frame_interval"), &ShootBarrage::set_frame_interval);
    ClassDB::bind_method(D_METHOD("get_frame_interval"), &ShootBarrage::get_frame_interval);

    ClassDB::bind_method(D_METHOD("start_shoot"), &ShootBarrage::start_shoot);

    ADD_PROPERTY(PropertyInfo(Variant::REAL, "speed"), "set_speed", "get_speed");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "radius"), "set_radius", "get_radius");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "accel"), "set_accel", "get_accel");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "rotation_accel"), "set_rotation_accel", "get_rotation_accel");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "body_size"), "set_body_size", "get_body_size");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "bullet_scale"), "set_bullet_scale", "get_bullet_scale");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "shoot_time"), "set_shoot_time", "get_shoot_time");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "frame_interval"), "set_frame_interval", "get_frame_interval");

    BIND_VMETHOD(MethodInfo("_shoot"));
}

//==================ScatterBarrage==========================

void ScatterBarrage::_shoot(){

    Vector2 v2 = target - get_global_position();
    float in = Math::deg2rad(angle_interval);
    float angle = Math::atan2(v2.y, v2.x);
    float start = angle - in*((bullet_count-1)/2.0);
    if (distance_interval != 0) {
        Vector2 step(Math::cos(angle+Math_PI/2)*distance_interval, Math::sin(angle+Math_PI/2)*distance_interval);
        Vector2 start_pos = -step*((bullet_count-1)/2.0);

        for (int i = 0; i < bullet_count; ++i) {
            float alpha = start + in*i;
            Vector2 vn(Math::cos(alpha), Math::sin(alpha));
            Bullet *bullet = create_bullet(vn*radius + (start_pos+i*step));
            bullet->set_rotation(alpha);
            bullet->set_speed(speed);
            bullet->set_frame(sprite_frame);
            bullet->set_body_size(body_size);
            bullet->set_scale(bullet_scale);
            bullet->set_accel(accel);
            bullet->set_rotation_accel(rotation_accel/180*Math_PI);
        }
    }else {
        for (int i = 0; i < bullet_count; ++i) {
            float alpha = start + in*i;
            Vector2 vn(Math::cos(alpha), Math::sin(alpha));
            Bullet *bullet = create_bullet(vn*radius);
            bullet->set_rotation(alpha);
            bullet->set_speed(speed);
            bullet->set_frame(sprite_frame);
            bullet->set_body_size(body_size);
            bullet->set_scale(bullet_scale);
            bullet->set_accel(accel);
            bullet->set_rotation_accel(rotation_accel/180*Math_PI);
        }
    }
}

void ScatterBarrage::shoot(int frame) {
    sprite_frame = frame;
    start_shoot();
}

void ScatterBarrage::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_interval", "interval"), &ScatterBarrage::set_interval);
    ClassDB::bind_method(D_METHOD("get_interval"), &ScatterBarrage::get_interval);

    ClassDB::bind_method(D_METHOD("set_distance_interval", "interval"), &ScatterBarrage::set_distance_interval);
    ClassDB::bind_method(D_METHOD("get_distance_interval"), &ScatterBarrage::get_distance_interval);

    ClassDB::bind_method(D_METHOD("set_target", "target"), &ScatterBarrage::set_target);
    ClassDB::bind_method(D_METHOD("get_target"), &ScatterBarrage::get_target);

    ClassDB::bind_method(D_METHOD("set_bullet_count", "bullet_count"), &ScatterBarrage::set_bullet_count);
    ClassDB::bind_method(D_METHOD("get_bullet_count"), &ScatterBarrage::get_bullet_count);

    ClassDB::bind_method(D_METHOD("shoot", "frame"), &ScatterBarrage::shoot);

    ADD_PROPERTY(PropertyInfo(Variant::REAL, "interval"), "set_interval", "get_interval");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "distance_interval"), "set_distance_interval", "get_distance_interval");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "bullet_count"), "set_bullet_count", "get_bullet_count");
}

//==================RandomBarrage==========================

void RandomBarrage::_shoot() {
    for (int i = 0; i < bullet_once_count; ++i) {
        make_bullet();
    }
}

float RandomBarrage::randf() {
    return Math::randf()*2-1;
}

Bullet *RandomBarrage::make_bullet() {

    float angle = Math::deg2rad(shoot_angle+angle_range*randf());
    Bullet *bullet = create_bullet(Vector2(pos_range.x*randf(), pos_range.y*randf()));
    bullet->set_rotation(angle);
    bullet->set_speed(speed+speed_range*randf());
    bullet->set_frame(sprite_frame);
    bullet->set_body_size(body_size);
    bullet->set_scale(bullet_scale);
    return bullet;
}

void RandomBarrage::shoot(float angle, int frame) {
    shoot_angle = angle;
    sprite_frame = frame;
    start_shoot();
}

void RandomBarrage::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_pos_range", "pos_range"), &RandomBarrage::set_pos_range);
    ClassDB::bind_method(D_METHOD("get_pos_range"), &RandomBarrage::get_pos_range);

    ClassDB::bind_method(D_METHOD("set_angle_range", "angle_range"), &RandomBarrage::set_angle_range);
    ClassDB::bind_method(D_METHOD("get_angle_range"), &RandomBarrage::get_angle_range);

    ClassDB::bind_method(D_METHOD("set_speed_range", "speed_range"), &RandomBarrage::set_speed_range);
    ClassDB::bind_method(D_METHOD("get_speed_range"), &RandomBarrage::get_speed_range);

    ClassDB::bind_method(D_METHOD("set_bullet_once_count", "bullet_count"), &RandomBarrage::set_bullet_once_count);
    ClassDB::bind_method(D_METHOD("get_bullet_once_count"), &RandomBarrage::get_bullet_once_count);

    ClassDB::bind_method(D_METHOD("shoot", "angle", "frame"), &RandomBarrage::shoot);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "bullet_count"), "set_bullet_once_count", "get_bullet_once_count");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "pos_range"), "set_pos_range", "get_pos_range");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "angle_range"), "set_angle_range", "get_angle_range");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "speed_range"), "set_speed_range", "get_speed_range");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "shoot_time"), "set_shoot_time", "get_shoot_time");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "frame_interval"), "set_frame_interval", "get_frame_interval");
}