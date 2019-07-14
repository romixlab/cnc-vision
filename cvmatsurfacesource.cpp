#include "cvmatsurfacesource.hpp"

#include <QAbstractVideoSurface>
#include <QThread>
#include <QDebug>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

Q_DECLARE_METATYPE(cv::Mat)

struct CVMatSurfaceSourcePrivate {
    CVMatSurfaceSourcePrivate() {}
    ~CVMatSurfaceSourcePrivate() {}
    QHash<QString, CVMatSurfaceSource *> sources;
};
Q_GLOBAL_STATIC(CVMatSurfaceSourcePrivate, g_sources)

CVMatSurfaceSource::CVMatSurfaceSource(QObject *parent) : QObject(parent),
    m_surface(nullptr), m_videoFrame(nullptr)
{
}

CVMatSurfaceSource::~CVMatSurfaceSource()
{
    stopSurface();
    CVMatSurfaceSourcePrivate *d = g_sources;
    if (d) {
        d->sources.remove(m_name);
    }
}

QAbstractVideoSurface *CVMatSurfaceSource::videoSurface() const
{
    return m_surface;
}

void CVMatSurfaceSource::setVideoSurface(QAbstractVideoSurface *surface)
{
    if (m_surface && m_surface->isActive())
        m_surface->stop();
    m_surface = surface;
}

void CVMatSurfaceSource::imshow(const cv::Mat &mat)
{
    if (!m_surface)
        return;
    if (mat.empty())
        return;
    if (QThread::currentThread() != this->thread()) {
        QVariant matVariant = QVariant::fromValue(mat.clone());
        QMetaObject::invokeMethod(this, "imshowFromVariant", Qt::QueuedConnection, Q_ARG(QVariant, matVariant));
        return;
    }
    if (!m_videoFrame) {
        createVideoFrame(mat);
    }
    if (matHasChanged(mat)) {
        if (m_videoFrame->isMapped())
            m_videoFrame->unmap();
        delete m_videoFrame;
        createVideoFrame(mat);
    }
    cv::Mat matFromSurface = cv::Mat(m_videoFrame->size().height(),
                                     m_videoFrame->size().width(),
                                     CV_8UC4, m_videoFrame->bits());
    if (mat.channels() == 3) {
        cv::cvtColor(mat, matFromSurface, CV_RGB2RGBA);
    } else if (mat.channels() == 1) {
        cv::cvtColor(mat, matFromSurface, CV_GRAY2RGBA);
    } else {
        qDebug() << "Wrong channel count";
        return;
    }
    if (!m_surface->present(*m_videoFrame)) {
        qDebug() << "Surface present() failed" << m_surface->error();
    }
}

void CVMatSurfaceSource::imshowFromVariant(const QVariant &matVariant)
{
    cv::Mat mat = matVariant.value<cv::Mat>();
    imshow(mat);
}

void CVMatSurfaceSource::imshow(const QString &surfaceName, const cv::Mat &mat)
{
    CVMatSurfaceSourcePrivate *d = g_sources;
    if (d) {
        CVMatSurfaceSource *source = d->sources.value(surfaceName, 0);
        if (source)
            source->imshow(mat);
        else
            qWarning() << "CVMatSurfaceSource with name" << surfaceName << "doesn't exist or not yet created";
    }
}

void CVMatSurfaceSource::stopSurface()
{
    if (m_surface && m_surface->isActive())
        m_surface->stop();
}

void CVMatSurfaceSource::createVideoFrame(const cv::Mat &mat)
{
    m_videoFrame = new QVideoFrame(mat.rows * mat.cols * 4,
                                   QSize(mat.cols, mat.rows),
                                   mat.cols * 4,
                                   QVideoFrame::Format_ARGB32);
    m_format = QVideoSurfaceFormat(m_videoFrame->size(), m_videoFrame->pixelFormat());
    m_surface->start(m_format);
    if (!m_videoFrame->map(QAbstractVideoBuffer::ReadOnly)) {
        qWarning() << "QVideoFrame::map() failed";
    }
}

bool CVMatSurfaceSource::matHasChanged(const cv::Mat &mat) const
{
    return m_videoFrame->size().width() != mat.cols ||
           m_videoFrame->size().height() != mat.rows;
}

QString CVMatSurfaceSource::name() const
{
    return m_name;
}

void CVMatSurfaceSource::setName(const QString &name)
{
    CVMatSurfaceSourcePrivate *d = g_sources;
    if (d) {
        d->sources.remove(m_name);
        d->sources.insert(name, this);
    }
    m_name = name;
}
