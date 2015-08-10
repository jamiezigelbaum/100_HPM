/*{
 "DESCRIPTION": "RGB Average"
,"CREDIT": "by dantheman"
,"CATEGORIES": ["stylized Effect"]
,"INPUTS": [{"NAME": "image6",
"TYPE": "image"},
{"NAME": "image7",
"TYPE": "image"},
{"NAME": "image8",
"TYPE": "image"},
{"NAME": "image9",
"TYPE": "image"},
{"NAME": "image10",
"TYPE": "image"},
{"NAME": "image11",
"TYPE": "image"}]}*/

void main(void) {
vec2 uv = vv_FragNormCoord.xy;
vec4	final = vec4(0.0);
vec4	srcPixel6 = IMG_NORM_PIXEL(image6,uv);
final+=srcPixel6;
vec4	srcPixel7 = IMG_NORM_PIXEL(image7,uv);
final+=srcPixel7;
vec4	srcPixel8 = IMG_NORM_PIXEL(image8,uv);
final+=srcPixel8;
vec4	srcPixel9 = IMG_NORM_PIXEL(image9,uv);
final+=srcPixel9;
vec4	srcPixel10 = IMG_NORM_PIXEL(image10,uv);
final+=srcPixel10;
vec4	srcPixel11 = IMG_NORM_PIXEL(image11,uv);
final+=srcPixel11;
gl_FragColor = vec4(vec3(final)/6.0, 1.0);

}
