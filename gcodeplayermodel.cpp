#include <QQmlEngine>

#include "gcodeplayermodel.h"

GcodePlayerModel::GcodePlayerModel(QObject *parent)
{

}

void GcodePlayerModel::registerQmlTypes()
{
    qmlRegisterType<GcodePlayerModel>("tech.vhrd", 1, 0, "GcodePlayerModel");
    qmlRegisterUncreatableType<GcodePlayerItem>("tech.vhrd", 1, 0, "GcodePlayerItem", "enums only");
    //qRegisterMetaType<GcodePlayerItem>("GcodePlayerItem");
}

int GcodePlayerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_items.count();
}

QVariant GcodePlayerModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_items.count())
        return QVariant();

    const GcodePlayerItem& item = m_items[index.row()];
    if (role == StatusRole)
        return item.m_status;
    else if (role == LineNumberRole)
        return item.m_lineNumber;
    else if (role == CodeRole)
        return item.m_code;
    else if (role == ResponseRole)
        return item.m_response;
    return QVariant();
}

QHash<int, QByteArray> GcodePlayerModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[StatusRole] = "status";
    roles[LineNumberRole] = "lineNumber";
    roles[CodeRole] = "code";
    roles[ResponseRole] = "response";
    return roles;
}

void GcodePlayerModel::addItem(const GcodePlayerItem &item)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_items << item;
    endInsertRows();
}

GcodePlayerItem GcodePlayerModel::getItem(int index)
{
    if (index < 0 || index >= m_items.length())
        return GcodePlayerItem();
    return m_items[index];
}

void GcodePlayerModel::replaceItem(int index, const GcodePlayerItem &item)
{
    if (index < 0 || index > m_items.count())
        return;
    QModelIndex modelIndex = createIndex(index, 0);
    m_items[index] = item;
    emit dataChanged(modelIndex, modelIndex);
}

void GcodePlayerModel::changeAllStates(GcodePlayerItem::Status to)
{
    for (int i = 0; i < m_items.length(); ++i)
        m_items[i].m_status = to;
    QModelIndex startIndex = createIndex(0, 0);
    QModelIndex endIndex = createIndex(m_items.length() - 1, 0);
    emit dataChanged(startIndex, endIndex);
}

void GcodePlayerModel::removeAll()
{
    beginRemoveRows(QModelIndex(), 0, m_items.count());
    m_items.clear();
    endRemoveRows();
}

