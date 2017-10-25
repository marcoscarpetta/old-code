#include "clock.h"

#include <QDateTime>


Clock::Clock()
{
    helper = NULL;
    rightMenu = NULL;
    info = new QSettings(":/data/clock.ini", QSettings::IniFormat);
    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));

    connect(&menu, SIGNAL(aboutToHide()), SIGNAL(deactivated()));

    clockLabel = new QLabel();

    QWidgetAction *widgetAction = new QWidgetAction(this);
    widgetAction->setDefaultWidget(&calendar);
    menu.addAction(widgetAction);

    deactivatedByPanel = false;
}

Clock::~Clock()
{
    delete info;
    if (helper)
        delete helper;
    delete clockLabel;
}

QSettings *Clock::data()
{
    return info;
}

QWidget* Clock::widget()
{
    return clockLabel;
}

QObject *Clock::object()
{
    return this;
}

AppletInterface *Clock::create(AppletHelper *helper, QMenu *rightMenu)
{
    Clock *clock = new Clock();
    clock->helper = helper;
    clock->rightMenu = rightMenu;
    clock->timer.start(10);
    return clock;
}

bool Clock::click(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (event->button() == Qt::LeftButton)
            this->menu.popup(helper->getPopupPoint(clockLabel->pos(), menu.sizeHint()));
    }
    else if (event->button() == Qt::RightButton)
        rightMenu->popup(helper->getPopupPoint(event->pos()+clockLabel->pos(), rightMenu->sizeHint()));
    return true;
}

bool Clock::dragClick()
{
    deactivatedByPanel = false;
    this->menu.popup(helper->getPopupPoint(clockLabel->pos(), menu.sizeHint()));
    return true;
}

void Clock::deactivate()
{
    deactivatedByPanel = true;
    menu.hide();
}

void Clock::onMenuAboutToHide()
{
    if (!deactivatedByPanel)
    {
        emit deactivated();
        deactivatedByPanel = false;
    }
}

/* aggiorna l'orario visualizzato in base a quello di sistema ogni 10ms */
void Clock::onTimeOut()
{
    clockLabel->setText(QDateTime::currentDateTime().toString());
}
