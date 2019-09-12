#include "linedetectordatasource.h"
#include <QDebug>

LineDetectorDataSource::LineDetectorDataSource(QObject *parent) : QObject(parent),
    m_integratedSeries(nullptr), m_thresholdSeries(nullptr)
{

}

void LineDetectorDataSource::setIntegratedPlotSeries(QAbstractSeries *series)
{
    QXYSeries *xySeries = static_cast<QXYSeries *>(series);
    if (xySeries) {
        m_integratedSeries = xySeries;
        connect(xySeries, &QObject::destroyed,
                this,     &LineDetectorDataSource::integratedSeriesDestroyed);
    }
}

void LineDetectorDataSource::setThresholdPlotSeries(QAbstractSeries *series)
{
    QXYSeries *xySeries = static_cast<QXYSeries *>(series);
    if (xySeries) {
        m_thresholdSeries = xySeries;
    }
}

void LineDetectorDataSource::updateIntegratedPlot(const QVector<float> &data)
{
    if (!m_integratedSeries)
        return;

    float normalizedStep = 1.0f / data.size();
    float x = 0.0;
    QVector<QPointF> points;
    for (int i = 0; i < data.size(); i++) {
        points.push_back(QPointF(x, data[i]));
        x += normalizedStep;
    }
    m_integratedSeries->replace(points);
}

void LineDetectorDataSource::drawBeamThickness(const QPointF &pt1, const QPointF &pt2)
{
    if (!m_thresholdSeries)
        return;
    QVector<QPointF> points;
    points << pt1 << pt2;
    m_thresholdSeries->replace(points);
}

void LineDetectorDataSource::integratedSeriesDestroyed()
{
    m_integratedSeries = nullptr;
    m_thresholdSeries = nullptr;
}
