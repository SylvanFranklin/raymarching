#version 330 core
uniform bool clicked;
uniform float time;
uniform vec4 influences;

in vec2 uv;
out vec4 FragColor;

const float maximumDistance = 100;
const float mininumDistance = 0.001;
const int maximumSteps = 60;

// Utils

vec3 twist(vec3 p) {
    const float k = 0.2;
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
    return length(p)-r;
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


float map(vec3 ro, vec3 rd, float distanceTraveled){
    vec3 p = ro + (rd * distanceTraveled);
    vec3 q = twist(vec3(p.x - influences[3], p.y, p.z));

    q.yz *= rot2D(influences[0] + time / 10);
    q.xz *= rot2D(influences[1] + time / 30);
    q.xy *= rot2D(influences[2] - time / 80);

    float distance = sdBoxFrame(repeat(q, 0.42), vec3(0.2, 0.2, 0), 1);
    return distance;
}


void main() {
    float distanceTraveled = 0;
    vec3 ro = vec3(uv.x, uv.y, -3);
    vec3 rd = normalize(vec3(uv, 1));
    vec3 color = vec3(0);

    for (int i = 0; i < maximumSteps; i++)
    {
        distanceTraveled += map(ro,rd, distanceTraveled);

        if(distanceTraveled>maximumDistance){
            color = vec3(1,0,0);
        }else{
            float col = (1/distanceTraveled);
            color = vec3(col/4,0,col/20);
        }
    }
    FragColor = vec4(color, 1);
}
