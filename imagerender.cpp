#include "imagerender.h"

ImageRender::ImageRender(QObject *parent) :
    QObject(parent),
    m_context(nullptr),
    m_fbo(nullptr),
    m_program(nullptr)
{
}

ImageRender::~ImageRender()
{
    m_surface.destroy();
    delete m_fbo;
}

QImage ImageRender::getImage(const QVector2D &minBounder, const QVector2D &maxBounder,
                             const QList<StaticPlanet> &staticPlanets, const QList<DynamicPlanet> &dynamicPlanets)
{
    if (!m_fbo)
        initialize();

    m_fbo->bind();

    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, m_fbo->width(), m_fbo->height());

    QMatrix4x4 matrix;
    matrix.ortho(minBounder.x(), maxBounder.x(), minBounder.y(), maxBounder.y(), 0.0f, 1.0f);

    m_program->bind();
    m_program->setUniformValue(m_matrixUni, matrix);

    QOpenGLBuffer m_buffer;
    m_buffer.create();
    m_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_buffer.bind();

    int buffSize = staticPlanets.size() * sizeof(QVector2D);
    for (int i = 0; i < dynamicPlanets.size(); i++) {
        buffSize += dynamicPlanets.at(i).positions.size() * sizeof(QVector2D);
    }
    m_buffer.allocate(buffSize);

    for (int i = 0; i < staticPlanets.size(); i++) {
        m_buffer.write(i * sizeof(QVector2D), &staticPlanets.at(i).startPosition, sizeof(QVector2D));
    }
    int buffOffset = staticPlanets.size() * sizeof(QVector2D);
    for (int i = 0; i< dynamicPlanets.size(); i++) {
        m_buffer.write(buffOffset, dynamicPlanets.at(i).positions.constData(), dynamicPlanets.at(i).positions.size() * sizeof(QVector2D));
        buffOffset += dynamicPlanets.at(i).positions.size() * sizeof(QVector2D);
    }

    m_program->setAttributeBuffer(m_vertexAttr, GL_FLOAT, 0, 2);
    m_program->enableAttributeArray(m_vertexAttr);
    for (int i = 0; i < staticPlanets.size(); i++) {
        m_program->setUniformValue(m_colorUni, staticPlanets.at(i).color);
        glDrawArrays(GL_POINTS, i, 1);
    }

    buffOffset = staticPlanets.size();
    for (int i = 0; i < dynamicPlanets.size(); i++) {
        m_program->setUniformValue(m_colorUni, dynamicPlanets.at(i).color);
        glDrawArrays(GL_LINE_STRIP, buffOffset, dynamicPlanets.at(i).positions.size());
        buffOffset += dynamicPlanets.at(i).positions.size();
    }

    m_buffer.release();
    m_program->release();

    m_fbo->bindDefault();

    return m_fbo->toImage();
}

void ImageRender::initialize()
{
    m_context = new QOpenGLContext(this);
    m_surface.create();
    m_context->setFormat(m_surface.requestedFormat());
    m_context->create();
    m_context->makeCurrent(&m_surface);

    QSize size(800, 600);
    m_fbo = new QOpenGLFramebufferObject(size);

    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/GLSL/main.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/GLSL/main.frag");
    m_program->link();
    m_vertexAttr = m_program->attributeLocation("qt_Vertex");
    m_colorUni = m_program->uniformLocation("qt_Color");
    m_matrixUni = m_program->uniformLocation("qt_Matrix");
}
