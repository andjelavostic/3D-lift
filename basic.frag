#version 330 core
out vec4 FragColor;

in vec3 chNormal;  
in vec3 chFragPos;  
in vec2 chUV;

// Kamera
uniform vec3 uViewPos;

// Glavno (ambient / globalno) svetlo
uniform vec3 uLightPos; 
uniform vec3 uLightColor;

// Svetlo lampe na spratu
uniform vec3 uLampPos;    
uniform vec3 uLampColor;  

// Svetlo lampe u liftu
uniform vec3 uLiftLampPos;
uniform vec3 uLiftLampColor;

// Tekstura
uniform sampler2D uDiffMap1;

// ---------------- ATTENUATION ----------------
float attenuation(float dist)
{
    float constant  = 1.0;
    float linear    = 0.14;
    float quadratic = 0.07;
    return 1.0 / (constant + linear * dist + quadratic * dist * dist);
}
// Dodatna svetla sa dugmadi (maks 16)
uniform int uNumPanelLights;
uniform vec3 uPanelLightPos[12];
uniform vec3 uPanelLightColor[12];

void main()
{
    // NORMAL
    vec3 norm = normalize(chNormal);
    vec3 viewDir = normalize(uViewPos - chFragPos);

    // ---------------- AMBIENT ----------------
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * uLightColor;

    // ---------------- GLOBAL LIGHT ----------------
    vec3 lightDir = normalize(uLightPos - chFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = 0.4 * spec * uLightColor;

    // ---------------- FLOOR LAMP ----------------
    vec3 lampDir = normalize(uLampPos - chFragPos);
    float lampDist = length(uLampPos - chFragPos);
    float lampAtt = attenuation(lampDist);

    float lampDiff = max(dot(norm, lampDir), 0.0);
    vec3 lampDiffuse = lampDiff * uLampColor * lampAtt;

    vec3 lampReflect = reflect(-lampDir, norm);
    float lampSpec = pow(max(dot(viewDir, lampReflect), 0.0), 32);
    vec3 lampSpecular = 0.7 * lampSpec * uLampColor * lampAtt;

    // ---------------- LIFT LAMP ----------------
    vec3 liftDir = normalize(chFragPos - uLiftLampPos);
    float liftDist = length(uLiftLampPos - chFragPos);
    float liftAtt = attenuation(liftDist);

    float liftDiff = max(dot(norm, liftDir), 0.0);
    vec3 liftDiffuse = liftDiff * uLiftLampColor * liftAtt;

    vec3 liftReflect = reflect(-liftDir, norm);
    float liftSpec = pow(max(dot(viewDir, liftReflect), 0.0), 32);
    vec3 liftSpecular = 0.7 * liftSpec * uLiftLampColor * liftAtt;

    // ---------------- TEXTURE ----------------
    vec3 texColor = texture(uDiffMap1, chUV).rgb;

    // ---------------- PANEL BUTTON LIGHTS ----------------
    
        vec3 panelGlow = vec3(0.0);

        for (int i = 0; i < uNumPanelLights; i++) {
            float dist = length(chFragPos - uPanelLightPos[i]);

            // radius glow-a oko dugmeta
            float glow = smoothstep(0.3, 0.04, dist);

            panelGlow += glow * uPanelLightColor[i];
        }


    // ---------------- FINAL COLOR ----------------
    vec3 finalColor =
    texColor *
    ( ambient +
      diffuse + specular +
      lampDiffuse + lampSpecular +
      liftDiffuse + liftSpecular +
      panelGlow*0.6 );


    FragColor = vec4(finalColor, 1.0);
}
