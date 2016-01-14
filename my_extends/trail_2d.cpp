//
// Created by Gen on 2016/1/5.
//

#include "trail_2d.h"

void TrailPoint2D::_update_frame(bool minus) {
    _update_position(minus);
    if (minus) {
        if (trail_enable && (span_frame == 0 || (++span_count > span_frame))) {
            span_count = 0;
            TrailItem np = TrailItem();
            np.count = trail_items.limit();
            trail_items.push(np);
        }
    }
}

void TrailPoint2D::_update_position(bool minus) {
    Vector2 new_pos = get_global_pos();
    Vector2 offset = (new_pos - old_position)/get_global_transform().get_scale();
    old_position = new_pos;

    for (int n = trail_items.size()-1; n >= 0; n--) {
        if (trail_items[n].count > 0) {
            trail_items[n].position -= offset;
            if (minus) {
                trail_items[n].count -= 1;
                trail_items[n].position += gravity/trail_items.limit();
            }
        }else break;
    }
}

void TrailPoint2D::_normal_points(int idx, int total, Vector2 &res1, Vector2 &res2) {
    bool has_before = false, has_front = false;
    int count = trail_items[idx].count, front_idx = count, back_idx = count;
    Point2 pos_o = trail_items[idx].position, pos_f = pos_o, pos_b = pos_o;
    for (int i = idx + 1; i < total; ++i) {
        const TrailItem &tp = trail_items[i];
        if (tp.position != pos_o) {
            pos_f = tp.position;
            front_idx = tp.count;
            has_front = true;
            break;
        }
    }
    for (int j = idx - 1; j >= 0 ; --j) {
        const TrailItem &tp = trail_items[j];
        if (tp.position != pos_o) {
            pos_b = tp.position;
            back_idx = tp.count;
            has_before = true;
            break;
        }
    }

    if (!has_before && !has_front) {
        res1 = res2 = pos_o;
    }else if (!has_front) {
        Vector2 off = pos_o - pos_b;
        Vector2 v1 = Vector2(-off.y, off.x).normalized();
        Vector2 v2 = v1*(line_width*count/(total - 1)/2);
        res1 = pos_o + v2;
        res2 = pos_o - v2;
    } else if (!has_before) {
        res1 = res2 = pos_o;
    }else {
        Vector2 v3 = pos_f - pos_o;
        Vector2 v1 = (pos_b - pos_o + v3).normalized();
        Vector2 v2 = v1*(line_width*count/(total - 1)/2);
        Vector2 p1 = pos_o + v2, p2 = pos_o - v2;

        if((p1.x - pos_o.x)*v3.y-(p1.y - pos_o.y)*v3.x > 0) {
            res1 = p1;
            res2 = p2;
        }else {
            res1 = p2;
            res2 = p1;
        }
    }
}

void TrailPoint2D::_update_trail_target() {
    if (is_inside_tree()) {
        if (target_path != NodePath() && has_node(target_path)) {
            CanvasItem *nt = get_node(target_path)->cast_to<CanvasItem>();
            if (nt != trail_target) {
                if (trail_target != NULL) {
                    trail_target->disconnect("exit_tree", this, "_on_exit_tree");
                }
                trail_target = nt;
                if (trail_target != NULL) {
                    trail_target->connect("exit_tree", this, "_on_exit_tree");
                }
            }
        }else {
            trail_target = NULL;
        }
    }
}

void TrailPoint2D::_on_exit_tree() {
    if (trail_target != NULL) {
        trail_target->disconnect("exit_tree", this, "_on_exit_tree");
        trail_target = NULL;
    }
}

Rect2 TrailPoint2D::get_item_rect() const {

    return Rect2(Point2(-10,-10),Size2(20,20));
}

float determinant(Vector2 v1, Vector2 v2) {
    return v1.x * v2.y - v2.x * v1.y;
}

int intersect(Point2 point_a1, Point2 point_a2, Point2 point_b1, Point2 point_b2, Point2 *result)
{
    float delta = determinant(point_a2-point_a1, point_b1-point_b2);
    float d1 = determinant(point_b1-point_a1, point_b1-point_b2);
    float d2 = determinant(point_a2-point_a1, point_b1-point_a1);
    if ( delta == 0 )
    {
        if (d1 == 0 || d2 == 0) return -1;
        return 0;
    }
    double namenda = d1 / delta;
    if ( namenda>1 || namenda<0 )
    {
        return 0;
    }
    double miu = d2 / delta;
    if (miu>1 || miu<0) {
        return 0;
    }

    if (result != NULL) {
        *result = (point_a2-point_a1)*namenda+point_a1;
    }
    return 1;
}

