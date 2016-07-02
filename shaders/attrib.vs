#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec4 splatAxes;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;
uniform float radiusFactor;
//uniform sampler2DShadow texDepth;
uniform sampler2D texDepth;


out vec3 vNormal;
out vec3 vSplatAxis;
out float major2minor;
out vec3 vColor;
out int visible;

void main()
{
	// to viewspace
	vColor = color;
	vec4 vPos = viewMatrix * vec4(position, 1);	
	vNormal = normalMatrix * normal.xyz;

	// flip normals to viewer
	if (dot(vPos.xyz, vNormal) < 0)
		vNormal = -vNormal;
			
	vec4 proj = projectionMatrix * vPos;
	vec2 ndc = (proj.xy / proj.w) * 0.5 + 0.5;
	//float d = texture(texDepth, vec3(ndc, proj.z/proj.w));
	float d = texture(texDepth, ndc).r;
	
	//vColor = vec3(d);
	vColor = abs(vNormal);
	//vColor = vec3(ndc, 0);
	
	visible = 0;
	if ((proj.z/proj.w+1)/2 < d + 0.01)
		visible = 1;
	
	vSplatAxis.xyz = normalMatrix * splatAxes.xyz;
	vSplatAxis.xyz *= radiusFactor;
	major2minor = splatAxes.w;

	gl_Position = vPos;
}

