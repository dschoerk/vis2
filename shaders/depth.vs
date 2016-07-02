#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec4 splatAxes;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;
uniform float radiusFactor;

out int visible;
out vec3 vNormal;
out vec3 vSplatAxis;
out float major2minor;
out vec3 vColor;

void main()
{
	// to viewspace
	vColor = color;
	vec4 vPos = viewMatrix * vec4(position, 1);	
	vNormal = normalMatrix * normal.xyz;

	// flip normals to viewer
	if (dot(vPos.xyz, vNormal) < 0)
		vNormal = -vNormal;

	vSplatAxis.xyz = normalMatrix * splatAxes.xyz;
	vSplatAxis.xyz *= radiusFactor;
	major2minor = splatAxes.w;

	gl_Position = vPos;
	visible = 1;
}

