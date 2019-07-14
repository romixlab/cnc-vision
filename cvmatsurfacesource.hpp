#ifndef CVMATSURFACESOURCE_H
#define CVMATSURFACESOURCE_H

#include <QObject>
#include <QVideoSurfaceFormat>

#include <opencv2/core.hpp>

class QAbstractVideoSurface;


class CVMatSurfaceSource : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractVideoSurface* videoSurface READ videoSurface WRITE setVideoSurface)
    Q_PROPERTY(QString name READ name WRITE setName)
public:
    explicit CVMatSurfaceSource(QObject *parent = nullptr);
    ~CVMatSurfaceSource();

    QAbstractVideoSurface* videoSurface() const;
    void setVideoSurface(QAbstractVideoSurface *surface);

    /**
     * @brief imshow Sends mat to video surface setted by @ref setVideoSurface
     * @param mat
     */
    void imshow(const cv::Mat &mat);
    /**
     * @brief imshow Finds surface source by name in global hashmap and calls imshow on it
     * @param surfaceName
     * @param mat
     */
    static void imshow(const QString &surfaceName, const cv::Mat &mat);

    /**
     * @brief setName Sets name and mades this object accessible through static imshow()
     * @param name
     */
    void setName(const QString &name);
    QString name() const;

private slots:
    void imshowFromVariant(const QVariant &matVariant);

private:
    void stopSurface();
    void createVideoFrame(const cv::Mat &mat);
    bool matHasChanged(const cv::Mat &mat) const;
    QAbstractVideoSurface* m_surface;
    QVideoFrame* m_videoFrame;
    QVideoSurfaceFormat m_format;
    QString m_name;
};

#endif // CVMATSURFACESOURCE_H
