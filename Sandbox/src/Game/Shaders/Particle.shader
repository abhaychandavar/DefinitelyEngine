#shader vertex
#version 330 core

layout(location = 0) in vec3  a_Position;
layout(location = 1) in vec4  a_Color;
layout(location = 2) in float a_Size;

out vec4 v_Color;

uniform mat4 u_ViewProjection;

void main() {
    gl_Position  = u_ViewProjection * vec4(a_Position, 1.0);
    gl_PointSize = a_Size;
    v_Color      = a_Color;
}

#shader fragment
#version 330 core

in  vec4 v_Color;
out vec4 fragColor;

void main() {
    fragColor = v_Color;
}
