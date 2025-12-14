#version 460 core

out vec4 FragColor;

in vec2 TexCoord;


uniform int width;
uniform int height;

uniform vec3 pixel00_loc;
uniform vec3 pixel_delta_u;
uniform vec3 pixel_delta_v;
uniform vec3 camera_center;

uniform float timer;



struct HitRecord
{
    vec3 p;
    vec3 normal;
    float t;

    bool front_face;
};

struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct Sphere
{
    vec3 center;
    float radius;
};


const int maxSpheres = 2;
struct Hitlist
{
    Sphere spheres[maxSpheres];
    int activeSpheres;
};

const float infinity = 1.0 / 0.0;
const float negInfinity = -1.0 / 0.0;
struct Interval
{
    float minV;
    float maxV;
};

float IntervalSize(Interval interval)
{
    return interval.maxV - interval.minV;
}

bool IntervalContains(Interval interval, float v)
{
    return interval.minV <= v && v <= interval.maxV;
}

bool IntervalSurrounds(Interval interval, float v)
{
    return interval.minV < v && v < interval.maxV;
}

float lengthSqaured(vec3 v)
{
    return v.x*v.x + v.y*v.y + v.z*v.z;
}

void set_face_normal(inout HitRecord hitRecord, Ray r, vec3 outward_normal)
{
    hitRecord.front_face = dot(r.direction, outward_normal) < 0.0;
    hitRecord.normal = hitRecord.front_face ? outward_normal : -outward_normal;
}

vec3 RayAt(Ray ray, float t)
{
    return ray.origin + (ray.direction * t);
}

bool hit_sphere(Sphere sphere, Ray r, Interval ray_t, out HitRecord rec)
{
    vec3 oc = sphere.center - r.origin;
    float a = lengthSqaured(r.direction);
    float h = dot(r.direction, oc);
    float c = lengthSqaured(oc) - (sphere.radius * sphere.radius);
    float discriminant = h * h - a * c;
    //return (discriminant >= 0);

    if (discriminant < 0)
    {
        return false;
    }
    float sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    float root = (h - sqrtd) / a;
    if (!IntervalSurrounds(ray_t, root)) {
        root = (h + sqrtd) / a;
        if (!IntervalSurrounds(ray_t, root))
        {
            return false;
        }
    }

    rec.t = root;
    rec.p = RayAt(r, rec.t);
    rec.normal = (rec.p - sphere.center) / sphere.radius;
    vec3 outward_normal = (rec.p - sphere.center) / sphere.radius;
    set_face_normal(rec, r, outward_normal);


    return true;
}

bool hit_hitlist(Hitlist hitlist, Ray r, Interval ray_t, out HitRecord rec)
{
    HitRecord temp_rec;
    bool hit_anything = false;
    float closest_so_far = ray_t.maxV;

    for (int i = 0; i < hitlist.activeSpheres; ++i)
    {
        if (hit_sphere(hitlist.spheres[i], r, Interval(ray_t.minV, closest_so_far), temp_rec))
        {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}

vec3 getColour(Ray r)
{
    HitRecord rec;
//    Sphere sphere1 = Sphere(vec3(cos(timer) / 10, 0, sin(timer) / 5.0 - 1.0), ((cos(timer / 5.0) + sin(timer / 7.0)) / 20.0) + 0.5);
//    Sphere sphere2 = Sphere(vec3(0.0, 0, (sin(timer) / 2.0) +  -1.0), 0.5);
//    float orbitD = 0.5 / 2.0;
//    float distanceOut = 1.0;
//    float size = 0.1;
//    float speed = 1.0;
//    //Sphere sphere1 = Sphere(vec3(0, 0, -1.0), 0.01);
//    float orbitX = sin(timer * speed) * orbitD;
//    float orbitZ = cos(timer * speed) * orbitD;
//    Sphere sphere1 = Sphere(vec3(-orbitX, 0.0, orbitZ - distanceOut), size);
//    Sphere sphere2 = Sphere(vec3(orbitX, 0.0, -orbitZ - distanceOut), size);

    Sphere sphere1 = Sphere(vec3(0, 0, -1), 0.5);
    Sphere sphere2 = Sphere(vec3(0, -100.5, -1), 100);

    Hitlist hitlist;
    hitlist.spheres[0] = sphere1;
    hitlist.spheres[1] = sphere2;
    hitlist.activeSpheres = 2;
    bool hit = hit_hitlist(hitlist, r, Interval(0.0, infinity), rec);
    if (hit)
    {
        vec3 N = rec.normal;
        if (rec.front_face)
        {
            //N = -N;
        }
//        return rec.t / 3 * vec3(1, 1, 1);
        return 0.5 * (N + vec3(1.0, 1.0, 1.0));
    }

    vec3 unit_direction = normalize(r.direction);
    float a = 0.5*(unit_direction.y + 1.0);
    return (1.0-a)*vec3(1.0, 1.0, 1.0) + a*vec3(0.5, 0.7, 1.0);
}

void main()
{
    int i = int(TexCoord.x * float(width));
    int j = int((-TexCoord.y + 1.0) * float(height));

    vec3 pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
    vec3 ray_direction = pixel_center - camera_center;
    Ray r = Ray(camera_center, ray_direction);

    vec3 colour = getColour(r);
    //vec3 colour  = vec3(TexCoord.xy, 0);
    FragColor = vec4(colour * 1, 1.0);
}