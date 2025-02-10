#include "shape.h"
#include "vec2.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

CircleShape::CircleShape(float radius) : radius(radius) {
    std::cout << "CircleShape constructor called!" << std::endl;
}

CircleShape::~CircleShape() {
    std::cout << "CircleShape destructor called!" << std::endl;
}

float CircleShape::get_moment_of_inertia() const {
    // remember, to be multiplied by `mass`
    return 0.5 * (radius * radius);
}

void CircleShape::update_vertices([[maybe_unused]] float angle,
                                  [[maybe_unused]] const Vec2 &position) {
    // circle have no vertices
    return;
}

ShapeType CircleShape::get_type() const { return CIRCLE; }

Shape *CircleShape::clone() const { return new CircleShape(radius); }

PolygonShape::PolygonShape(const std::vector<Vec2> vertices)
    : local_vertices(vertices), world_vertices(vertices) {

    float min_x = std::numeric_limits<float>::max();
    float min_y = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float max_y = std::numeric_limits<float>::lowest();

    // initialize vertices of the polygon shape and set width and height
    for (auto vertex : vertices) {
        local_vertices.push_back(vertex);
        world_vertices.push_back(vertex);

        // find min and max X and Y to calculate polygon width and height
        min_x = std::min(min_x, vertex.x);
        max_x = std::max(max_x, vertex.x);
        min_y = std::min(min_y, vertex.y);
        max_y = std::max(max_y, vertex.y);
    }

    width = max_x - min_x;
    height = max_y - min_y;

    std::cout << "PolygonShape constructor called!" << std::endl;
}

PolygonShape::~PolygonShape() {
    std::cout << "PolygonShape destructor called!" << std::endl;
}

ShapeType PolygonShape::get_type() const { return POLYGON; }

Shape *PolygonShape::clone() const { return new PolygonShape(local_vertices); }

float PolygonShape::get_moment_of_inertia() const {
    // TODO
    return 5000;
}

void PolygonShape::update_vertices(float angle, const Vec2 &position) {
    for (size_t i = 0; i < local_vertices.size(); i++) {
        // first, rotate
        world_vertices[i] = local_vertices[i].rotate(angle);

        // then, translate
        world_vertices[i] += position;
    }
}

Vec2 PolygonShape::edge_at(const int index) const {
    int curr_vertex = index;
    int next_vertex = (index + 1) % world_vertices.size();

    return world_vertices[next_vertex] - world_vertices[curr_vertex];
}

float PolygonShape::find_min_separation(const PolygonShape *other,
                                        int &index_ref_edge,
                                        Vec2 &support_point) const {
    // Loop all vertices of a
    //   - find normal axis (perpendicular)
    //   - loop all vertices of b
    //     - project vertex b onto the normal axis
    //     - keep track of the min separation
    // Return the best separation out of all axis
    float separation = std::numeric_limits<float>::lowest();
    // separation is also negative collision depth

    for (size_t i = 0; i < this->world_vertices.size(); i++) {
        Vec2 va = this->world_vertices[i];
        Vec2 normal = this->edge_at(i).normal();
        float min_sep = std::numeric_limits<float>::max();
        Vec2 min_vertex;

        for (size_t j = 0; j < other->world_vertices.size(); j++) {
            Vec2 vb = other->world_vertices[j];
            float proj = (vb - va).dot(normal);
            if (proj < min_sep) {
                min_sep = proj;
                min_vertex = vb;
            }
            min_sep = std::min(min_sep, (vb - va).dot(normal));
        }

        if (separation < min_sep) {
            separation = min_sep;
            index_ref_edge = i;
            support_point = min_vertex;
        }
    }

    return separation;
}

int PolygonShape::find_incident_edge(const Vec2 &normal) const {
    int index_incident_edge = 0;
    float min_proj = std::numeric_limits<float>::max();
    for (size_t i = 0; i < this->world_vertices.size(); ++i) {
        auto edge_normal = this->edge_at(i).normal();
        auto proj = edge_normal.dot(normal);
        if (proj < min_proj) {
            min_proj = proj;
            index_incident_edge = i;
        }
    }
    return index_incident_edge;
}

int PolygonShape::clip_segment_to_line(const std::vector<Vec2> &contacts_in,
                                       std::vector<Vec2> &contacts_out,
                                       const Vec2 &c0, const Vec2 &c1) const {
    // start with no output pionts
    int num_out = 0;

    // calculate the distance of end points to the line
    Vec2 normal = (c1 - c0).normalize();
    float dist0 = (contacts_in[0] - c0).cross(normal);
    float dist1 = (contacts_in[1] - c0).cross(normal);

    // if the points are behind the plane
    if (dist0 <= 0) {
        contacts_out[num_out++] = contacts_in[0];
    }
    if (dist1 <= 0) {
        contacts_out[num_out++] = contacts_in[1];
    }

    // if the points are on different sides of the plane
    if (dist0 * dist1 < 0) {
        float total_dist = dist0 - dist1;

        // find the intersection using linear interpolation
        // lerp(start, end) => start + t * (end - start)
        float t = dist0 / (total_dist);
        Vec2 contact = contacts_in[0] + (contacts_in[1] - contacts_in[0]) * t;

        contacts_out[num_out] = contact;
        num_out++;
    }

    return num_out;
}

BoxShape::BoxShape(float width, float height) : width(width), height(height) {
    // load vertices of the box polygon
    local_vertices.push_back(Vec2(-width / 2.0, -height / 2.0));
    local_vertices.push_back(Vec2(width / 2.0, -height / 2.0));
    local_vertices.push_back(Vec2(width / 2.0, height / 2.0));
    local_vertices.push_back(Vec2(-width / 2.0, height / 2.0));

    world_vertices.push_back(Vec2(-width / 2.0, -height / 2.0));
    world_vertices.push_back(Vec2(width / 2.0, -height / 2.0));
    world_vertices.push_back(Vec2(width / 2.0, height / 2.0));
    world_vertices.push_back(Vec2(-width / 2.0, height / 2.0));
    std::cout << "BoxShape constructor called!" << std::endl;
}

BoxShape::~BoxShape() {
    std::cout << "BoxShape destructor called!" << std::endl;
}

ShapeType BoxShape::get_type() const { return BOX; }

Shape *BoxShape::clone() const { return new BoxShape(width, height); }

float BoxShape::get_moment_of_inertia() const {
    // remember, to be multiplied by `mass`
    return (1.0 / 12.0) * (width * width + height * height);
}
