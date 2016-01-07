//
// Created by Gen on 2016/1/5.
//

#include "trail_2d.h"

void TrailPoint2D::_update_frame(bool minus) {
    _update_position(minus);
    if (trail_enable && (span_frame == 0 || (++span_count > span_frame))) {
        span_count = 0;
        TrailPoint np = TrailPoint();
        np.count = trail_positions.limit();
        trail_positions.push(np);
    }
}

void TrailPoint2D::_update_position(bool minus) {
    Vector2 new_pos = get_global_pos();
    Vector2 offset = (new_pos - old_position)/get_global_transform().get_scale();
    old_position = new_pos;

    for (int n = trail_positions.size()-1; n >= 0; n--) {
        if (trail_positions[n].count > 0) {
            trail_positions[n].position -= offset;
            if (minus)
                trail_positions[n].count -= 1;
        }else break;
    }
}

void TrailPoint2D::_normal_points(int idx, int total, Vector2 &res1, Vector2 &res2) {
    bool has_before = false, has_front = false;
    int count = trail_positions[idx].count, front_idx = count, back_idx = count;
    Point2 pos_o = trail_positions[idx].position, pos_f = pos_o, pos_b = pos_o;
    int offset = 0;
    for (int i = idx + 1; i < total; ++i) {
        const TrailPoint &tp = trail_positions[i];
        if (tp.position != pos_o) {
            pos_f = tp.position;
            front_idx = tp.count;
            has_front = true;
            break;
        }
    }
    for (int j = idx - 1; j >= 0 ; --j) {
        const TrailPoint &tp = trail_positions[j];
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

bool intersect(Point2 point_a1, Point2 point_a2, Point2 point_b1, Point2 point_b2, Point2 *result)
{
    double delta = determinant(point_a2-point_a1, point_b1-point_b2);
    if ( delta == 0 )
    {
        return false;
    }
    double namenda = determinant(point_b1-point_a1, point_b1-point_b2) / delta;
    if ( namenda>1 || namenda<0 )
    {
        return false;
    }
    double miu = determinant(point_a2-point_a1, point_b1-point_a1) / delta;
    if (miu>1 || miu<0) {
        return false;
    }

    if (result != NULL) {
        *result = (point_a2-point_a1)*namenda+point_a1;
    }
    return true;
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
            if (line_width <= 10) {
                int total = trail_positions.size();
                for (int n = total-2; n >= 0; n--) {
                    if (trail_positions[n].count > 0) {
                        const Vector2& p1 = trail_positions[n].position, p2 = trail_positions[n+1].position;
                        if (p1 != p2) {
                            float p = trail_positions[n].count/(float)(total-1);
                            draw_line(p1, p2, !line_color.is_null() ? line_color->get_color_at_offset(p):Color(1,1,1,p), p*line_width);
                        }
                    }else break;
                }
            }else {
                int total = trail_positions.size();
                Vector2 last_points[2];
                Color last_color;
                for (int n = total-2; n >= 0; n--) {
                    if (trail_positions[n].count > 0) {
                        if (n == total-2) {
                            _normal_points(n+1, total, last_points[0], last_points[1]);
                            last_color = !line_color.is_null() ? line_color->get_color_at_offset(1):Color(1,1,1);
                        }
                        Vector2 ps[2];
                        _normal_points(n, total, ps[0], ps[1]);
                        Vector2 cross_point;
                        if ((trail_positions[n].position - trail_positions[n+1].position).length() < 1e-6) {
                        }else if (intersect(ps[0], ps[1], last_points[0], last_points[1], &cross_point)) {
//                            Color color = !line_color.is_null() ? line_color->get_color_at_offset(n/(float)(total-1)):Color(1,1,1,n/(float)(total-1));
//                            Vector<Vector2> points;
//                            points.resize(3);
//                            points[0] = ps[0];
//                            points[1] = last_points[1];
//                            points[2] = cross_point;
//                            Vector<Color> colors;
//                            colors.resize(3);
//                            colors[0] = color;
//                            colors[1] = last_color;
//                            colors[2] = last_color;
//                            draw_polygon(points, colors);
//                            points[0] = ps[1];
//                            points[1] = last_points[0];
//                            points[2] = cross_point;
//                            colors[0] = color;
//                            colors[1] = last_color;
//                            colors[2] = last_color;
//                            last_color = color;
//                            last_points[0] = ps[1];
//                            last_points[1] = ps[0];
//                            draw_polygon(points, colors);
                        }else if (intersect(ps[1], last_points[0], last_points[1], ps[0], &cross_point)) {

                        }else if (ps[0] == ps[1]) {
//                            Color color = !line_color.is_null() ? line_color->get_color_at_offset(n/(float)(total-1)):Color(1,1,1,n/(float)(total-1));
//                            Vector<Vector2> points;
//                            points.resize(3);
//                            points[0] = ps[0];
//                            points[1] = last_points[1];
//                            points[2] = last_points[0];
//                            Vector<Color> colors;
//                            colors.resize(3);
//                            colors[0] = color;
//                            colors[1] = last_color;
//                            colors[2] = last_color;
//                            last_color = color;
//                            draw_polygon(points, colors);
//                            last_points[0] = ps[1];
//                            last_points[1] = ps[0];
                        }else {
                            Vector<Vector2> points;
                            points.resize(4);
                            points[0] = ps[0];
                            points[1] = ps[1];
                            points[2] = last_points[0];
                            points[3] = last_points[1];
                            Color color = !line_color.is_null() ? line_color->get_color_at_offset(n/(float)(total-1)):Color(1,1,1,n/(float)(total-1));
                            Vector<Color> colors;
                            colors.resize(4);
                            colors[0] = color;
                            colors[1] = color;
                            colors[2] = last_color;
                            colors[3] = last_color;
                            last_color = color;
                            last_points[0] = ps[1];
                            last_points[1] = ps[0];
                            draw_polygon(points, colors);
                        }
                    }else break;
                }
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

    ObjectTypeDB::bind_method(_MD("_on_exit_tree"), &TrailPoint2D::_on_exit_tree);

    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "target_path"), _SCS("set_target_path"), _SCS("get_target_path"));
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "trail_enable"), _SCS("set_trail_enable"), _SCS("get_trail_enable"));
    ADD_PROPERTY(PropertyInfo(Variant::INT, "trail_count"), _SCS("set_trail_count"), _SCS("get_trail_count"));
    ADD_PROPERTY(PropertyInfo(Variant::INT, "span_frame"), _SCS("set_span_frame"), _SCS("get_span_frame"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "line_width"), _SCS("set_line_width"), _SCS("get_line_width"));
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "line_color",PROPERTY_HINT_RESOURCE_TYPE,"ColorRamp"), _SCS("set_line_color"), _SCS("get_line_color"));
}