void TrailPoint2D::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_ENTER_TREE: {
            old_position = get_global_pos();
            _update_trail_target();
        } break;
        case NOTIFICATION_FIXED_PROCESS: {
            _update_frame(true);
        } break;
        case NOTIFICATION_PROCESS: {
            if (trail_target != NULL) {
                set_global_pos(trail_target->get_global_transform().get_origin());
            }
            _update_frame();
            update();
        } break;
        case NOTIFICATION_DRAW: {
            int total = trail_items.size();
            if (wave != 0) time_during += get_process_delta_time();
            for (int n = total-2; n >= 0; n--) {
                if (trail_items[n].count > 0) {
                    float per1 = 1-trail_items[n].count/(float)(total-1);
                    float per2 = 1-trail_items[n+1].count/(float)(total-1);

                    if (wave == 0) {
                        const Vector2& p1 = trail_items[n].position,
                                &p2 = trail_items[n+1].position;
                        if (p1 != p2) {
                            draw_line(p1, p2, !line_color.is_null() ? line_color->get_color_at_offset(per1):Color(1,1,1,1-per1), (1-per1)*line_width);
                        }
                    }else {
                        const Vector2& p1 = trail_items[n].position+(Vector2(0,wave*Math::cos(per1*wave_scale-time_during*wave_time_scale)))*per1,
                                &p2 = trail_items[n+1].position+(Vector2(0,wave*Math::cos(per2*wave_scale-time_during*wave_time_scale)))*per2;
                        if (p1 != p2) {
                            draw_line(p1, p2, !line_color.is_null() ? line_color->get_color_at_offset(per1):Color(1,1,1,1-per1), line_width);
                        }
                    }
                }else break;
            }
        } break;
    }
}

void TrailPoint2D::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("get_trail_enable"), &TrailPoint2D::get_trail_enable);
    ObjectTypeDB::bind_method(_MD("set_trail_enable", "trail_enable"), &TrailPoint2D::set_trail_enable);

    ObjectTypeDB::bind_method(_MD("get_trail_count"),&TrailPoint2D::get_trail_count);
    ObjectTypeDB::bind_method(_MD("set_trail_count", "trail_count"),&TrailPoint2D::set_trail_count);

    ObjectTypeDB::bind_method(_MD("get_span_frame"),&TrailPoint2D::get_span_frame);
    ObjectTypeDB::bind_method(_MD("set_span_frame", "span_frame"),&TrailPoint2D::set_span_frame);

    ObjectTypeDB::bind_method(_MD("get_line_width"),&TrailPoint2D::get_line_width);
    ObjectTypeDB::bind_method(_MD("set_line_width", "line_width"),&TrailPoint2D::set_line_width);

    ObjectTypeDB::bind_method(_MD("get_line_color"),&TrailPoint2D::get_line_color);
    ObjectTypeDB::bind_method(_MD("set_line_color", "line_color:ColorRamp"),&TrailPoint2D::set_line_color);

    ObjectTypeDB::bind_method(_MD("get_target_path"),&TrailPoint2D::get_target_path);
    ObjectTypeDB::bind_method(_MD("set_target_path", "target_path"),&TrailPoint2D::set_target_path);

    ObjectTypeDB::bind_method(_MD("get_gravity"),&TrailPoint2D::get_gravity);
    ObjectTypeDB::bind_method(_MD("set_gravity", "gravity"),&TrailPoint2D::set_gravity);

    ObjectTypeDB::bind_method(_MD("get_wave"),&TrailPoint2D::get_wave);
    ObjectTypeDB::bind_method(_MD("set_wave", "wave"),&TrailPoint2D::set_wave);

    ObjectTypeDB::bind_method(_MD("get_wave_scale"),&TrailPoint2D::get_wave_scale);
    ObjectTypeDB::bind_method(_MD("set_wave_scale", "wave_scale"),&TrailPoint2D::set_wave_scale);

    ObjectTypeDB::bind_method(_MD("get_wave_time_scale"),&TrailPoint2D::get_wave_time_scale);
    ObjectTypeDB::bind_method(_MD("set_wave_time_scale", "wave_time_scale"),&TrailPoint2D::set_wave_time_scale);

    ObjectTypeDB::bind_method(_MD("_on_exit_tree"), &TrailPoint2D::_on_exit_tree);

    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "target_path"), _SCS("set_target_path"), _SCS("get_target_path"));
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "trail_enable"), _SCS("set_trail_enable"), _SCS("get_trail_enable"));
    ADD_PROPERTY(PropertyInfo(Variant::INT, "trail_count"), _SCS("set_trail_count"), _SCS("get_trail_count"));
    ADD_PROPERTY(PropertyInfo(Variant::INT, "span_frame"), _SCS("set_span_frame"), _SCS("get_span_frame"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "line_width"), _SCS("set_line_width"), _SCS("get_line_width"));
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "line_color",PROPERTY_HINT_RESOURCE_TYPE,"ColorRamp"), _SCS("set_line_color"), _SCS("get_line_color"));

    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "gravity"), _SCS("set_gravity"), _SCS("get_gravity"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "wave"), _SCS("set_wave"), _SCS("get_wave"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "wave_scale"), _SCS("set_wave_scale"), _SCS("get_wave_scale"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "wave_time_scale"), _SCS("set_wave_time_scale"), _SCS("get_wave_time_scale"));
}


