#ifndef LINEDETECTORDATASOURCE_H
#define LINEDETECTORDATASOURCE_H

#include <QObject>
#include <QtCharts/QXYSeries>

QT_CHARTS_USE_NAMESPACE

class LineDetectorDataSource : public QObject
{
    Q_OBJECT
public:
    explicit LineDetectorDataSource(QObject *parent = nullptr);

    Q_INVOKABLE void setIntegratedPlotSeries(QAbstractSeries *series);
    Q_INVOKABLE void setThresholdPlotSeries(QAbstractSeries *series);

signals:

public slots:
    void updateIntegratedPlot(const QVector<float> &data);
    void drawBeamThickness(const QPointF &pt1, const QPointF &pt2);

private slots:
    void integratedSeriesDestroyed();

private:
    QXYSeries *m_integratedSeries;
    QXYSeries *m_thresholdSeries;
};

#endif // LINEDETECTORDATASOURCE_H
