#include "collision_detection.h"
#include "contact.h"
#include "graphics.h"
#include "shape.h"
#include "vec2.h"
#include <iostream>
#include <limits>
#include <vector>

bool CollisionDetection::is_colliding(Body *a, Body *b,
                                      std::vector<Contact> &contacts) {
    bool a_is_circle = a->shape->get_type() == CIRCLE;
    bool b_is_circle = b->shape->get_type() == CIRCLE;
    bool a_is_polygon =
        a->shape->get_type() == POLYGON || a->shape->get_type() == BOX;
    bool b_is_polygon =
        b->shape->get_type() == POLYGON || b->shape->get_type() == BOX;

    if (a_is_circle && b_is_circle) {
        return is_colliding_circle_circle(a, b, contacts);
    }

    if (a_is_polygon && b_is_polygon) {
        return is_colliding_polygon_polygon(a, b, contacts);
    }

    if (a_is_polygon && b_is_circle) {
        return is_colliding_polygon_circle(a, b, contacts);
    }
    if (a_is_circle && b_is_polygon) {
        return is_colliding_polygon_circle(b, a, contacts);
    }
    return false;
}

bool CollisionDetection::is_colliding_circle_circle(
    Body *a, Body *b, std::vector<Contact> &contacts) {
    CircleShape *a_shape = (CircleShape *)a->shape;
    CircleShape *b_shape = (CircleShape *)b->shape;

    const Vec2 ab = b->position - a->position;
    const auto sum_radius = a_shape->radius + b_shape->radius;

    bool is_colliding = ab.mag_sqaure() <= sum_radius * sum_radius;
    if (!is_colliding) {
        return false;
    }

    // compute Contact information
    Contact contact;
    contact.a = a;
    contact.b = b;
    contact.normal = ab;
    contact.normal.normalize();
    contact.start = b->position - contact.normal * b_shape->radius;
    contact.end = a->position + contact.normal * a_shape->radius;
    contact.depth = (contact.end - contact.start).mag();

    contacts.push_back(contact);

    return true;
}

bool CollisionDetection::is_colliding_polygon_polygon(
    Body *a, Body *b, std::vector<Contact> &contacts) {
    // find separation between a and b, _and_ b and a
    PolygonShape *a_polygon_shape = (PolygonShape *)a->shape;
    PolygonShape *b_polygon_shape = (PolygonShape *)b->shape;
    int a_index_ref_edge, b_index_ref_edge;
    Vec2 a_support_point, b_support_point;

    // return the **index** of the reference edge, _NOT_ the edge itself

    float ab_sep = a_polygon_shape->find_min_separation(
        b_polygon_shape, a_index_ref_edge, a_support_point);
    if (ab_sep >= 0) {
        return false;
    }

    float ba_sep = b_polygon_shape->find_min_separation(
        a_polygon_shape, b_index_ref_edge, b_support_point);
    if (ba_sep >= 0) {
        return false;
    }

    PolygonShape *ref_shape;
    PolygonShape *incident_shape;
    int index_ref_edge;
    if (ab_sep > ba_sep) {
        // set "A" as ref shape
        // set "B" as incident shape
        ref_shape = a_polygon_shape;
        incident_shape = b_polygon_shape;
        index_ref_edge = a_index_ref_edge;
    } else {
        // set "B" as ref shape
        // set "A" as incident shape
        ref_shape = b_polygon_shape;
        incident_shape = a_polygon_shape;
        index_ref_edge = b_index_ref_edge;
    }

    // find the ref edge
    Vec2 ref_edge = ref_shape->edge_at(index_ref_edge);
    // find the incident edge
    int incident_index = incident_shape->find_incident_edge(ref_edge.normal());

    // perform clipping
    // clip the incident edge based on the side planes of the reference edge
    int incident_next_index =
        (incident_index + 1) % incident_shape->world_vertices.size();
    Vec2 v0 = incident_shape->world_vertices[incident_index];
    Vec2 v1 = incident_shape->world_vertices[incident_next_index];

    std::vector<Vec2> contact_points = {v0, v1};
    std::vector<Vec2> clipped_points = contact_points;

    for (size_t i = 0; i < ref_shape->world_vertices.size(); i++) {
        if ((int)i == index_ref_edge)
            continue;
        Vec2 c0 = ref_shape->world_vertices[i];
        Vec2 c1 =
            ref_shape
                ->world_vertices[(i + 1) % ref_shape->world_vertices.size()];
        int num_clipped = ref_shape->clip_segment_to_line(
            contact_points, clipped_points, c0, c1);
        std::cout << "num_clipped: " << num_clipped << std::endl;
        if (num_clipped < 2)
            break;

        // make next contact points the ones that were just clipped
        contact_points = clipped_points;
    }

    auto vref = ref_shape->world_vertices[index_ref_edge];

    // loop all clipped points, but only consider those where separation is
    // negative (objects are penetrating each other)
    for (auto &vclip : clipped_points) {
        float separation = (vclip - vref).dot(ref_edge.normal());
        if (separation <= 0) {
            // negative separation means positive penetration
            Contact contact;
            contact.a = a;
            contact.b = b;
            contact.normal = ref_edge.normal();
            contact.start = vclip;
            contact.end = vclip + contact.normal * -separation;
            if (ba_sep >= ab_sep) {
                // the start-end points are always from a to b
                std::swap(contact.start, contact.end);
                // the collision normal is always from a to b
                contact.normal *= -1.0;
            }

            contacts.push_back(contact);
        }
    }

    return true;

    /*
    Contact contact;
    contact.a = a;
    contact.b = b;
    // always pointing from a to b
    if (ab_sep > ba_sep) {
        contact.depth = -ab_sep;
        contact.normal = a_axis.normal();
        contact.start = a_point;
        contact.end = a_point + contact.normal * contact.depth;
    } else {
        contact.depth = -ba_sep;
        contact.normal = -b_axis.normal();
        contact.start = b_point - contact.normal * contact.depth;
        contact.end = b_point;
    }

    contacts.push_back(contact);
    */
}

