#include "OpenRGBDialog2.h"
#include "OpenRGBDevicePage.h"
#include "OpenRGBDeviceInfoPage.h"
#include "OpenRGBSoftwareInfoPage.h"
#include "OpenRGBSystemInfoPage.h"
#include "OpenRGBProfileSaveDialog.h"
#include <QLabel>
#include <QTabBar>
#include <QMessageBox>

using namespace Ui;

OpenRGBDialog2::OpenRGBDialog2(std::vector<i2c_smbus_interface *>& bus, std::vector<RGBController *>& control, ProfileManager& manager, QWidget *parent) : QMainWindow(parent), busses(bus), controllers(control), profile_manager(manager), ui(new OpenRGBDialog2Ui)
{
    ui->setupUi(this);

    QIcon logo(":OpenRGB.png");
    setWindowIcon(logo);

    /*-----------------------------------------------------*\
    | Set up tray icon menu                                 |
    \*-----------------------------------------------------*/
    QMenu* trayIconMenu = new QMenu( this );

    trayIcon = new QSystemTrayIcon(this);

    QAction* actionShowHide = new QAction("Show/Hide", this);
    connect(actionShowHide, SIGNAL(triggered()), this, SLOT(on_ShowHide()));
    trayIconMenu->addAction(actionShowHide);

    profileMenu = new QMenu("Profiles", this);

    trayIconMenu->addMenu(profileMenu);

    QMenu* quickColorsMenu = new QMenu("Quick Colors", this);

    QAction* actionQuickRed = new QAction("Red", this);
    connect(actionQuickRed, SIGNAL(triggered()), this, SLOT(on_QuickRed()));
    quickColorsMenu->addAction(actionQuickRed);

    QAction* actionQuickYellow = new QAction("Yellow", this);
    connect(actionQuickYellow, SIGNAL(triggered()), this, SLOT(on_QuickYellow()));
    quickColorsMenu->addAction(actionQuickYellow);

    QAction* actionQuickGreen = new QAction("Green", this);
    connect(actionQuickGreen, SIGNAL(triggered()), this, SLOT(on_QuickGreen()));
    quickColorsMenu->addAction(actionQuickGreen);

    QAction* actionQuickCyan = new QAction("Cyan", this);
    connect(actionQuickCyan, SIGNAL(triggered()), this, SLOT(on_QuickCyan()));
    quickColorsMenu->addAction(actionQuickCyan);

    QAction* actionQuickBlue = new QAction("Blue", this);
    connect(actionQuickBlue, SIGNAL(triggered()), this, SLOT(on_QuickBlue()));
    quickColorsMenu->addAction(actionQuickBlue);
    
    QAction* actionQuickMagenta = new QAction("Magenta", this);
    connect(actionQuickMagenta, SIGNAL(triggered()), this, SLOT(on_QuickMagenta()));
    quickColorsMenu->addAction(actionQuickMagenta);

    QAction* actionQuickWhite = new QAction("White", this);
    connect(actionQuickWhite, SIGNAL(triggered()), this, SLOT(on_QuickWhite()));
    quickColorsMenu->addAction(actionQuickWhite);

    trayIconMenu->addMenu(quickColorsMenu);

    QAction* actionLightsOff = new QAction("Lights Off", this);
    connect(actionLightsOff, SIGNAL(triggered()), this, SLOT(on_LightsOff()));
    trayIconMenu->addAction(actionLightsOff);

    QAction* actionExit = new QAction( "Exit", this );
    connect( actionExit, SIGNAL( triggered() ), this, SLOT( on_Exit() ));
    trayIconMenu->addAction(actionExit);

    trayIcon->setIcon(logo);
    trayIcon->setToolTip("OpenRGB");
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();

    RefreshProfileList();

    /*-----------------------------------------------------*\
    | Set up list of devices                                |
    \*-----------------------------------------------------*/
    QTabBar *DevicesTabBar = ui->DevicesTabBar->tabBar();

    for(std::size_t dev_idx = 0; dev_idx < control.size(); dev_idx++)
    {
        OpenRGBDevicePage *NewPage = new OpenRGBDevicePage(control[dev_idx]);
        ui->DevicesTabBar->addTab(NewPage, "");

        /*-----------------------------------------------------*\
        | Connect the page's Set All button to the Set All slot |
        \*-----------------------------------------------------*/
        connect(NewPage,
                SIGNAL(SetAllDevices(unsigned char, unsigned char, unsigned char)),
                this,
                SLOT(on_SetAllDevices(unsigned char, unsigned char, unsigned char)));

        /*-----------------------------------------------------*\
        | Connect the page's Resize signal to the Save Size slot|
        \*-----------------------------------------------------*/
        connect(NewPage,
                SIGNAL(SaveSizeProfile()),
                this,
                SLOT(on_SaveSizeProfile()));

        /*-----------------------------------------------------*\
        | Use Qt's HTML capabilities to display both icon and   |
        | text in the tab label.  Choose icon based on device   |
        | type and append device name string.                   |
        \*-----------------------------------------------------*/
        QString NewLabelString = "<html><table><tr><td width='30'><img src='";
        switch(control[dev_idx]->type)
        {
        case DEVICE_TYPE_MOTHERBOARD:
            NewLabelString += ":/motherboard.svg";
            break;
        case DEVICE_TYPE_DRAM:
            NewLabelString += ":/ram.svg";
            break;
        case DEVICE_TYPE_GPU:
            NewLabelString += ":/graphics-card.svg";
            break;
        case DEVICE_TYPE_COOLER:
            NewLabelString += ":/fan.svg";
            break;
        case DEVICE_TYPE_LEDSTRIP:
            //break;
        case DEVICE_TYPE_KEYBOARD:
            //break;
        case DEVICE_TYPE_MOUSE:
            //break;
        case DEVICE_TYPE_HEADSET:
            //break;
        case DEVICE_TYPE_UNKNOWN:
            NewLabelString += ":/keyboard.svg";
            break;
        }
        NewLabelString += "' height='15' width='15'></td><td>" + QString::fromStdString(control[dev_idx]->name) + "</td></tr></table></html>";

        QLabel *NewTabLabel = new QLabel();
        NewTabLabel->setText(NewLabelString);
        NewTabLabel->setIndent(20);
        NewTabLabel->setGeometry(0, 0, 200, 20);

        DevicesTabBar->setTabButton(dev_idx, QTabBar::LeftSide, NewTabLabel);
    }

    /*-----------------------------------------------------*\
    | Set up list of information                            |
    \*-----------------------------------------------------*/
    QTabBar *InformationTabBar = ui->InformationTabBar->tabBar();

    for(std::size_t dev_idx = 0; dev_idx < control.size(); dev_idx++)
    {
        OpenRGBDeviceInfoPage *NewPage = new OpenRGBDeviceInfoPage(control[dev_idx]);
        ui->InformationTabBar->addTab(NewPage, "");

        /*-----------------------------------------------------*\
        | Use Qt's HTML capabilities to display both icon and   |
        | text in the tab label.  Choose icon based on device   |
        | type and append device name string.                   |
        \*-----------------------------------------------------*/
        QString NewLabelString = "<html><table><tr><td width='30'><img src='";
        switch(control[dev_idx]->type)
        {
        case DEVICE_TYPE_MOTHERBOARD:
            NewLabelString += ":/motherboard.svg";
            break;
        case DEVICE_TYPE_DRAM:
            NewLabelString += ":/ram.svg";
            break;
        case DEVICE_TYPE_GPU:
            NewLabelString += ":/graphics-card.svg";
            break;
        case DEVICE_TYPE_COOLER:
            NewLabelString += ":/fan.svg";
            break;
        case DEVICE_TYPE_LEDSTRIP:
            //break;
        case DEVICE_TYPE_KEYBOARD:
            //break;
        case DEVICE_TYPE_MOUSE:
            //break;
        case DEVICE_TYPE_HEADSET:
            //break;
        case DEVICE_TYPE_UNKNOWN:
            NewLabelString += ":/keyboard.svg";
            break;
        }
        NewLabelString += "' height='15' width='15'></td><td>" + QString::fromStdString(control[dev_idx]->name) + "</td></tr></table></html>";

        QLabel *NewTabLabel = new QLabel();
        NewTabLabel->setText(NewLabelString);
        NewTabLabel->setIndent(20);
        NewTabLabel->setGeometry(0, 0, 200, 20);

        InformationTabBar->setTabButton(dev_idx, QTabBar::LeftSide, NewTabLabel);
    }

    OpenRGBSystemInfoPage *SysInfoPage = new OpenRGBSystemInfoPage(bus);
    ui->InformationTabBar->addTab(SysInfoPage, "");

    /*-----------------------------------------------------*\
    | Use Qt's HTML capabilities to display both icon and   |
    | text in the tab label.  Choose icon based on device   |
    | type and append device name string.                   |
    \*-----------------------------------------------------*/
    QString SystemLabelString = "<html><table><tr><td width='30'><img src='";
    SystemLabelString += ":/keyboard.svg";
    SystemLabelString += "' height='15' width='15'></td><td>System</td></tr></table></html>";

    QLabel *SystemTabLabel = new QLabel();
    SystemTabLabel->setText(SystemLabelString);
    SystemTabLabel->setIndent(20);
    SystemTabLabel->setGeometry(0, 0, 200, 20);

    InformationTabBar->setTabButton(control.size(), QTabBar::LeftSide, SystemTabLabel);

    OpenRGBSoftwareInfoPage *SoftInfoPage = new OpenRGBSoftwareInfoPage();
    ui->InformationTabBar->addTab(SoftInfoPage, "");

    /*-----------------------------------------------------*\
    | Use Qt's HTML capabilities to display both icon and   |
    | text in the tab label.  Choose icon based on device   |
    | type and append device name string.                   |
    \*-----------------------------------------------------*/
    QString SoftwareLabelString = "<html><table><tr><td width='30'><img src='";
    SoftwareLabelString += ":/keyboard.svg";
    SoftwareLabelString += "' height='15' width='15'></td><td>Software</td></tr></table></html>";

    QLabel *SoftwareTabLabel = new QLabel();
    SoftwareTabLabel->setText(SoftwareLabelString);
    SoftwareTabLabel->setIndent(20);
    SoftwareTabLabel->setGeometry(0, 0, 200, 20);

    InformationTabBar->setTabButton(control.size() + 1, QTabBar::LeftSide, SoftwareTabLabel);
}

