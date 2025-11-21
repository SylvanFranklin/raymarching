#version 330 core
uniform bool clicked;
uniform float time;
uniform vec4 influences;
uniform float pulse;
uniform float db;

in vec2 uv;
out vec4 FragColor;

const float MAXIMUM_DISTANCE = 10;
const float MINIMUM_DISTANCE = 0.001;
const int MAXIMUM_STEPS = 100;

// Utils

vec3 marchRay(vec3 p, vec3 rd, float distance) {
  return p + (rd * distance);
}

vec2 closest(vec2 obj1, vec2 obj2) {
  if (obj1.x < obj2.x) {
    return obj1;
  }
  return obj2;
}

vec3 twist(vec3 p) {
  float thing = min(db / -10 - 3, 2);
  float k = thing;
  float c = cos(k * p.y);
  float s = sin(k * p.y);
  mat2 m = mat2(c, -s, s, c);
  vec3 q = vec3(m * p.xz, p.y);
  return q;
}

// https://[quilezles.org/articles/distfunctions/
vec3 repeat(vec3 p, float spacing) {
  return mod(p + spacing * 0.5, spacing) - spacing * 0.5;
}

mat2 rot2D(float angle) {
  float s = sin(angle);
  float c = cos(angle);
  return mat2(c, -s, s, c);
}

// Smoothed mininum between two floats
float smin(float a, float b, float k) {
  float h = max(k - abs(a - b), 0.0) / k;
  return min(a, b) - h * h * h * k * (1.0 / 6.0);
}

// Shapes

float sdSphere(vec3 p, float r) {
  return length(p) - r;
}

float sdBox(vec3 p, vec3 b)
{
  vec3 q = abs(p) - b;
  return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float sdRoundBox(vec3 p, vec3 b, float r) {
  vec3 q = abs(p) - b + r;
  return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0) - r;
}

float sdTorus(vec3 p, vec2 t) {
  vec2 q = vec2(length(p.xz) - t.x, p.y);
  return length(repeat(vec3(q, 1.0), 0.3)) - t.y;
}

float sdLink(vec3 p, float le, float r1, float r2) {
  vec3 q = vec3(p.x, max(abs(p.y) - le, 0.0), p.z);
  return length(vec2(length(q.xy) - r1, q.z)) - r2;
}

float sdBoxFrame(vec3 p, vec3 b, float e) {
  p = abs(p) - b;
  vec3 q = abs(p + e) - e;

  return min(min(
      length(max(vec3(p.x, q.y, q.z), 0.0)) + min(max(p.x, max(q.y, q.z)), 0.0),
      length(max(vec3(q.x, p.y, q.z), 0.0)) + min(max(q.x, max(p.y, q.z)), 0.0)),
    length(max(vec3(q.x, q.y, p.z), 0.0)) + min(max(q.x, max(q.y, p.z)), 0.0));
}

float sdCross(vec3 p) {
  float da = max(abs(p.x), abs(p.y));
  float db = max(abs(p.y), abs(p.z));
  float dc = max(abs(p.z), abs(p.x));
  return min(da, min(db, dc)) - 1.0;
}

//float map(vec3 ro, vec3 rd, float distanceTraveled) {
//    vec3 p = ro + (rd * distanceTraveled);
//    p = reflect(ro,rd,4,vec3(0,1,0),distanceTraveled);
//    vec3 q = twist(vec3(p.x - influences[3], p.y, p.z));
//    q.yz *= rot2D(influences[0] + time / 10);
//    q.xz *= rot2D(influences[1] + time / 30);
//    q.xy *= rot2D(influences[2] - time / 80);
//
////    float distance = sdBoxFrame(repeat(q, 0.5), vec3(0.2, 0.2, 0.2), 0.01); // kinda like alien grass
////    float distance = sdRoundBox(repeat(q,0.5),vec3(0.2,0.2,0.2),0.2); // dope dots
////    float distance = min(sdBoxFrame(repeat(q, 0.5), vec3(0.2, 0.2, 0.2), 0.1),sdTorus(repeat(q, 1),vec2(0.01,0.1))); // sick boxes
//    float distance = sdBoxFrame(repeat(q, 0.2), vec3(0.2, 0.2, 0.01), 0.1); // TAME IMPALA ESQUE
//    return distance;
//}

