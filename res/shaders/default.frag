#version 330 core
out vec4 FragColor;
in vec2 uv;
in vec4 dials;

vec3 repeat(vec3 p, float spacing) {
    return mod(p + spacing * 0.5, spacing) - spacing * 0.5;
}

float sdTorus(vec3 p, vec2 t) {
    vec2 q = vec2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}

mat2 rot2D(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat2(c, -s, s, c);
}

float sdBoxFrame(vec3 p, vec3 b, float e) {
    p = abs(p) - b;
    vec3 q = abs(p + e) - e;
    return min(min(
            length(max(vec3(p.x, q.y, q.z), 0.0)) + min(max(p.x, max(q.y, q.z)), 0.0),
            length(max(vec3(q.x, p.y, q.z), 0.0)) + min(max(q.x, max(p.y, q.z)), 0.0)),
        length(max(vec3(q.x, q.y, p.z), 0.0)) + min(max(q.x, max(q.y, p.z)), 0.0));
}

float sdSphere(vec3 p) {
    vec3 q = vec3(p.x - dials[3], p.y, p.z);
    q.yz *= rot2D(dials[0]);
    q.xz *= rot2D(dials[1]);
    q.xy *= rot2D(dials[2]);
    return length(repeat(q, 0.5)) - 0.1;
    //    return length(p)-0.2;
}

float smin(float a, float b, float k) {
    float h = max(k - abs(a - b), 0.0) / k;
    return min(a, b) - h * h * h * k * (1.0 / 6.0);
}

void main() {
    float distanceTraveled = 0;
    float maximumDistance = 10;
    float mininumDistance = 0.001;
    int maximumSteps = 80;
    vec3 ro = vec3(uv.x, uv.y, -3);
    vec3 rd = normalize(vec3(uv, 1));
    vec3 color = vec3(0);

    for (int i = 0; i < maximumSteps; i++)
    {
        vec3 p = ro + (rd * distanceTraveled);
        vec3 q = vec3(p.x - dials[3], p.y, p.z);
        q.yz *= rot2D(dials[0]);
        q.xz *= rot2D(dials[1]);
        q.xy *= rot2D(dials[2]);

        float boxFrame = sdBoxFrame(p, vec3(1.5, 1.5, 1.5), 0.1);
        float torus = sdTorus(p, vec2(0.25, 0.05));
        float sphere = sdSphere(p);

        float distance = smin(boxFrame, sphere, 0.5);
        distanceTraveled += distance;

        if (distanceTraveled > maximumDistance || distance <= mininumDistance) {
            if (distanceTraveled > maximumDistance) {
                color = vec3(0.2, 0.0, 0);
                break;
            }
            if (distance == boxFrame) {
                color = vec3(0, 0, distanceTraveled) / 3;
            } else if (distance == torus) {
                color = vec3(distanceTraveled, 0, 0) / 3;
            } else {
                color = vec3(distanceTraveled, 0, distanceTraveled) / 6;
            }
            break;
        }
    }
    FragColor = vec4(color, 1);
}
