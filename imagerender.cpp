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
    delete m_program;
    delete m_fbo;
    delete m_context;
}

void ImageRender::createSurface()
{
    m_surface.create();
}

void ImageRender::initialize(const QVector2D &min, const QVector2D &max, const QList<StaticPlanet> &staticPlanets)
{
    if (!m_fbo) {
        m_context = new QOpenGLContext();
        m_context->setFormat(m_surface.requestedFormat());
        m_context->create();
        m_context->makeCurrent(&m_surface);

        QSize size(800, 600);
        m_fbo = new QOpenGLFramebufferObject(size);

        m_program = new QOpenGLShaderProgram();
        m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/GLSL/main.vert");
        m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/GLSL/main.frag");
        m_program->link();
        m_vertexAttr = m_program->attributeLocation("qt_Vertex");
        m_colorUni = m_program->uniformLocation("qt_Color");
        m_matrixUni = m_program->uniformLocation("qt_Matrix");
    }

    minBound = min;
    maxBound = max;
    m_fbo->bind();
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    if (staticPlanets.size() > 0) {
        glViewport(0, 0, m_fbo->width(), m_fbo->height());

        QMatrix4x4 matrix;
        matrix.ortho(minBound.x(), maxBound.x(), minBound.y(), maxBound.y(), 0.0f, 1.0f);

        m_program->bind();
        m_program->setUniformValue(m_matrixUni, matrix);

        QOpenGLBuffer m_buffer;
        m_buffer.create();
        m_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_buffer.bind();
        m_buffer.allocate(staticPlanets.size() * sizeof(PointDouble2D));
        for (int i = 0; i < staticPlanets.size(); i++)
            m_buffer.write(i * sizeof(PointDouble2D), &staticPlanets.at(i).position, sizeof(PointDouble2D));
        m_program->setAttributeBuffer(m_vertexAttr, GL_DOUBLE, 0, 2);
        m_program->enableAttributeArray(m_vertexAttr);
        for (int i = 0; i < staticPlanets.size(); i++) {
            m_program->setUniformValue(m_colorUni, staticPlanets.at(i).color);
            glDrawArrays(GL_POINTS, i, 1);
        }
        m_buffer.release();
        m_program->release();
    }

    m_fbo->bindDefault();
}

void ImageRender::render(const QList<DynamicPlanet> &dynamicPlanets)
{
    if (dynamicPlanets.size() > 0) {
        m_fbo->bind();
        glViewport(0, 0, m_fbo->width(), m_fbo->height());

        QMatrix4x4 matrix;
        matrix.ortho(minBound.x(), maxBound.x(), minBound.y(), maxBound.y(), 0.0f, 1.0f);

        m_program->bind();
        m_program->setUniformValue(m_matrixUni, matrix);

        QOpenGLBuffer m_buffer;
        m_buffer.create();
        m_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_buffer.bind();

        int buffSize = 0;
        for (int i = 0; i < dynamicPlanets.size(); i++)
            buffSize += dynamicPlanets.at(i).samples.size() * sizeof(QVector2D);
        m_buffer.allocate(buffSize);

        int buffOffset = 0;
        for (int i = 0; i< dynamicPlanets.size(); i++) {
            for (int j = 0; j < dynamicPlanets.at(i).samples.size(); j++)
                m_buffer.write(buffOffset + j * sizeof(QVector2D), &dynamicPlanets.at(i).samples.at(j).position, sizeof(QVector2D));
            buffOffset += dynamicPlanets.at(i).samples.size() * sizeof(QVector2D);
        }

        m_program->setAttributeBuffer(m_vertexAttr, GL_FLOAT, 0, 2);
        m_program->enableAttributeArray(m_vertexAttr);

        buffOffset = 0;
        for (int i = 0; i < dynamicPlanets.size(); i++) {
            m_program->setUniformValue(m_colorUni, dynamicPlanets.at(i).color);
            glDrawArrays(GL_LINE_STRIP, buffOffset, dynamicPlanets.at(i).samples.size());
            buffOffset += dynamicPlanets.at(i).samples.size();
        }

        m_buffer.release();
        m_program->release();
        m_fbo->bindDefault();
    }
}

QImage ImageRender::getImage() const
{
    return m_fbo->toImage();
}
