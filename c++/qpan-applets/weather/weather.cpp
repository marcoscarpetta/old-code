#include "weather.h"

#include <QDateTime>
#include <QXmlStreamReader>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QIcon>
#include <QLineEdit>
#include <QInputDialog>
#include <QLayout>

Weather::Weather()
{
    helper = NULL;
    rightMenu = NULL;
    info = new QSettings(":/data/weather.ini", QSettings::IniFormat);
    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetworkReply(QNetworkReply*)));

    connect(&menu, SIGNAL(aboutToHide()), SIGNAL(deactivated()));

    weatherLabel = new QLabel();
    weatherInfo.setTextFormat(Qt::RichText);
    weatherInfo.setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

    QWidgetAction *widgetAction = new QWidgetAction(this);
    widgetAction->setDefaultWidget(&weatherInfo);
    menu.addAction(widgetAction);
    menu.setContentsMargins(10,10,10,10);

    deactivatedByPanel = false;
}

Weather::~Weather()
{
    delete info;
    if (helper)
        delete helper;
    delete weatherLabel;
}

QSettings *Weather::data()
{
    return info;
}

QWidget* Weather::widget()
{
    return weatherLabel;
}

QObject *Weather::object()
{
    return this;
}

AppletInterface *Weather::create(AppletHelper *helper, QMenu *rightMenu)
{
    Weather *weather = new Weather();
    weather->helper = helper;
    QAction *action = new QAction("Settings", rightMenu);
    rightMenu->insertSeparator(rightMenu->actions().first());
    rightMenu->insertAction(rightMenu->actions().first(), action);
    connect(action, SIGNAL(triggered()), weather, SLOT(onSettingsTriggered()));
    weather->rightMenu = rightMenu;
    weather->onTimeOut();
    return weather;
}

bool Weather::click(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (event->button() == Qt::LeftButton)
            this->menu.popup(helper->getPopupPoint(weatherLabel->pos(), menu.sizeHint()));
    }
    else if (event->button() == Qt::RightButton)
        rightMenu->popup(helper->getPopupPoint(event->pos()+weatherLabel->pos(), rightMenu->sizeHint()));
    return true;
}

bool Weather::dragClick()
{
    deactivatedByPanel = false;
    this->menu.popup(helper->getPopupPoint(weatherLabel->pos(), menu.sizeHint()));
    return true;
}

void Weather::deactivate()
{
    deactivatedByPanel = true;
    menu.hide();
}

void Weather::onMenuAboutToHide()
{
    if (!deactivatedByPanel)
    {
        emit deactivated();
        deactivatedByPanel = false;
    }
}

/* invia la richiesta al server del meteo */
void Weather::onTimeOut()
{
    QUrl url("http://weather.yahooapis.com/forecastrss?w="+helper->value("woeid", "716196").toString()+"&u=c");
    networkManager->get(QNetworkRequest(url));
    timer.start(300000);
}

/* quando la risposta arriva dal server viene elaborata */
void Weather::onNetworkReply(QNetworkReply *reply)
{
    bool error = false;
    QString replyString;
    if(reply->error() == QNetworkReply::NoError)
    {
        int httpstatuscode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt();
        if (httpstatuscode == 200)
        {
            if (reply->isReadable())
            {
                updateWeather(QString::fromUtf8(reply->readAll().data()));
            } else error = true;
        } else error = true;
    reply->deleteLater();
    } else error = true;

    if (error)
    {
        weatherLabel->setText("--");
        weatherInfo.setText("Network error!");
    }
}

/* aggiorna le informazioni mostrate dall'applet in base al valore di 'xml' */
void Weather::updateWeather(QString xml)
{
    QXmlStreamReader xmlReader(xml);

    while(!xmlReader.atEnd() && !xmlReader.hasError()) {
        QXmlStreamReader::TokenType token = xmlReader.readNext();
        if(token == QXmlStreamReader::StartDocument) {
            continue;
        }

        if(token == QXmlStreamReader::StartElement) {
            if (xmlReader.name() == "condition")
                weatherLabel->setText(xmlReader.attributes().value("temp").toString()+"°C");
            if (xmlReader.name() == "description")
                weatherInfo.setText(xmlReader.readElementText());
        }
    }
    xmlReader.clear();
}

/* chiamata quando la voce 'Settings' del menu (tasto destro) è attivata */
void Weather::onSettingsTriggered()
{
    bool ok;
    QString text = QInputDialog::getText(NULL, tr("Weather applet settings"),
                                         tr("WOEID:"), QLineEdit::Normal,
                                         helper->value("woeid").toString(), &ok);

    if (ok && !text.isEmpty())
    {
        helper->setValue("woeid", text);
        timer.stop();
        onTimeOut();
    }
}
