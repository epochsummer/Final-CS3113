uniform sampler2D diffuse;
varying vec2 texCoordVar;

uniform bool player_dead;

void main() {
    vec4 colour = texture2D(diffuse, texCoordVar);

    if(player_dead)
    {
        float gray = dot(colour.rgb, vec3(0.299, 0.587, 0.114));
        colour = vec4(vec3(gray), colour.a); // Set RGB to the grayscale value, keep alpha unchanged
    }
    gl_FragColor = colour;
}
