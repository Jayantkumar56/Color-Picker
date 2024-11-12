

#type Vertex
#version 450 core

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec2 a_TextureCoord;

uniform mat4 u_ViewProjection;

out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TextureCoord;
	gl_Position = u_ViewProjection * a_Position;
}

#type fragment
#version 450 core

out vec4 o_Color;
in vec2 v_TexCoord;

uniform sampler2D u_ImageTexSlot;

void main()
{
	vec4 texColor = texture(u_ImageTexSlot, v_TexCoord);
	o_Color = texColor;
}
