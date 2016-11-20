#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <QLineEdit>
#include "tab_raw.h"

TabRaw::TabRaw(QWidget* parent) :
    QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    edit = new QTextEdit("",this);
    edit->setReadOnly(true);

    layout->addWidget(edit);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(new QLabel("Send:", this));
    sendText = new QLineEdit("", this);
    hlayout->addWidget(sendText);

    btClear = new QPushButton("clear", this);
    hlayout->addWidget(btClear);
    layout->addLayout(hlayout);
    setLayout(layout);
}

void TabRaw::displayText(const QString& text)
{
    edit->moveCursor(QTextCursor::End);
    edit->insertPlainText(text);

    if(!edit->underMouse())
        edit->ensureCursorVisible();
}

void TabRaw::displayTextHtml(const QString& text)
{
    edit->moveCursor(QTextCursor::End);
    edit->insertHtml(text);

    if(!edit->underMouse())
        edit->ensureCursorVisible();
}
