#include "appmenu.h"

#include <QSizePolicy>
#include <QDir>
#include <QDebug>
#include <QProcess>

bool readDesktopFile(QIODevice &device, QSettings::SettingsMap &map)
{
    bool desktopEntry = false;
    QByteArray line = device.readLine(1024);
    while (!line.isEmpty()) {
        line.chop(1);
        if (line.contains('=') && desktopEntry) {
            if (line.endsWith(';')) line.chop(1);
            QList<QByteArray> keyValue = line.split('=');
            QVariant value;
            if (keyValue[1].contains(';'))
                value = QString(keyValue[1]).split(';');
            else value = QString(keyValue[1]);
            map.insert(QString(keyValue[0]), value);
        }
        else if (line == QByteArray("[Desktop Entry]"))
            desktopEntry = true;
        else desktopEntry = false;
        line = device.readLine(1024);
    }
    return true;
}

bool writeDesktopFile(QIODevice &device, const QSettings::SettingsMap &map) {return true;}

AppMenu::AppMenu()
{
    helper = NULL;
    rMenu = NULL;
    info = new QSettings(":/data/appmenu.ini", QSettings::IniFormat);
    menuButton = new QPushButton(tr("Applications"));
    menuButton->setCheckable(true);
    menuButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    menu = new QMenu();
    connect(menu, SIGNAL(aboutToHide()), this, SLOT(onMenuAboutToHide()));
    signalMapper = new QSignalMapper(this);
    deactivatedByPanel = false;
}

AppMenu::~AppMenu()
{
    delete info;
    if (helper)
        delete helper;
    delete menu;
    delete menuButton;
}

QSettings *AppMenu::data()
{
    return info;
}

QWidget* AppMenu::widget()
{
    return menuButton;
}

QObject *AppMenu::object()
{
    return this;
}

AppletInterface *AppMenu::create(AppletHelper *helper, QMenu *rightMenu)
{
    AppMenu *appmenu = new AppMenu();
    appmenu->helper = helper;
    appmenu->rMenu = rightMenu;
    appmenu->loadMenu();
    return appmenu;
}

bool AppMenu::click(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        this->menu->popup(helper->getPopupPoint(menuButton->pos(), menu->sizeHint()));
        menuButton->setChecked(true);
    }
    else if (event->button() == Qt::RightButton)
        rMenu->popup(helper->getPopupPoint(event->pos()+menuButton->pos(), rMenu->sizeHint()));
    return true;
}

bool AppMenu::dragClick()
{
    deactivatedByPanel = false;
    menuButton->setChecked(true);
    this->menu->popup(helper->getPopupPoint(menuButton->pos(), menu->sizeHint()));
    return true;
}

void AppMenu::deactivate()
{
    deactivatedByPanel = true;
    menuButton->setChecked(false);
    this->menu->hide();
}

/* emette il segnale 'deactivated' qunado il menu delle applicazioni si sta per chiudere */
void AppMenu::onMenuAboutToHide()
{
    if (!deactivatedByPanel)
    {
        emit deactivated();
        deactivatedByPanel = false;
    }
    menuButton->setChecked(false);
}

/* inserisce le voci delle applicazioni nel menu basandosi sui file nella cartella
 * /usr/share/applications */
void AppMenu::loadMenu()
{
    const QSettings::Format DesktopFormat =
                QSettings::registerFormat("desktop", readDesktopFile, writeDesktopFile);
    QDir appDir("/usr/share/applications");
    // categorie standard
    QStringList categories;
    categories << "AudioVideo" << "Audio" << "Video" << "Development" << "Education" <<
                  "Game" << "Graphics" << "Network" << "Office" << "Science" << "Settings" <<
                  "System" << "Utility";
    foreach (QString fileName, appDir.entryList(QDir::Files)) {
        QSettings desktopFile(appDir.absoluteFilePath(fileName), DesktopFormat);
        if (!desktopFile.contains("OnlyShowIn") && !desktopFile.contains("NoDisplay"))
        {
            foreach (QString category, desktopFile.value("Categories").toStringList()) {
                if (categories.contains(category)) {
                    bool exist = false;
                    QAction *action;
                    foreach (QAction *existingAction, menu->actions())
                        if (existingAction->data().toString() == category) {
                            action = existingAction;
                            exist = true;
                        }
                    // crea il sottomenu se non esiste
                    if (!exist) {
                        action = menu->addMenu(new QMenu(menu));
                        action->setText(category);
                        action->setData(category);
                    }
                    QAction *launcher = action->menu()->addAction(desktopFile.value("Name").toString());
                    QString iconName = desktopFile.value("Icon").toString();
                    if (iconName.startsWith('/'))
                        launcher->setIcon(QIcon(iconName));
                    else if (iconName.contains('.'))
                        launcher->setIcon(QIcon(iconName.split('.')[0]));
                    else launcher->setIcon(QIcon::fromTheme(iconName));
                    connect(launcher, SIGNAL(triggered()), signalMapper, SLOT(map()));
                    signalMapper->setMapping(launcher, desktopFile.value("Exec").toString());
                }
            }
        }
    }
    connect(signalMapper, SIGNAL(mapped(QString)),
                this, SLOT(onMenuActionTriggered(QString)));
}

/* esegue il comando relativo alla voce del menu selezionata */
void AppMenu::onMenuActionTriggered(QString command)
{
    QStringList cmdList = command.split(' ');
    QProcess::startDetached(cmdList[0]);
}
