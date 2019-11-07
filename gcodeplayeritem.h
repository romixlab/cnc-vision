#ifndef GCODEPLAYERITEM_H
#define GCODEPLAYERITEM_H

#include <QObject>

class GcodePlayerItem
{
    Q_GADGET
    Q_PROPERTY(Status status MEMBER m_status)
    Q_PROPERTY(int lineNumber MEMBER m_lineNumber)
    Q_PROPERTY(QString code MEMBER m_code)
    Q_PROPERTY(QString response MEMBER m_response)
public:
    enum Status {
        Pending,
        Ok,
        Warning,
        InternalCommand
    };
    Q_ENUM(Status)

    GcodePlayerItem(Status status, int lineNumber, QString code):
        m_status(status), m_lineNumber(lineNumber), m_code(code)
    {
    }

    GcodePlayerItem() :
        m_status(Pending), m_lineNumber(0)
    {
    }


    Status m_status;
    int m_lineNumber;
    QString m_code;
    QString m_response;
};
Q_DECLARE_METATYPE(GcodePlayerItem)

#endif // GCODEPLAYERITEM_H
