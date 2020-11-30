#version 430 core

out vec4 color;

in VS_OUT
{
    vec2 tc;
    vec4 normals;
	vec4 fragPos;
	vec3 fPosition;
} fs_in;

uniform sampler2D tex;
uniform samplerCube skybox;
uniform uint texSwitch;
uniform vec4 lightPosition;
uniform vec4 viewPosition;
uniform vec4 ia;		// Ambient colour
uniform float ka;		// Ambient constant
uniform vec4 id;		// diffuse colour
uniform float kd;		// Diffuse constant
uniform vec4 is;		// specular colour
uniform float ks;		// specular constant
uniform float shininess;// shininess constant
uniform float lightConstant;
uniform float lightLinear;
uniform float lightQuadratic;
uniform vec4 objcolor;

void main(void){
	// Ambient
	vec3 ambient = ka * ia.rgb;

	// Diffuse
	vec4 lightDir = normalize(lightPosition - fs_in.fragPos);
	float diff = max(dot(normalize(fs_in.normals), lightDir), 0.0);
	vec3 diffuse = kd * id.rgb * diff;

	// Specular
	vec4 viewDir = normalize(viewPosition - fs_in.fragPos);
	vec4 reflectDir = reflect(-lightDir, normalize(fs_in.normals));
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = ks * is.rgb * spec;

	// Attenuation
	float distance = length(lightPosition - fs_in.fragPos);
	float attenuation = 1.0f / (lightConstant +
	lightLinear * distance +
	lightQuadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;


	//if texture is 0 then use standart lighting, else use texture with appropriate lighting
	if(texSwitch == 0){
		color = vec4(ambient + diffuse + specular, 1.0) * objcolor;

	} else if (texSwitch == 1) {
		color = vec4(ambient + diffuse + specular, 1.0) * texture(tex, fs_in.tc);
	} else if (texSwitch == 2) {
		color = texture(skybox, fs_in.fPosition);
	}
}