Rect2 TrailLine2D::get_item_rect() const {

    return Rect2(Point2(-10,-10),Size2(20,20));
}

void TrailLine2D::_update_frame(bool minus) {
    _update_position(minus);
    if (minus) {
        if (trail_enable && (span_frame == 0 || (++span_count > span_frame))) {
            span_count = 0;
            TrailItem np = TrailItem();
            if (terminal)
                np.position2 = terminal->get_pos();
            np.count = trail_items.limit();
            trail_items.push(np);
        }
    }
}

void TrailLine2D::_update_position(bool minus) {
    Vector2 new_pos = get_global_pos();
    Vector2 offset = (new_pos - old_position)/get_global_transform().get_scale();
    old_position = new_pos;

    for (int n = trail_items.size()-1; n >= 0; n--) {
        if (trail_items[n].count > 0) {
            trail_items[n].offset(-offset);
            if (minus)
                trail_items[n].count -= 1;
        }else break;
    }
}

Vector< Vector<Point2> > simplify(Vector<Point2> original) {
    Vector< Vector<Point2> > new_vs;
    int total = original.size();
	if (total < 3) return new_vs;
    int i = 0;
    Vector<Point2> n_path;
    n_path.push_back(original[0]);
    while (i < total) {
        const Vector2 &p1 = original[i];
        int next = (i+1)%total;
        bool conti = true;
        while (true) {
            if (next == 0) {
                conti = false;
                break;
            }
            if (original[next] != p1) {
                break;
            }else {
                next = (next+1)%total;
            }
        }
        if (!conti) break;
        const Vector2 &p2 = original[next];
        n_path.push_back(p2);
        i = next;
    }
    total = n_path.size();
    if (total >= 3) {
        int off = 0;
        while (off < total) {
            if (total <= 3) break;
            Point2 &p1 = n_path[off], p2 = n_path[(off+1)%total];
            int c_off = off + 1;
            while (c_off < total) {
                Point2 &p3 = n_path[c_off], &p4 = n_path[(c_off+1)%total], cp;
                int check = intersect(p1, p2, p3, p4, &cp);
                if (total < 3) {
                    total = 0;
                    break;
                }else if (total == 3 && check == 1) {
                    off = total;
                    break;
                }else if (total == 3) {
                    break;
                }else if (check == 1 && c_off != off + 1 && (c_off != total - 1 || off != 0)) {
                    Vector<Point2> vs;
                    vs.push_back(cp);
                    for (int j = off+1, t = (c_off+1>total?total:c_off+1); j < t; ++j) {
                        vs.push_back(n_path[off+1]);
                        n_path.remove(off+1);
                    }
                    total = n_path.size();
                    Vector< Vector<Point2> > s_p = simplify(vs);
                    for (int k = 0, t = s_p.size(); k < t; ++k) {
                        new_vs.push_back(s_p[k]);
                    }
                    if (cp == p1) {
                        break;
                    }else if (cp == p4) {
                        p2 = cp;
                        c_off += 1;
                    }else {
                        p2 = cp;
                        n_path.insert(off+1, cp);
                    }
                }else if (check == -1) {
                    Vector<Point2> vs;
                    for (int j = off+1, t = (c_off+1>total?total:c_off+1); j < t; ++j) {
                        vs.push_back(n_path[off+1]);
                        n_path.remove(off+1);
                    }
                    Vector< Vector<Point2> > s_p = simplify(vs);
                    for (int k = 0, t = s_p.size(); k < t; ++k) {
                        new_vs.push_back(s_p[k]);
                    }
                    if (p1 == p4) {
                        n_path.remove(off+1);
                    }else {
                        p2 = p4;
                    }
                    total = n_path.size();
                }
                c_off ++;
            }
            off += 1;
        }
    }
    if (total > 3) new_vs.push_back(n_path);

    return new_vs;
}

