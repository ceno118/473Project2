#version 330 core

out vec4 FragColor;

in vec2 texture_coordinates;
in vec3 normal_vector;
in vec4 fragment_position;
in vec3 color_vec;
in vec3 spec_color_vec;

uniform bool use_color;
uniform vec4 set_color;
uniform bool use_kernel;

uniform vec4 eye_position;

struct PointLight {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 position;

    float constant;
    float linear;
    float quadratic;

    bool on;
};

struct DirectionLight {
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    bool on;
};

struct SpotLight {
    vec4 position;
    vec4 direction;
    float cutoff;
    float outer_cutoff;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float constant;
    float linear;
    float quadratic;

    bool on;
};

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

Material material;

uniform PointLight point_light;
uniform DirectionLight direction_light;
uniform SpotLight spot_light;

uniform sampler2D a_texture;
uniform bool use_texture;

vec4 CalcPointLight (PointLight light, vec4 fragment, vec3 norm, vec4 eye);
vec4 CalcDirLight (DirectionLight light, vec4 fragment, vec3 norm, vec4 eye);
vec4 CalcSpotLight(SpotLight light,vec4 fragment, vec3 norm, vec4 eye);
float Attenuate(float distance, float constant, float linear, float quadratic);

void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
    
    
    
    
    if (use_color){
        FragColor = set_color;
        return;
    }
    material.ambient = vec4(color_vec,1.0);
    material.diffuse = vec4(color_vec,1.0);
    material.specular = vec4(spec_color_vec,1.0);
    material.shininess = 256;

    vec4 point_light_vec = vec4(0.0,0.0,0.0,1.0);
    vec4 dir_light_vec = vec4 (0.0,0.0,0.0,1.0);
    vec4 spot_light_vec = vec4 (0.0,0.0,0.0,1.0);
    if (point_light.on)
    {
        point_light_vec = CalcPointLight(point_light,
                                         fragment_position,
                                        normal_vector,
                                        eye_position);
    }
    if (direction_light.on) {
        dir_light_vec = CalcDirLight(direction_light,
                                        fragment_position,
                                        normal_vector,
                                        eye_position);
    }
    if (spot_light.on)
    {
      spot_light_vec = CalcSpotLight(spot_light,
                                        fragment_position,
                                        normal_vector,
                                        eye_position);

    }
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        //sampleTex[i] = vec4(texture(a_texture, texture_coordinates + offsets[i]));
        sampleTex[i] = vec4(point_light_vec + dir_light_vec + spot_light_vec) 
                * texture(a_texture, texture_coordinates);
    }
    vec4 col = vec4(0.0, 0.0, 0.0, 1.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
        
    if (!use_kernel) {
        FragColor = (point_light_vec + dir_light_vec + spot_light_vec) 
                * texture(a_texture, texture_coordinates)
        return;
    }
    else if (use_kernel){
        FragColor =  col;
        return;
    }
    
    FragColor = point_light_vec+dir_light_vec+spot_light_vec

    
}

vec4 CalcPointLight (PointLight light, vec4 fragment, vec3 sn, vec4 eye)
{
    vec3 light_direction = normalize(vec3(light.position) - vec3(fragment));
    sn = normalize(sn);
    float diff_coefficient = max(dot(sn,light_direction),0.0);

    vec3 view_direction = normalize(vec3(eye) - vec3(fragment));
    vec3 reflect_direction = reflect(-light_direction,sn);
    float spec_coefficient = pow(max(dot(view_direction,reflect_direction),0.0),256);

    vec4 light_color = light.ambient 
                + diff_coefficient * light.diffuse 
                + spec_coefficient * light.specular;
    if (!use_texture) {
        light_color = light.ambient * material.ambient
                     + diff_coefficient * light.diffuse * material.diffuse
                     + spec_coefficient * light.specular * material.specular;
    }
    float attenuation = Attenuate(length(light.position-fragment),
                                  light.constant,
                                  light.linear,
                                  light.quadratic);
    return vec4(vec3(attenuation * light_color),1.0);

}

vec4 CalcDirLight (DirectionLight light, vec4 fragment, vec3 sn, vec4 eye)
{
    vec3 light_direction = normalize(-vec3(light.direction));
    sn = normalize(sn);
    float diff_coefficient = max(dot(sn,light_direction),0.0);

    vec3 view_direction = normalize(vec3(eye) - vec3(fragment));
    vec3 reflect_direction = reflect(-light_direction,sn);
    float spec_coefficient = pow(max(dot(view_direction,reflect_direction),0.0),256);

    vec4 light_color = light.ambient 
                + diff_coefficient * light.diffuse 
                + spec_coefficient * light.specular;
    if (!use_texture) {
        light_color = light.ambient * material.ambient
                     + diff_coefficient * light.diffuse * material.diffuse
                     + spec_coefficient * light.specular * material.specular;
    }
    return light_color;

}


vec4 CalcSpotLight (SpotLight light, vec4 fragment, vec3 sn, vec4 eye)
{
    vec3 light_direction = normalize(vec3(light.position) - vec3(fragment));
    float theta = dot(light_direction, normalize(vec3(-light.direction)));
    
    float epsilon = (light.cutoff - light.outer_cutoff);
    float intensity = clamp((theta-light.outer_cutoff)/epsilon,0.0,1.0);

    sn = normalize(sn);
    float diff_coefficient = max(dot(sn,light_direction),0.0);

    vec3 view_direction = normalize(vec3(eye) - vec3(fragment));
    vec3 reflect_direction = reflect(-light_direction,sn);
    float spec_coefficient = pow(max(dot(view_direction,reflect_direction),0.0),256);

    vec4 light_color = light.ambient 
                + diff_coefficient* intensity * light.diffuse 
                + spec_coefficient* intensity * light.specular;
    if (!use_texture) {
        light_color = light.ambient * material.ambient
                     + diff_coefficient * intensity * light.diffuse * material.diffuse
                     + spec_coefficient * intensity * light.specular * material.specular;
    }
    float attenuation = Attenuate(length(light.position-fragment),
                                  light.constant,
                                  light.linear,
                                  light.quadratic);
    return vec4(vec3(attenuation * light_color),1.0);

}

float Attenuate(float distance, float constant, float linear, float quadratic)
{
   
    float attenuation = 1.0 / (constant + linear * distance + 
    		    quadratic * (distance * distance));  
    return attenuation;  
}