#ifndef TAB_RAW_H
#define TAB_RAW_H

#include <QWidget>

class QPushButton;
class QTextEdit;
class QLineEdit;

class TabRaw : public QWidget
{
    Q_OBJECT

public:
    TabRaw(QWidget *parent = 0);
    void displayText(const QString &text);
    void displayTextHtml(const QString& text);

    QLineEdit *sendText;

private:
    QTextEdit *edit;
    QPushButton *btClear;
};

#endif
