#version 330 core
out vec4 FragColor;

in vec3 f_pos;
in vec3 f_norm;
in vec3 f_col;
in vec2 f_tc;

uniform vec3 cam_pos;
uniform samplerCube skybox;
uniform sampler2D norm_map;
uniform bool use_normal_map;

void main()
{
    vec3 ambient = .2 * f_col;

    vec3 norm;

    if (use_normal_map)
    {
        vec3 rgb_norm = texture(norm_map, f_tc).rgb;
        norm = normalize(rgb_norm * 2. - 1.);
    }
    else
    {
        norm = normalize(f_norm);
    }

    /* vec3 new_pos = f_pos; */
    vec3 new_pos = f_pos + norm;

    vec3 ldir = normalize(vec3(-1., .2, .0));
    float diff = max(dot(norm, ldir), 0.);
    vec3 diffuse = .7 * diff * f_col;

    vec3 view_dir = normalize(cam_pos - new_pos);
    vec3 reflect_dir = reflect(-ldir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.), 100.f);
    vec3 specular = 1. * spec * f_col;

    vec3 result = ambient + diffuse + specular;

    float ratio = 1. / 1.1;
    vec3 I = normalize(new_pos - cam_pos);
    vec3 R = refract(I, normalize(f_norm), ratio);

    FragColor = vec4(mix(texture(skybox, R).rgb, result, .3), 1.);
}

