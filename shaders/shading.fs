#version 330

in vec2 frag_uv;
in vec2 frag_Pos;

uniform mat4 viewWorldTrans; // inverse world-view-projection transform

uniform sampler2D gb_color;
uniform sampler2D gb_normal;
uniform sampler2D gb_weight;
uniform sampler2D gb_depth;

uniform vec3 camPos;
uniform vec3 lightPos;
uniform vec3 lightColor;

const float k_a = 0.5; // ambient coefficient
const float k_d = 0.5; // diffuse coefficient
const float k_s = 0.7; // specular reflection coefficient
const float phongExp = 3; // specular parameter

out vec3 color_out;

void main() {
	vec4 diffuseColor_w = texture(gb_color, frag_uv).rgba;
	vec4 normal_w = texture(gb_normal, frag_uv).rgba;
	float weight = texture(gb_weight, frag_uv).r;
	float depth = texture(gb_depth, frag_uv).r;

	vec3 normal = normalize(normal_w.rgb);
	vec3 diffuseColor = diffuseColor_w.rgb / weight;
	diffuseColor = abs(normal_w.rgb / weight);
	
	//diffuseColor = diffuseColor / weight;

	// viewport position of current fragment in [-1,1]
	vec4 H = vec4(frag_Pos.x, frag_Pos.y * 2 - 1, depth * 2 - 1, 1);
	// world position of current fragment
	vec4 D = viewWorldTrans * H;
	vec4 worldPos = D / D.w;

	vec3 lightVector = normalize(lightPos.xyz - worldPos.xyz);
	vec3 halfWay = normalize(lightVector + normalize(camPos - worldPos.xyz));
	float diffuseI = max(dot(lightVector, normal), 0);

	//diffuse
	color_out = k_a * diffuseColor.rgb + k_d * diffuseI * lightColor * diffuseColor.rgb;

	//+specular
	color_out = color_out + k_s * pow(max(dot(halfWay, normal), 0), phongExp) * lightColor * diffuseColor.rgb;
	
	//color_out = abs(normal);
	//color_out = diffuseColor;
}