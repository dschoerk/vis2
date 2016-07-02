#version 330
	
layout(points) in;
layout(triangle_strip, max_vertices=4) out;
	
uniform mat4 projectionMatrix;

in int visible[];
in vec3 vNormal[];
in vec3 vSplatAxis[];
in vec3 vColor[];
in float major2minor[];

out vec3 gNormal;
out vec3 gCol;
out vec2 gUV;
out float gDepth;
out vec2 gAxis;


void main()
{
	vec4 viewspace_pos = vec4( gl_in[0].gl_Position.xyz, 1 );
	
	// Major Axis
	vec4 offsetMajor = 0.001 * vec4(0,1,0,0);
	
	// Minor Axis
	vec4 offsetMinor = 0.001 * vec4(1,0,0,0);
	
	// Major Axis
	offsetMajor =  vec4(vSplatAxis[0].xyz, 0); 
	// Minor Axis
	offsetMinor = major2minor[0] * vec4(cross(vSplatAxis[0].xyz, vNormal[0]), 0);
	
	vec2 axisLen = vec2(length(offsetMajor), length(offsetMinor));
	
	if(visible[0] == 1)
	{
		vec4 offPos0 = (viewspace_pos + offsetMinor + offsetMajor);
		gl_Position = projectionMatrix * offPos0;
		gNormal = vNormal[0];
		gUV = vec2(1,1);
		gDepth = offPos0.z/offPos0.w;
		gCol = vColor[0];
		gAxis = axisLen;
		EmitVertex();
		
		vec4 offPos1 = (viewspace_pos + offsetMinor - offsetMajor);
		gl_Position = projectionMatrix * offPos1;
		gNormal = vNormal[0];
		gUV = vec2(-1,1);
		gDepth = offPos1.z/offPos1.w;
		gCol = vColor[0];
		gAxis = axisLen;
		EmitVertex();
		
		vec4 offPos2 = (viewspace_pos - offsetMinor + offsetMajor);
		gl_Position = projectionMatrix * (viewspace_pos - offsetMinor + offsetMajor);
		gNormal = vNormal[0];
		gUV = vec2(1,-1);
		gDepth = offPos2.z/offPos2.w;
		gCol = vColor[0];
		gAxis = axisLen;
		EmitVertex();
		
		vec4 offPos3 = (viewspace_pos - offsetMinor - offsetMajor);
		gl_Position = projectionMatrix * offPos3;
		gNormal = vNormal[0];
		gUV = vec2(-1,-1);
		gDepth = offPos3.z/offPos3.w;
		gCol = vColor[0];
		gAxis = axisLen;
		EmitVertex();
	}
}