vec2 distanceToClosestObject(vec3 p) {
  mat2 r = rot2D(time / 3);
  p = twist(vec3(p.x, p.y, p.z));

  vec3 spherePos = p;
  spherePos.x -= influences[2];
  vec2 sphere1 = vec2(sdSphere(spherePos, influences[3]), 1);
  spherePos.x += 2 * influences[2];
  vec2 sphere2 = vec2(sdSphere(spherePos, influences[3]), 1);

  p.zy *= r;
  p.xz *= r;
  float d = sdBox(p, vec3(1));

  float s = 2.0;
  float thing = min(db / -10 - 3, 2);
  for (int m = 0; m < (thing); m++)
  {
    vec3 a = mod(p * s, 2.0) - 1.0;
    s *= 3.0;
    vec3 r = 1.0 - 3.0 * abs(a);

    float c = sdCross(r) / s;
    d = max(d, c);
  }
  return closest(vec2(d, 2), closest(sphere1, sphere2));
}

// https://michaelwalczyk.com/blog-ray-marching.html
vec3 GetSurfaceNormal(vec3 p)
{
  const float eps = 0.0001;
  const vec2 h = vec2(eps, 0);
  return normalize(vec3(distanceToClosestObject(p + h.xyy).x - distanceToClosestObject(p - h.xyy).x,
      distanceToClosestObject(p + h.yxy).x - distanceToClosestObject(p - h.yxy).x,
      distanceToClosestObject(p + h.yyx).x - distanceToClosestObject(p - h.yyx).x));
}

void main() {
  float distance_traveled = 0;
  vec3 ray_origin = vec3(uv.x, uv.y, -3);
  vec3 ray_direction = normalize(vec3(uv, 3));
  int step_count = 0;
  int bounces = 0;
  vec3 color = vec3(0);

  // Render loop
  while (step_count < MAXIMUM_STEPS) {
    vec3 p = marchRay(ray_origin, ray_direction, distance_traveled);
    vec2 closest_object = distanceToClosestObject(p);
    distance_traveled += closest_object.x;

    // hit
    if (closest_object.x <= MINIMUM_DISTANCE) {
      // reflect the ray
      if (closest_object.y == 0 && bounces < 3) {
        ray_origin = p;
        ray_direction = reflect(ray_direction, GetSurfaceNormal(p));
        distance_traveled = 0.01; // offset to prevent the ray being "trapped" within the collision.
        bounces++;
      }
      // color by distance traveled
      if (closest_object.y == 1) {
        float col = (1 / distance_traveled);
        color = vec3(col + 0.2, db / -100, 0.2 + col);
        break;
      }
      // color by normal
      if (closest_object.y == 2) {
        vec3 normal = GetSurfaceNormal(p);
        color = vec3(abs(normal.x), abs(normal.y), abs(normal.z));
        break;
      }
    }
    if (distance_traveled >= MAXIMUM_DISTANCE) {
      break;
    }
    step_count++;
  }

  FragColor = vec4(color, 1);
}

//void main() {
//    float distanceTraveled = 0.1;
//    vec3 ro = vec3(uv.x, uv.y, -3);
//    vec3 rd = normalize(vec3(uv, 1));
//    vec3 color = vec3(0);
//    int stepCount = 0;
//    while(stepCount < maximumSteps){
//        float distance = map(ro,rd,distanceTraveled);
//        distanceTraveled += distance;
//        if(distance <= mininumDistance){ // hit
//            if(distanceTraveled < 0){
//                color = vec3(pulse*2,0,pulse*2);
//                break;
//            }
//            float col = (1/distanceTraveled);
//            color = vec3(col/3,0,col*pulse/2)/5;
//            break;
//        }
//        if(distanceTraveled>=maximumDistance){
//            break;
//        }
//        stepCount++;
//    }
//    FragColor = vec4(color, 1);
//}
