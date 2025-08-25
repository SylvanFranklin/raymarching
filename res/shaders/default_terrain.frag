#version 330 core

out vec4 FragColor;
in vec3 pos;
uniform vec2 mouse;
uniform vec4 influences;
uniform float offset;
uniform float shufflah;
uniform vec2 ratio;
uniform float cloudSlide;

vec3 lightWater = vec3(0, 63, 178);
vec3 deepWater = vec3(15, 81, 198);
vec3 lightSand = vec3(192, 181, 131);
vec3 mediumSand = vec3(166, 150, 99);
vec3 darkSand = vec3(113, 98, 49);
vec3 mediumForest = vec3(90, 127, 50);
vec3 darkForest = vec3(60, 97, 20);
vec3 deepForest = vec3(40, 77, 0);
vec3 lightForest = vec3(122, 158, 82);
vec3 darkRock = vec3(141, 140, 122);
vec3 lightRock = vec3(160, 164, 145);
vec3 snow = vec3(235, 235, 235);

float whitenoise(vec2 p) {
    float random = dot(p + offset, ratio);
    random = sin(random);
    random = random * shufflah;
    random = fract(random);
    return random;
}

float octive_noise(float octives) {
    vec2 uv = gl_FragCoord.xy;
    uv = uv / vec2(1512, 982);
    uv = uv * octives;
    vec2 griduv = fract(uv);
    vec2 gridid = floor(uv);

    griduv = smoothstep(0, 1, griduv);

    float botleft = whitenoise(gridid);
    float botright = whitenoise(gridid + vec2(1.0, 0.0));
    float b = mix(botleft, botright, griduv.x);

    float topleft = whitenoise(gridid + vec2(0.0, 1.0));
    float topright = whitenoise(gridid + vec2(1.0, 1.0));
    float t = mix(topleft, topright, griduv.x);
    float valueNoise = mix(b, t, griduv.y);

    return valueNoise;
}


float cloud_whitenoise(vec2 p) {
    float random = dot(p, vec2(12,78));
    random = sin(random);
    random = random * 29837.23;
    random = fract(random);
    return random;
}

float cloud_octive_noise(float octives) {
    vec2 uv = gl_FragCoord.xy;
    uv = uv / vec2(1512, 982);
    uv = uv * octives;
    vec2 griduv = fract(uv);
    vec2 gridid = floor(uv);

    griduv = smoothstep(0, 1, griduv);

    float botleft = cloud_whitenoise(gridid);
    float botright = cloud_whitenoise(gridid + vec2(1.0, 0.0));
    float b = mix(botleft, botright, griduv.x);

    float topleft = cloud_whitenoise(gridid + vec2(0.0, 1.0));
    float topright = cloud_whitenoise(gridid + vec2(1.0, 1.0));
    float t = mix(topleft, topright, griduv.x);
    float valueNoise = mix(b, t, griduv.y);

    return valueNoise;
}

float clouds() {
    float cloud = cloud_octive_noise(16+cloudSlide)*0.3;
    cloud += cloud_octive_noise(32+cloudSlide)*0.2;
    cloud += cloud_octive_noise(64)*0.01;
    cloud += cloud_octive_noise(128)*0.1;

    return cloud;
}


void main() {
    float water_strength = 0.55;
    float sand_strength = 0.65;
    float dirt_strength = 0.69;
    float forest_strength = 0.75;
    float rock_strength = 0.95;
    vec2 uv = gl_FragCoord.xy;
    vec3 color = vec3(0, 62, 178);

    float valueNoise = octive_noise(8.0) * 0.4 * influences[0];

    for (int i = 4; i < floor(40 * influences[3]); i++) {
        valueNoise += octive_noise(4.0 * influences[1] * i * i) * (1.2 / (i * i * influences[2]));
    }

    if (valueNoise < 0.35) {
        color = lightWater;
    } else if (valueNoise < 0.38) {
        color = deepWater;
    } else if (valueNoise < 0.40) {
        color = lightSand;
    } else if (valueNoise < 0.43) {
        color = mediumSand;
    } else if (valueNoise < 0.45) {
        color = darkSand;
    } else if (valueNoise < 0.47) {
        color = mediumForest;
    } else if (valueNoise < 0.53) {
        color = darkForest;
    } else if (valueNoise < 0.56) {
        color = mediumForest;
    } else if (valueNoise < 0.57) {
        color = lightForest;
    } else if (valueNoise < 0.59) {
        color = lightForest;
    } else if (valueNoise < 0.62) {
        color = darkRock;
    } else if (valueNoise < 0.66) {
        color = lightRock;
    } else {
        color = snow;
    }
    vec4 theColor = vec4(color / 255.0, 1);
    float cloud = clouds();

    if (cloud > 0.4 && valueNoise < 0.54){
        theColor = vec4(cloud+0.4,cloud+0.4,cloud+0.4,1);
    }

    FragColor = theColor;
}
