attribute vec4 qt_Vertex;
uniform mat4 qt_Matrix;

void main(void)
{
    gl_Position = qt_Matrix * qt_Vertex;
}
