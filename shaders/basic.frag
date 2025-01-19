#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;

out vec4 fColor;

// Matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceTrMatrix;

// Directional Light
uniform vec3 lightDir;       // Directional light direction
uniform vec3 lightColor;     // Directional light color

// Point Light
uniform vec3 pointLightPos;  // Point light position
uniform vec3 pointLightColor; // Point light color
uniform float constant = 0.8f;
uniform float linear = 0.29f;
uniform float quadratic = 0.003f;

// Textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

// Components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shadowFactor;
// Fog Settings
uniform int fogOn;         // Toggle for enabling/disabling fog
uniform bool fogEnabled;   // Toggle for enabling/disabling fog
uniform vec4 fogColor;     // Fog color
uniform float fogDensity;  // Fog density
uniform float fogFactor;   // Fog factor
uniform float fPosEye;      // Position of the eye

// Additional Point Lights
uniform vec3 pointLight1Position;
uniform vec3 pointLight1Color;

uniform vec3 pointLight2Position;
uniform vec3 pointLight2Color;

uniform vec3 pointLight3Position;
uniform vec3 pointLight3Color;

uniform vec3 pointLight4Position;
uniform vec3 pointLight4Color;

uniform vec3 pointLight5Position;
uniform vec3 pointLight5Color;


// Toggle for additional point lights
uniform bool pointLightsEnabled;

uniform bool isLake;


// Compute Directional Light
void computeDirLight()
{
    // Compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    // Normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    // Compute view direction (in eye coordinates, the viewer is at the origin)
    vec3 viewDir = normalize(-fPosEye.xyz);

    // Compute ambient light
    ambient = ambientStrength * lightColor;

    // Compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    // Compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32); // Shininess = 32
    specular = specularStrength * specCoeff * lightColor;
}

