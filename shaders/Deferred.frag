/*
Lighting pass fragment shader (second pass)
*/
#version 400

in VertexData {
    vec2 texCoords;
} vertexIn;

uniform sampler2D textureAlbedo;
uniform sampler2D textureNormal;
uniform sampler2D textureMetallic;
uniform sampler2D textureRougness;


uniform sampler2D tDepth;

uniform mat4 inverseProjectionMatrix;

struct Light {
    vec4 position;
    vec3 intensities;
    float attenuation;
    float ambientCoefficient;
    float coneAngle;
    vec3 direction;
};

const int maxLights = 32;
uniform int lightCount;
uniform Light lights[maxLights];

layout(location = 0) out vec4 fragmentColor;
layout(location = 1) out vec4 extraOut;

const float PI = 3.14159265359f;
const float GAMMA = 2.2f;

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    cosTheta = 1.0f - cosTheta;
    return F0 + (1.0f - F0) * cosTheta * cosTheta * cosTheta * cosTheta * cosTheta;
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom =(NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;
    
    return clamp((nom / denom), 0.0f, 5.5f);
}

float GemetrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0f);
    float k = (r*r) / 8.0f;

    float nom = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return nom/denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N,V), 0.0f);
    float NdotL = max(dot(N,L), 0.0f);
    float ggx1 = GemetrySchlickGGX(NdotV, roughness);
    float ggx2 = GemetrySchlickGGX(NdotL, roughness);

    return clamp((ggx1 * ggx2), 0.f, 1.0f);
}

vec3 ReconstructPos(vec2 texCoord, float depth) {
    vec4 sPos = vec4(texCoord * 2.0f - 1.0f, depth * 2.0f - 1.0f, 1.0f);
    sPos = inverseProjectionMatrix * sPos;

    return (sPos.xyz / sPos.w);
}

    float depth = texture(tDepth, vertexIn.texCoords).r;
    vec3 albedo = texture(textureAlbedo, vertexIn.texCoords).rgb;
    //vec3 albedo = pow(albedoRaw, vec3(GAMMA)); // Apply if texture not in sRGB
    vec3 normal = normalize(texture(textureNormal, vertexIn.texCoords).rgb);
    float metallic = texture(textureMetallic, vertexIn.texCoords).r;
    float roughness = texture(textureRougness, vertexIn.texCoords).r;
    vec3 pos = ReconstructPos(vertexIn.texCoords, depth);

vec3 applyLights() {
    vec3 Lo = vec3(0.0f);
    vec3 N = normalize(normal);
    vec3 V = normalize(-pos);
    
    vec3 F0 = mix(vec3(0.04f), albedo, metallic);
    
    for(int i = 0; i < lightCount; i++) {
        vec3 surfaceToLight;
        float attenuation;
        vec3 ambient;

        if (lights[i].position.w == 0.0f) {
            //Directional light.
            surfaceToLight = normalize(lights[i].position.xyz);
            attenuation = 1.0f;
        } else {
            // Point light
            vec3 toLight = lights[i].position.xyz - pos;
            surfaceToLight = normalize(toLight);
            attenuation = 1.0f / (1.0f + lights[i].attenuation * (toLight.x * toLight.x + toLight.y * toLight.y + toLight.z * toLight.z));

            // Spot light.
            float lightToSurfaceAngle = degrees(acos(clamp(dot(-surfaceToLight, normalize(lights[i].direction)), -1.0f, 1.0f)));
            if (lightToSurfaceAngle > lights[i].coneAngle) {
                attenuation = 0.0f;
            }
        }

        vec3 H = normalize(V + surfaceToLight);

        // Calculate radiance of the light.
        vec3 radiance = lights[i].intensities * attenuation;
        
        // Cook-torrance brdf.
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, surfaceToLight, roughness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0f), F0);
        
        // Calculate specular.
        vec3 nominator = NDF * G * F;
        float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, surfaceToLight), 0.0f) + 0.001f;
        vec3 specular = (nominator / denominator);
        
        // Energy of light that gets reflected.
        vec3 kS = F;
        
        // Energy of light that gets refracted (no refraction occurs when metallic).
        vec3 kD = (vec3(1.0f) - kS) * (1.0f - metallic);
        
        // Calculate light contribution.
        float NdotL = max(dot(N, surfaceToLight), 0.0f);
        
        // Add refraction.
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
        
        // Add ambient.
        Lo += lights[i].ambientCoefficient * albedo;
    }

    return Lo;
}


void main() {

    vec3 color = applyLights();

    // Reinhard tone mapping
    color = clamp(color / (color + vec3(1.0f)), 0.0f, 1.0f);
    
    // Gamma correction
    color = pow(color, vec3(1.0f / GAMMA)); 

    fragmentColor = vec4(color, 1.0f);
}