void TrailLine2D::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_ENTER_TREE: {
            old_position = get_global_pos();
            terminal = memnew(Position2D);
            add_child(terminal);
            terminal->set_pos(terminal_position);
        } break;
        case NOTIFICATION_FIXED_PROCESS: {
            _update_frame(true);
        } break;
        case NOTIFICATION_PROCESS: {
            _update_frame();
            update();
        } break;
        case NOTIFICATION_DRAW: {
            Vector<Point2> vs;
            int total = trail_items.size();
            if (total > 1) {
                vs.resize(total * 2);
                for (int i = total-1; i >= 0; --i) {
                    const TrailItem &item = trail_items[i];
                    if (item.count > 0) {
                        vs[i] = item.position1;
                        vs[total*2-1-i] = item.position2;
                    }else break;
                }
                Vector< Vector<Point2> > pols = simplify(vs);
                Color color = Color(1,1,1,0.3);
                for (int j = 0, t = pols.size(); j < t; ++j) {
                    draw_colored_polygon(pols[j], color);
                }
            }
        } break;
    }
}

void TrailLine2D::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("get_trail_enable"), &TrailLine2D::get_trail_enable);
    ObjectTypeDB::bind_method(_MD("set_trail_enable", "trail_enable"), &TrailLine2D::set_trail_enable);

    ObjectTypeDB::bind_method(_MD("get_trail_count"),&TrailLine2D::get_trail_count);
    ObjectTypeDB::bind_method(_MD("set_trail_count", "trail_count"),&TrailLine2D::set_trail_count);

    ObjectTypeDB::bind_method(_MD("get_span_frame"),&TrailLine2D::get_span_frame);
    ObjectTypeDB::bind_method(_MD("set_span_frame", "span_frame"),&TrailLine2D::set_span_frame);

    ObjectTypeDB::bind_method(_MD("get_terminal"),&TrailLine2D::get_terminal);
    ObjectTypeDB::bind_method(_MD("set_terminal", "terminal"),&TrailLine2D::set_terminal);

    ObjectTypeDB::bind_method(_MD("get_line_color"),&TrailLine2D::get_line_color);
    ObjectTypeDB::bind_method(_MD("set_line_color", "line_color:ColorRamp"),&TrailLine2D::set_line_color);

//    ObjectTypeDB::bind_method(_MD("_on_exit_tree"), &TrailPoint2D::_on_exit_tree);

//    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "target_path"), _SCS("set_target_path"), _SCS("get_target_path"));
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "trail_enable"), _SCS("set_trail_enable"), _SCS("get_trail_enable"));
    ADD_PROPERTY(PropertyInfo(Variant::INT, "trail_count"), _SCS("set_trail_count"), _SCS("get_trail_count"));
    ADD_PROPERTY(PropertyInfo(Variant::INT, "span_frame"), _SCS("set_span_frame"), _SCS("get_span_frame"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "terminal"), _SCS("set_terminal"), _SCS("get_line_color"));
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "line_color",PROPERTY_HINT_RESOURCE_TYPE,"ColorRamp"), _SCS("set_line_color"), _SCS("get_line_color"));
}