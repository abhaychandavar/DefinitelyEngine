#shader vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normals;
layout(location = 2) in vec2 a_TexCoords;

out vec3 v_Normal;
out vec2 v_TexCoords;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
    // Pass normal in world space (assumes uniform scale)
    v_Normal    = mat3(transpose(inverse(u_Transform))) * a_Normals;
    v_TexCoords = a_TexCoords;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_Normal;
in vec2 v_TexCoords;

uniform sampler2D u_Texture;
uniform int       u_HasTexture;

const vec3  lightDir = vec3(0.577, 0.577, 0.577); // normalized (1,1,1)
const float ambient  = 0.2;

void main()
{
    vec4 baseColor = (u_HasTexture == 1)
        ? texture(u_Texture, v_TexCoords)
        : vec4(0.8, 0.8, 0.8, 1.0);

    float diffuse  = max(dot(normalize(v_Normal), lightDir), 0.0);
    float lighting = ambient + (1.0 - ambient) * diffuse;

    color = vec4(baseColor.rgb * lighting, baseColor.a);
}
