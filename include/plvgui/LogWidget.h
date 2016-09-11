#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QDockWidget>
#include "qxtloggerengine.h"

QT_FORWARD_DECLARE_CLASS(QTextEdit)

namespace plvgui
{

    class LogWidget : public QDockWidget , public QxtLoggerEngine
    {
        Q_OBJECT
    public:
        explicit LogWidget(const QString& title, QWidget *parent = 0, int maxLogLength = 1000);
        virtual ~LogWidget();

        virtual void initLoggerEngine();
        virtual void killLoggerEngine();
        virtual bool isInitialized() const;
        virtual void writeFormatted(QxtLogger::LogLevel level, const QList<QVariant>& messages);

    signals:

    public slots:

    private:
        QTextEdit* m_textEdit;
        QString m_dateFormat;
    };

}

#endif // LOGWIDGET_H
