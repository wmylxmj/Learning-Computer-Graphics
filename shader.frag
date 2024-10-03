#version 430 core

in vec2 pixel;
out vec4 FragColor;

const float FLT_MAX = 3.402823466e+38;

const float PI = 3.14159265359;

uniform vec2 viewport_size;
uniform vec3 eye_position;
uniform float fovy;
uniform vec3 camera_x_direction;
uniform vec3 camera_y_direction;
uniform vec3 camera_z_direction;

uniform uint frameCount;

uniform sampler2D previousFrame;

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Vertex {
    vec3 position;
};

struct Triangle {
    uint indices[3];
    //uint padding;
};

struct OrientedBoundingBox {
    vec3 basis[3];
    vec3 size;
    vec3 center;
};

struct BVHNode {
    uint leftChild;
    uint rightChild;
    uint index;
    uint ntris;
    OrientedBoundingBox obb;
};


layout(std430, binding = 0) buffer VerticesBuffer {
    Vertex vertices[];
};

layout(std430, binding = 1) buffer TrianglesBuffer {
    Triangle triangles[];
};

layout(std430, binding = 2) buffer BVHNodesBuffer {
    BVHNode bvh_nodes[];
};


layout(std430, binding = 4) buffer SobolDirectionVectorsBuffer {
    uint sobol_v[];
};

bool hitAABB(Ray ray, vec3 AA, vec3 BB) {
    vec3 t_AA = (AA - ray.origin) / ray.direction;
    vec3 t_BB = (BB - ray.origin) / ray.direction;
    vec3 t_before = min(t_AA, t_BB);
    vec3 t_after = max(t_AA, t_BB);
    float t_in = max(t_before.x, max(t_before.y, t_before.z));
    float t_out = min(t_after.x, min(t_after.y, t_after.z));
    if (t_in > t_out || t_out < 0) return false;
    return true;
}

bool hitOBB(Ray ray, OrientedBoundingBox obb) {
    Ray ray_obbCoord;
    mat3 basis = mat3(obb.basis[0], obb.basis[1], obb.basis[2]);
    ray_obbCoord.origin = inverse(basis) * (ray.origin - obb.center);
    ray_obbCoord.direction = inverse(basis) * ray.direction;
    return hitAABB(ray_obbCoord, -0.5*obb.size, 0.5*obb.size);
}

float hitTriangleCore(Ray ray, vec3 p1, vec3 p2, vec3 p3) {
    vec3 n = normalize(cross(p2-p1, p3-p2));
    float inner = dot(n, ray.direction);
    // if parallel
    if (inner == 0) return FLT_MAX;
    // the triangle normal vector is opposite to the ray direction
    if (inner > 0) n = n * -1;
    float t = (dot(n, p1) - dot(ray.origin, n)) / dot(ray.direction, n);
    // if the triangle is behind the ray
    if (t < 0) return FLT_MAX;
    vec3 p = ray.origin + ray.direction * t;
    // determine whether the intersection point is in the triangle
    vec3 c1 = cross(p2-p1, p-p1);
    vec3 c2 = cross(p3-p2, p-p2);
    vec3 c3 = cross(p1-p3, p-p3);
    if (dot(c1, n) > 0 && dot(c2, n) > 0 && dot(c3, n) > 0) return t;
    if (dot(c1, n) < 0 && dot(c2, n) < 0 && dot(c3, n) < 0) return t;
    return FLT_MAX;
}

float hitTriangle(Ray ray, uint triangle_index) {
    vec3 p1 = vertices[triangles[triangle_index].indices[0]].position;
    vec3 p2 = vertices[triangles[triangle_index].indices[1]].position;
    vec3 p3 = vertices[triangles[triangle_index].indices[2]].position;
    return hitTriangleCore(ray, p1, p2, p3);
}

struct HitResult {
    uint triangle_index;
    float t;
};

HitResult hitTriangleArray(Ray ray, uint index, uint ntris) {
    HitResult res;
    res.t = FLT_MAX;
    for (uint i = index; i < (index + ntris); i++) {
        float t = hitTriangle(ray, i);
        if (t < res.t) {
            res.t = t;
            res.triangle_index = i;
        }
    }
    return res;
}