OpenRGBDialog2::~OpenRGBDialog2()
{
    delete ui;
}

void OpenRGBDialog2::show()
{
    QMainWindow::show();
}

void OpenRGBDialog2::RefreshProfileList()
{
    /*-----------------------------------------------------*\
    | Clear profile combo box and tray icon menu            |
    \*-----------------------------------------------------*/
    ui->ProfileBox->clear();
    profileMenu->clear();

    for(std::size_t profile_index = 0; profile_index < profile_manager.profile_list.size(); profile_index++)
    {
        /*-----------------------------------------------------*\
        | Fill in profile combo box                             |
        \*-----------------------------------------------------*/
        ui->ProfileBox->addItem(profile_manager.profile_list[profile_index].c_str());

        /*-----------------------------------------------------*\
        | Fill in profile tray icon menu                        |
        \*-----------------------------------------------------*/
        QAction* actionProfileSelected = new QAction(profile_manager.profile_list[profile_index].c_str(), this);
        actionProfileSelected->setObjectName(profile_manager.profile_list[profile_index].c_str());
        connect(actionProfileSelected, SIGNAL(triggered()), this, SLOT(on_ProfileSelected()));
        profileMenu->addAction(actionProfileSelected);
    }
}

void OpenRGBDialog2::on_Exit()
{
    close();
}

