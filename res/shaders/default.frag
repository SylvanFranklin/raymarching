#version 330 core
uniform bool clicked;
uniform float time;
uniform vec4 influences;
uniform float pulse;

in vec2 uv;
out vec4 FragColor;

const float maximumDistance = 10;
const float mininumDistance = 0.001;
const int maximumSteps = 100;

// Utils

vec3 twist(vec3 p) {
    const float k = 0.4;
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

// Smooth min between two floats
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

float sdCross(vec3 p)
{
    float da = max(abs(p.x),abs(p.y));
    float db = max(abs(p.y),abs(p.z));
    float dc = max(abs(p.z),abs(p.x));
    return min(da,min(db,dc))-1.0;
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


vec2 closest(vec2 obj1, vec2 obj2){
    if (obj1.x < obj2.x){
        return obj1;
    }
    return obj2;
}

vec2 closestObject(vec3 p){
    mat2 r = rot2D(-0.785398 + time/3);
    p = twist(vec3(p.x, p.y, p.z));
//    mat2 r45 = rot2D(-0.785398-influences[3]);
//    mat2 r45_ = rot2D(0.785398+influences[3]);
//
//    vec3 boxPos1 = p;
//    vec3 boxPos2 = p;
//    boxPos1.xz *= r45;
//    boxPos2.xz *= r45_;
//    boxPos1.z-=1.5;
//    boxPos2.z-=1.5;
//    vec2 box = vec2(sdBox(boxPos1, vec3(2, 2, 0.001)), 0);
//    vec2 box2 = vec2(sdBox(boxPos2, vec3(2, 2, 0.001)), 0);
    vec3 spherePos = p;
    spherePos.x -= influences[2];
    vec2 sphere = vec2(sdSphere(spherePos, influences[3]), 0);
    spherePos.x += 2*influences[2];
    vec2 sphere2 = vec2(sdSphere(spherePos, influences[3]), 0);
//    return closest(box, sphere);


    p.zy *= r;
    p.xz *= r;
    float d = sdBox(p,vec3(1));

    float s = 2.0;
    for( int m=0; m<4; m++ )
    {
        vec3 a = mod( p*s, 2.0 )-1.0;
        s *= 3.0;
        vec3 r = 1.0 - 3.0*abs(a);

        float c = sdCross(r)/s;
        d = max(d,c);
    }
    return closest(vec2(d,2),closest(sphere,sphere2));

    return vec2(d,2);

}

vec2 closestObject2(vec3 p){
    vec3 spherePos = p;
    spherePos.x -= influences[2];

    return vec2(sdSphere(spherePos, influences[3]), 2);
}


// https://michaelwalczyk.com/blog-ray-marching.html
vec3 GetSurfaceNormal(vec3 p)
{
    const float eps = 0.0001;
    const vec2 h = vec2(eps, 0);
    return normalize(vec3(closestObject(p+h.xyy).x-closestObject(p-h.xyy).x,
    closestObject(p+h.yxy).x-closestObject(p-h.yxy).x,
    closestObject(p+h.yyx).x-closestObject(p-h.yyx).x));
}

vec3 marchRay(vec3 p, vec3 rd, float distance){
    return p + (rd*distance);
}


void main(){

    mat2 r45 = rot2D(-0.785398+influences[2]);
    vec3 mirrorNormal = vec3(1,0,0);
    mirrorNormal.xz*r45;

    float distanceTraveled = 0;
    vec3 ro = vec3(uv.x, uv.y, -3);
    vec3 rd = normalize(vec3(uv, 3));
    vec3 color = vec3(0);
    int stepCount = 0;
    int bounces = 0;
    vec2 closest = vec2(0,0);
    while (stepCount < maximumSteps){
        vec3 p = marchRay(ro, rd, distanceTraveled);

//        if(bounces==0){
//            closest = closestObject2(p);
//        }else{
//            closest = closestObject(p);
//        }
        closest = closestObject(p);

        distanceTraveled += closest.x;
        if(distanceTraveled < 0){
            color = vec3(1+pulse*2,0,1+pulse*2)/4;
            break;
        }
        if (closest.x <= mininumDistance){
            //hit
            if (closest.y == 0 && bounces<3){
//                color = vec3(1, 1, 1);
                ro = p;
                rd = reflect(rd, GetSurfaceNormal(p));
                distanceTraveled=0.01;
                bounces++;

            }
            if(closest.y == 1){
                float col = (1/distanceTraveled);
                color = vec3(col+0.2,0,0.2+col*pulse);
                break;
            }
            if(closest.y == 2){
//                float col = (1/distanceTraveled);
                vec3 normal = GetSurfaceNormal(p);
                color = vec3(abs(normal.x),abs(normal.y),abs(normal.z));
                break;
            }

        }
        if (distanceTraveled >= maximumDistance){
            break;
        }
        stepCount++;

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
