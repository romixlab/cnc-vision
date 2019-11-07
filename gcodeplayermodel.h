#ifndef GCODEPLAYERMODEL_H
#define GCODEPLAYERMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "gcodeplayeritem.h"

class GcodePlayerModel : public QAbstractListModel
{
    Q_OBJECT
public:
    GcodePlayerModel(QObject *parent = nullptr);

    static void registerQmlTypes();

    enum ItemRoles {
        StatusRole = Qt::UserRole + 1,
        LineNumberRole,
        CodeRole,
        ResponseRole
    };

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    void addItem(const GcodePlayerItem &item);
    GcodePlayerItem getItem(int index);
    void replaceItem(int index, const GcodePlayerItem &item);
    void changeAllStates(GcodePlayerItem::Status to);
    void removeAll();

protected:
    QHash<int, QByteArray> roleNames() const override;

signals:

private:

    QList<GcodePlayerItem> m_items;
};

#endif // GCODEPLAYERMODEL_H