bool CollisionDetection::is_colliding_polygon_circle(
    Body *polygon, Body *circle, std::vector<Contact> &contacts) {
    const PolygonShape *polygon_shape = (PolygonShape *)polygon->shape;
    const CircleShape *circle_shape = (CircleShape *)circle->shape;
    const std::vector<Vec2> &polygon_vertices = polygon_shape->world_vertices;

    Vec2 min_curr_vertex;
    Vec2 min_next_vertex;
    bool is_outside = false;
    float distance_circle_edge = std::numeric_limits<float>::lowest();

    for (size_t i = 0; i < polygon_vertices.size(); i++) {
        int curr_vertex_idx = i;
        int next_vertex_idx = (i + 1) % polygon_vertices.size();
        Vec2 edge = polygon_shape->edge_at(curr_vertex_idx);
        Vec2 normal = edge.normal();

        // compare circle center with rectangle vertex
        Vec2 vertext_to_circle_center =
            circle->position - polygon_vertices[curr_vertex_idx];

        // project circle center onto the edge normal
        float projection = vertext_to_circle_center.dot(normal);

        // if projection is on the positive side of the normal
        if (projection > 0) {
            // circle outside the polygon
            distance_circle_edge = projection;
            min_curr_vertex = polygon_vertices[curr_vertex_idx];
            min_next_vertex = polygon_vertices[next_vertex_idx];
            is_outside = true;
            break;
        } else {
            // circle center is inside the polygon
            // find the min edge - edge with the least negative projection
            if (projection > distance_circle_edge) {
                distance_circle_edge = projection;
                min_curr_vertex = polygon_vertices[curr_vertex_idx];
                min_next_vertex = polygon_vertices[next_vertex_idx];
            }
        }
    }

    Contact contact;
    if (is_outside) {
        // check if inside region A
        Vec2 v1 = circle->position - min_curr_vertex;
        Vec2 v2 = min_next_vertex - min_curr_vertex;
        if (v1.dot(v2) < 0) {
            if (v1.mag() > circle_shape->radius) {
                return false;
            } else {
                // detected collision in region A
                contact.a = polygon;
                contact.b = circle;
                contact.depth = circle_shape->radius - v1.mag();
                contact.normal = v1.normalize();
                contact.start =
                    circle->position - (contact.normal * circle_shape->radius);
                contact.end = contact.start + (contact.normal * contact.depth);
            }
        } else {
            // check if inside region B
            v1 = circle->position - min_next_vertex;
            v2 = min_curr_vertex - min_next_vertex;
            if (v1.dot(v2) < 0) {
                // inside region B
                if (v1.mag() > circle_shape->radius) {
                    return false;
                } else {
                    contact.a = polygon;
                    contact.b = circle;
                    contact.depth = circle_shape->radius - v1.mag();
                    contact.normal = v1.normalize();
                    contact.start = circle->position -
                                    (contact.normal * circle_shape->radius);
                    contact.end =
                        contact.start + (contact.normal * contact.depth);
                }
            } else {
                // region C
                if (distance_circle_edge > circle_shape->radius) {
                    return false;
                } else {
                    contact.a = polygon;
                    contact.b = circle;
                    contact.depth = circle_shape->radius - distance_circle_edge;
                    contact.normal =
                        (min_next_vertex - min_curr_vertex).normal();
                    contact.start = circle->position -
                                    (contact.normal * circle_shape->radius);
                    contact.end =
                        contact.start + (contact.normal * contact.depth);
                }
            }
        }

    } else {
        // center of circle is inside polygon... Definitely colliding
        contact.a = polygon;
        contact.b = circle;
        contact.depth = circle_shape->radius - distance_circle_edge;
        contact.normal = (min_next_vertex - min_curr_vertex).normal();
        contact.start =
            circle->position - (contact.normal * circle_shape->radius);
        contact.end = contact.start + (contact.normal * contact.depth);
    }

    // Graphics::draw_fill_circle(min_curr_vertex.x, min_curr_vertex.y, 5,
    // 0xFF00FFFF);
    // Graphics::draw_fill_circle(min_next_vertex.x, min_next_vertex.y, 5,
    // 0xFF00FFFF);

    contacts.push_back(contact);

    return true;
}
