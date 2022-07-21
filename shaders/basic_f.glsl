#version 330 core
out vec4 FragColor;

in vec3 f_pos;
in vec3 f_norm;
in vec3 f_col;

uniform vec3 cam_pos;
uniform samplerCube skybox;

void main()
{
    float ratio = 1. / 1.52;
    vec3 I = normalize(f_pos - cam_pos);
    vec3 R = refract(I, normalize(f_norm), ratio);
    FragColor = vec4(mix(texture(skybox, R).rgb, f_col, .2), 1.);
}

