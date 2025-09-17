#version 330 core
uniform bool clicked;
uniform float time;
uniform vec4 influences;
uniform float pulse;

in vec2 uv;
out vec4 FragColor;

const float maximumDistance = 10;
const float mininumDistance = 0.001;
const int maximumSteps = 60;

// Utils

vec3 reflect(vec3 ro, vec3 rd, float distanceTo, vec3 normal, float totalDistance){
    vec3 reflected = ro + (rd * distanceTo) + normal*(totalDistance-distanceTo);
    return reflected;
}

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

float sdBox( vec3 p, vec3 b )
{
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
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

// https://iquilezles.org/articles/normalsSDF
vec3 GetSurfaceNormal(vec3 p)
{
    float d0 = sdBox(p, vec3(0, 1, 1));
    const vec2 epsilon = vec2(.001,0);
    vec3 d1 = vec3(
    sdBox(p-epsilon.xyy,vec3(0,1,1)),
    sdBox(p-epsilon.yxy,vec3(0,1,1)),
    sdBox(p-epsilon.yyx,vec3(0,1,1)));
    return normalize(d0 - d1);
}


void main() {
    float distanceTraveled = 0;
    vec3 ro = vec3(uv.x, uv.y, -3);
    vec3 rd = normalize(vec3(uv, 3));
    vec3 color = vec3(0);
    int stepCount = 0;
    int bounces = 0;

    float col = 0;
    float draw = 0;

    bool notReflected = true;

    while(stepCount < maximumSteps){
        vec3 p = ro + (rd * distanceTraveled);

        vec3 spherePos = p;
        spherePos.x -= 1;
        float sphere = sdSphere(spherePos,0.2);

        mat2 r = rot2D(0.785398 + influences[3]);
        p.xz *= r;
        vec3 normal = vec3(1,0,0);
        normal.xz*=r;

        float mirror = sdBox(p, vec3(0, 1, 1));

        draw = min(sphere, mirror);


        float distance = draw;

        distanceTraveled += distance;

        if(distance <= mininumDistance){ // hit
            if(draw == mirror){
                ro = ro + rd * distanceTraveled;
                distanceTraveled = 0;
                rd = reflect(rd,GetSurfaceNormal(p));
                color = vec3(0.5,0,0.5);
                notReflected = false;
            }else{
                color = vec3(1,0,1);
                break;
            }

        }
        if(distanceTraveled>=maximumDistance){
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