void OpenRGBDialog2::on_LightsOff()
{
    on_SetAllDevices(0x00, 0x00, 0x00);
}

void OpenRGBDialog2::on_QuickRed()
{
    on_SetAllDevices(0xFF, 0x00, 0x00);
}

void OpenRGBDialog2::on_QuickYellow()
{
    on_SetAllDevices(0xFF, 0xFF, 0x00);
}

void OpenRGBDialog2::on_QuickGreen()
{
    on_SetAllDevices(0x00, 0xFF, 0x00);
}

void OpenRGBDialog2::on_QuickCyan()
{
    on_SetAllDevices(0x00, 0xFF, 0xFF);
}

void OpenRGBDialog2::on_QuickBlue()
{
    on_SetAllDevices(0x00, 0x00, 0xFF);
}

void OpenRGBDialog2::on_QuickMagenta()
{
    on_SetAllDevices(0xFF, 0x00, 0xFF);
}

void OpenRGBDialog2::on_QuickWhite()
{
    on_SetAllDevices(0xFF, 0xFF, 0xFF);
}

void OpenRGBDialog2::on_SetAllDevices(unsigned char red, unsigned char green, unsigned char blue)
{
    for(int device = 0; device < ui->DevicesTabBar->count(); device++)
    {
        qobject_cast<OpenRGBDevicePage *>(ui->DevicesTabBar->widget(device))->SetCustomMode();
        qobject_cast<OpenRGBDevicePage *>(ui->DevicesTabBar->widget(device))->SetDevice(red, green, blue);
    }
}

