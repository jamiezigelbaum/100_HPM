/*{
 "DESCRIPTION": "RGB Average"
,"CREDIT": "by dantheman"
,"CATEGORIES": ["stylized Effect"]
,"INPUTS": [{"NAME": "image9",
"TYPE": "image"},
{"NAME": "image10",
"TYPE": "image"},
{"NAME": "image11",
"TYPE": "image"},
{"NAME": "image12",
"TYPE": "image"},
{"NAME": "image13",
"TYPE": "image"},
{"NAME": "image14",
"TYPE": "image"},
{"NAME": "image15",
"TYPE": "image"},
{"NAME": "image16",
"TYPE": "image"},
{"NAME": "image17",
"TYPE": "image"}]}*/

void main(void) {
vec2 uv = vv_FragNormCoord.xy;
vec4	final = vec4(0.0);
vec4	srcPixel9 = IMG_NORM_PIXEL(image9,uv);
final+=srcPixel9;
vec4	srcPixel10 = IMG_NORM_PIXEL(image10,uv);
final+=srcPixel10;
vec4	srcPixel11 = IMG_NORM_PIXEL(image11,uv);
final+=srcPixel11;
vec4	srcPixel12 = IMG_NORM_PIXEL(image12,uv);
final+=srcPixel12;
vec4	srcPixel13 = IMG_NORM_PIXEL(image13,uv);
final+=srcPixel13;
vec4	srcPixel14 = IMG_NORM_PIXEL(image14,uv);
final+=srcPixel14;
vec4	srcPixel15 = IMG_NORM_PIXEL(image15,uv);
final+=srcPixel15;
vec4	srcPixel16 = IMG_NORM_PIXEL(image16,uv);
final+=srcPixel16;
vec4	srcPixel17 = IMG_NORM_PIXEL(image17,uv);
final+=srcPixel17;
gl_FragColor = vec4(vec3(final)/9.0, 1.0);

}