float computeShadow() {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; // Perspective division
    projCoords = projCoords * 0.5 + 0.5; // Transform to [0, 1] range

    float closestDepth = texture(shadowMap, projCoords.xy).r; // Depth in shadow map
    float currentDepth = projCoords.z;

    float bias = max(0.05 * (1.0 - dot(fNormal, lightDir)), 0.005); // Bias to avoid shadow acne

    float shadow = (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
    if (projCoords.z > 1.0 || projCoords.z < 0.0) shadow = 0.0; // Outside light frustum

    return shadow;
}


// Compute Point Light
void computePointLight(out vec3 pAmbient, out vec3 pDiffuse, out vec3 pSpecular)
{
    // Compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    // Compute light direction and distance to the light
    vec3 lightDir = normalize((view * vec4(pointLightPos, 1.0f)).xyz - fPosEye.xyz);
    float distance = length((view * vec4(pointLightPos, 1.0f)).xyz - fPosEye.xyz);

    // Compute view direction (viewer at the origin in eye space)
    vec3 viewDir = normalize(-fPosEye.xyz);

    // Compute attenuation
    float attenuation = 1.0 / (constant + linear * distance + quadratic * distance * distance);

    // Compute ambient light
    pAmbient = ambientStrength * pointLightColor * attenuation;

    // Compute diffuse light
    pDiffuse = max(dot(normalEye, lightDir), 0.0f) * pointLightColor * attenuation;

    // Compute specular light
    vec3 reflectDir = reflect(-lightDir, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32); // Shininess = 32
    pSpecular = specularStrength * specCoeff * pointLightColor * attenuation;
}

void computeFivePointLights(out vec3 pAmbient, out vec3 pDiffuse, out vec3 pSpecular) {
    // Initialize outputs
    pAmbient = vec3(0.0f);
    pDiffuse = vec3(0.0f);
    pSpecular = vec3(0.0f);

        // First Point Light
        vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
        vec3 normalEye = normalize(normalMatrix * fNormal);
        vec3 lightDir1 = normalize((view * vec4(pointLight1Position, 1.0f)).xyz - fPosEye.xyz);
        float distance1 = length((view * vec4(pointLight1Position, 1.0f)).xyz - fPosEye.xyz);
        vec3 viewDir = normalize(-fPosEye.xyz);
        float attenuation1 = 1.0 / (constant + linear * distance1 + quadratic * distance1 * distance1);
        pAmbient += ambientStrength * pointLight1Color * attenuation1;
        pDiffuse += max(dot(normalEye, lightDir1), 0.0f) * pointLight1Color * attenuation1;
        vec3 reflectDir1 = reflect(-lightDir1, normalEye);
        float specCoeff1 = pow(max(dot(viewDir, reflectDir1), 0.0f), 32); // Shininess = 32
        pSpecular += specularStrength * specCoeff1 * pointLight1Color * attenuation1;

        // Second Point Light
        vec3 lightDir2 = normalize((view * vec4(pointLight2Position, 1.0f)).xyz - fPosEye.xyz);
        float distance2 = length((view * vec4(pointLight2Position, 1.0f)).xyz - fPosEye.xyz);
        float attenuation2 = 1.0 / (constant + linear * distance2 + quadratic * distance2 * distance2);
        pAmbient += ambientStrength * pointLight2Color * attenuation2;
        pDiffuse += max(dot(normalEye, lightDir2), 0.0f) * pointLight2Color * attenuation2;
        vec3 reflectDir2 = reflect(-lightDir2, normalEye);
        float specCoeff2 = pow(max(dot(viewDir, reflectDir2), 0.0f), 32); // Shininess = 32
        pSpecular += specularStrength * specCoeff2 * pointLight2Color * attenuation2;

        // Third Point Light
        vec3 lightDir3 = normalize((view * vec4(pointLight3Position, 1.0f)).xyz - fPosEye.xyz);
        float distance3 = length((view * vec4(pointLight3Position, 1.0f)).xyz - fPosEye.xyz);
        float attenuation3 = 1.0 / (constant + linear * distance3 + quadratic * distance3 * distance3);
        pAmbient += ambientStrength * pointLight3Color * attenuation3;
        pDiffuse += max(dot(normalEye, lightDir3), 0.0f) * pointLight3Color * attenuation3;
        vec3 reflectDir3 = reflect(-lightDir3, normalEye);
        float specCoeff3 = pow(max(dot(viewDir, reflectDir3), 0.0f), 32); // Shininess = 32
        pSpecular += specularStrength * specCoeff3 * pointLight3Color * attenuation3;

        // Fourth Point Light
        vec3 lightDir4 = normalize((view * vec4(pointLight4Position, 1.0f)).xyz - fPosEye.xyz);
        float distance4 = length((view * vec4(pointLight4Position, 1.0f)).xyz - fPosEye.xyz);
        float attenuation4 = 1.0 / (constant + linear * distance4 + quadratic * distance4 * distance4);
        pAmbient += ambientStrength * pointLight4Color * attenuation4;
        pDiffuse += max(dot(normalEye, lightDir4), 0.0f) * pointLight4Color * attenuation4;
        vec3 reflectDir4 = reflect(-lightDir4, normalEye);
        float specCoeff4 = pow(max(dot(viewDir, reflectDir4), 0.0f), 32); // Shininess = 32
        pSpecular += specularStrength * specCoeff4 * pointLight4Color * attenuation4;

        // Fifth Point Light
        vec3 lightDir5 = normalize((view * vec4(pointLight5Position, 1.0f)).xyz - fPosEye.xyz);
        float distance5 = length((view * vec4(pointLight5Position, 1.0f)).xyz - fPosEye.xyz);
        float attenuation5 = 1.0 / (constant + linear * distance5 + quadratic * distance5 * distance5);
        pAmbient += ambientStrength * pointLight5Color * attenuation5;
        pDiffuse += max(dot(normalEye, lightDir5), 0.0f) * pointLight5Color * attenuation5;
        vec3 reflectDir5 = reflect(-lightDir5, normalEye);
        float specCoeff5 = pow(max(dot(viewDir, reflectDir5), 0.0f), 32); // Shininess = 32
        pSpecular += specularStrength * specCoeff5 * pointLight5Color * attenuation5;

    
}



void resetValue(vec3 param) {
	param = vec3(0.0f);
}

void main() 
{
    vec4 fragPosEye = view * model * vec4(fPosition, 1.0f);

    resetValue(ambient);
    resetValue(diffuse);
    resetValue(specular);
    vec3 additionalAmbient = vec3(0.0f);
    vec3 additionalDiffuse = vec3(0.0f);
    vec3 additionalSpecular = vec3(0.0f);


    // Compute directional light
    computeDirLight();

    // Compute shadow factor for directional light
    float shadow = computeShadow();
    shadowFactor = 1.0 - shadow;

    // Compute point light
    vec3 pAmbient, pDiffuse, pSpecular;
    //resetValue(pAmbient);
    //resetValue(pDiffuse);
    //resetValue(pSpecular);
    computePointLight(pAmbient, pDiffuse, pSpecular);

 if (pointLightsEnabled) {
        computeFivePointLights(additionalAmbient, additionalDiffuse, additionalSpecular);
        //fColor = vec4(1.0, 0.0, 0.0, 1.0);//debug
        //return;
    }

    // Combine all lighting components
    vec3 ambientColor = ambient + pAmbient + additionalAmbient;
    vec3 diffuseColor = diffuse + pDiffuse + additionalDiffuse;
    vec3 specularColor = specular + pSpecular + additionalSpecular;

    // Apply shadows to the diffuse and specular components
    vec3 shadowedDiffuse = shadowFactor * diffuseColor;
    vec3 shadowedSpecular = shadowFactor * specularColor;

    // Final lighting result
    vec3 finalLightingColor = (ambient + shadowedDiffuse + shadowedSpecular) * texture(diffuseTexture, fTexCoords).rgb;

    
	float fogDensity = 0.02;
	
    // Compute fog separately from lighting
    if (fogOn == 1) {
        float fragmentDistance = length(fragPosEye.xyz);
	    float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));
        fogFactor = clamp(fogFactor, 0.0, 1.0);
        //vec3 foggedColor = mix(fogColor, texture(diffuseTexture, fTexCoords).rgb, fogFactor); // Fog applies only to scene color
        //fColor = vec4(foggedColor + finalLightingColor, 1.0f); // Combine lighting and fog without blending them
        fColor = mix(fogColor, vec4(finalLightingColor, 1.0f), fogFactor);
    } else {
        fColor = vec4(finalLightingColor, 1.0f);
    }

    if(isLake) {
        fColor.w = 0.9;
    }
    else {
		fColor.w = 1.0;
	}
}