void OpenRGBDialog2::on_SaveSizeProfile()
{
    /*---------------------------------------------------------*\
    | Save the profile                                          |
    \*---------------------------------------------------------*/
    profile_manager.SaveProfile("sizes.ors");
}

void OpenRGBDialog2::on_ShowHide()
{
    if(isHidden())
    {
        show();
    }
    else
    {
        hide();
    }
}

void Ui::OpenRGBDialog2::on_ProfileSelected()
{
    /*---------------------------------------------------------*\
    | Get the profile filename from the selected object         |
    \*---------------------------------------------------------*/
    std::string profile_name = QObject::sender()->objectName().toStdString();

    /*---------------------------------------------------------*\
    | Load the profile                                          |
    \*---------------------------------------------------------*/
    if(profile_manager.LoadProfile(profile_name))
    {
        for(int device = 0; device < ui->DevicesTabBar->count(); device++)
        {
            qobject_cast<OpenRGBDevicePage *>(ui->DevicesTabBar->widget(device))->UpdateDevice();
        }
    }
}

void Ui::OpenRGBDialog2::on_ButtonSaveProfile_clicked()
{
    OpenRGBProfileSaveDialog dialog;

    /*---------------------------------------------------------*\
    | Open Profile Name Dialog                                  |
    \*---------------------------------------------------------*/
    std::string profile_name = dialog.show();

    /*---------------------------------------------------------*\
    | Extension .orp - OpenRgb Profile                          |
    \*---------------------------------------------------------*/
    std::string filename = profile_name + ".orp";

    /*---------------------------------------------------------*\
    | Save the profile                                          |
    \*---------------------------------------------------------*/
    if(profile_manager.SaveProfile(filename))
    {
        RefreshProfileList();
    }
}

void Ui::OpenRGBDialog2::on_ButtonLoadProfile_clicked()
{
    /*---------------------------------------------------------*\
    | Get the profile filename from the profiles list           |
    \*---------------------------------------------------------*/
    std::string profile_name = ui->ProfileBox->currentText().toStdString();

    /*---------------------------------------------------------*\
    | Load the profile                                          |
    \*---------------------------------------------------------*/
    if(profile_manager.LoadProfile(profile_name))
    {
        for(int device = 0; device < ui->DevicesTabBar->count(); device++)
        {
            qobject_cast<OpenRGBDevicePage *>(ui->DevicesTabBar->widget(device))->UpdateDevice();
        }
    }
}

void Ui::OpenRGBDialog2::on_ButtonDeleteProfile_clicked()
{
    /*---------------------------------------------------------*\
    | Get the profile filename from the profiles list           |
    \*---------------------------------------------------------*/
    std::string profile_name = ui->ProfileBox->currentText().toStdString();

    /*---------------------------------------------------------*\
    | Confirm we want to delete the profile                     |
    \*---------------------------------------------------------*/
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete Profile", "Do you really want to delete this profile?", QMessageBox::Yes|QMessageBox::No);

    /*---------------------------------------------------------*\
    | Load the profile                                          |
    \*---------------------------------------------------------*/
    if(reply == QMessageBox::Yes)
    {
        profile_manager.DeleteProfile(profile_name);

        RefreshProfileList();
    }
}