HitResult hitBVH(Ray ray, uint root_node_index) {
    HitResult res;
    res.t = FLT_MAX;
    uint stack[256];
    uint ptr = 0;
    stack[ptr++] = root_node_index;
    while (ptr > 0) {
        uint node_index = stack[--ptr];
        if (!hitOBB(ray, bvh_nodes[node_index].obb)) {
            continue;
        }
        if (bvh_nodes[node_index].ntris > 0) {
            HitResult hitRes = hitTriangleArray(ray, bvh_nodes[node_index].index, bvh_nodes[node_index].ntris);
            if (hitRes.t < res.t) res = hitRes;
            continue;
        }
        
        if (bvh_nodes[node_index].rightChild > 0) {
            stack[ptr++] = bvh_nodes[node_index].rightChild;
        }
        if (bvh_nodes[node_index].leftChild > 0) {
            stack[ptr++] = bvh_nodes[node_index].leftChild;
        }
    }
    return res;
}

Ray getPixelRay() {
    Ray ray;
    ray.origin = eye_position;
    float aspect = viewport_size[0] / viewport_size[1];
    float top = tan(fovy / 2.0f);
    float bottom = - top;
    float right = top * aspect;
    float left = - right;

    float hcomp = pixel.y * top;
    float wcomp = pixel.x * right;

    ray.direction = -1 * camera_z_direction + hcomp * camera_y_direction + wcomp * camera_x_direction;
    ray.direction = normalize(ray.direction);
    return ray;
}

uint grayCode(uint i) {
    return i ^ (i >> 1);
}

float sobol32(uint d, uint i) {
    uint x = 0;
    uint g = grayCode(i);
    for (uint b = 0; b < 32; b++) {
        if (g == 0) break;
        if (((g >> b) & 1U) == 1) {
            x ^= sobol_v[b + d * 32];
        }
    }
    return float(x) * (1.0f / float(0xFFFFFFFFU));
}

struct SampleResult {
    vec3 direction;
    float pdf;
};

SampleResult SampleHemisphere_CosineWeighted(float xi1, float xi2, vec3 n) {
    vec3 w = normalize(n);
    vec3 helper = vec3(1, 0, 0);
    if (abs(w.x) > 0.999) helper = vec3(0, 1, 0);
    vec3 u = normalize(cross(w, helper));
    vec3 v = normalize(cross(w, u));
    float x = cos(2 * PI * xi2) * sqrt(xi1);
    float y = sin(2 * PI * xi2) * sqrt(xi1);
    float z = sqrt(1 - xi1);
    SampleResult res;
    res.direction = x * u + y * v + z * w;
    res.pdf = (1.0 / PI) * sqrt(1 - xi1);
    return res;
}

vec3 RTAO(Ray ray, HitResult hitRes) {
    vec3 colorOut = vec3(0);
    vec3 colorHistory = vec3(1);
    for (int bounce = 0; bounce < 1; bounce++) {
        vec3 hit_point = ray.direction * hitRes.t + ray.origin;
        Triangle hit_triangle = triangles[hitRes.triangle_index];
        vec3 p1 = vertices[hit_triangle.indices[0]].position;
        vec3 p2 = vertices[hit_triangle.indices[1]].position;
        vec3 p3 = vertices[hit_triangle.indices[2]].position;
        vec3 n = normalize(cross(p2-p1, p3-p2));
        if (dot(n, ray.direction) > 0) n = -n;
        Ray newRay;
        newRay.origin = hit_point + 1e-6 * n;
        SampleResult sampleRes = SampleHemisphere_CosineWeighted(sobol32(bounce*2+1, frameCount), sobol32(bounce*2+2, frameCount), n);
        newRay.direction = sampleRes.direction;
        HitResult newHitRes = hitBVH(newRay, 0);
        vec3 fr = vec3(1, 1, 1) / PI;
        float cosine_wi = dot(normalize(newRay.direction), normalize(n));
        if (newHitRes.t == FLT_MAX) {
            colorOut += colorHistory * vec3(1, 1, 1) * fr * cosine_wi / sampleRes.pdf;
        }
        ray = newRay;
        hitRes = newHitRes;
        colorHistory *= fr * cosine_wi / sampleRes.pdf;
    }
    return colorOut;
}



void main()
{
    Ray ray = getPixelRay();

    vec3 light_position = vec3(0, 1, 0.25);

    vec3 color = vec3(0, 0, 0);

    HitResult hitRes = hitBVH(ray, 0);
    if (hitRes.t == FLT_MAX) {
        color = vec3(1, 1, 1);
    }
    else {
        color = RTAO(ray, hitRes);
    }

    // 和上一帧混合
    vec3 lastColor = texture2D(previousFrame, pixel.xy * 0.5 + 0.5).rgb;
    if (frameCount > 1) {
        color = lastColor * (1 - (1.0 / frameCount)) + color * 1.0 / frameCount;
    }

    FragColor = vec4(color, 1.0);
}