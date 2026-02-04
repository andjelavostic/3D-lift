#version 330 core
out vec4 FragColor;

in vec3 chNormal;  
in vec3 chFragPos;  
in vec2 chUV;

uniform vec3 uLightPos; 
uniform vec3 uViewPos; 
uniform vec3 uLightColor;

uniform sampler2D uDiffMap1; // Za obične modele
uniform vec4 uvRect;         // Za PanelGrid (x=minX, y=minY, z=maxX, w=maxY)
uniform bool isPanel;        // Dodajemo prekidač da znamo šta crtamo

void main()
{
    // --- 1. KOORDINATE TEKSTURE ---
    vec2 finalUV = chUV;
    if(isPanel) {
        // Skaliramo UV koordinate tako da zahvate samo jedan taster iz atlasa
        finalUV = vec2(
            uvRect.x + chUV.x * (uvRect.z - uvRect.x),
            uvRect.y + chUV.y * (uvRect.w - uvRect.y)
        );
    }

    // --- 2. OSVETLJENJE ---
    float ambientStrength = 0.2; // Malo jače ambijentalno da se vidi panel u liftu
    vec3 ambient = ambientStrength * uLightColor;

    vec3 norm = normalize(chNormal);
    vec3 lightDir = normalize(uLightPos - chFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(uViewPos - chFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * uLightColor;

    // --- 3. FINALNA BOJA ---
    vec3 texColor = texture(uDiffMap1, finalUV).rgb;
    
    // Ako je panel, možemo malo pojačati boju da "svetli"
    vec3 lighting = ambient + diffuse + specular;
    if(isPanel) lighting += 0.2; 

    FragColor = vec4(texColor * lighting, 1.0);
}