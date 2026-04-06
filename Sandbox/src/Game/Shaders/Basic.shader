#shader vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normals;
layout(location = 2) in vec2 a_TexCoords;

out vec2 v_TexCoords;
out vec3 v_Normals;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
    v_TexCoords = a_TexCoords;
    v_Normals = a_Normals;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoords;
in vec3 v_Normals;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

vec3 lightDir = normalize(vec3(1.0, 1.0, 0.5));
float ambient  = 0.15;

void main()
{
    vec4 texColor = u_Color;
    float diffuse  = max(dot(normalize(v_Normals), lightDir), 0.0);
    float lighting = ambient + diffuse;
    color = vec4(u_Color.rgb * lighting, u_Color.a);
};
