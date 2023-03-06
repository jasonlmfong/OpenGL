#shader vertex
#version 450 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_Position;
out vec2 v_TexCoord;

uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP * position;
	v_TexCoord = texCoord;
	v_Position = position.xy;
};



#shader fragment
#version 450 core

layout(location = 0) out vec4 fragColor;

in vec2 v_Position;
in vec2 v_TexCoord;

uniform vec4 u_Color;
uniform float u_Thickness;
uniform sampler2D u_Texture;

void main()
{
	vec2 uv = (v_Position / 50.0);

	float fade = 0.005;

	float distance = 1.0 - length(uv);
	// outer circle
	vec3 col = vec3(smoothstep(0.0, fade, distance));
	// inner negative circle
	col *= vec3(1.0 - smoothstep(u_Thickness, u_Thickness + fade, distance));

	fragColor = texture(u_Texture, v_TexCoord) * u_Color * vec4(col, 1.0f);
};