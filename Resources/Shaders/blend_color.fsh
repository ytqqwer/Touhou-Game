varying vec2 v_texCoord;
varying vec4 v_fragmentColor;

uniform float u_alpha;
uniform vec4 u_blendColor;

void main() {
    vec4 realColor = texture2D(CC_Texture0,v_texCoord);
    if(realColor.a>0.){
        gl_FragColor = u_blendColor*u_alpha + realColor*(1.-u_alpha);
    }
    else{
        discard;
    }
}