#include "QTimer"
#include "QProgressBar"
#include "QPalette"
#include <QString>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <string>
#include "session.h"

// The constructor for mainWindow
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Initialize variables

    // Initialize device and load sessions
    device = new Device();

    // Disable all input widgets until the power is turned on
    ui->decreaseIntensityButton_2->setDisabled(true);
    ui->increaseIntensityButton_2->setDisabled(true);
    ui->pushButton_4->setDisabled(true);
    ui->pushButton_5->setDisabled(true);
    ui->pushButton_6->setDisabled(true);
    ui->pushButton_7->setDisabled(true);
    ui->pushButton_8->setDisabled(true);
    ui->comboBox->setDisabled(true);
    ui->comboBox_2->setDisabled(true);
    ui->plainTextEdit->setDisabled(true);
    ui->saveIntButton->setDisabled(true);

    // Setup progress bar aka battery
    ui->battery->setMinimum(0);
    ui->battery->setMaximum(100);
    ui->battery->setValue(0); // Set initial battery level to 0
    ui->battery->setTextVisible(false); // Disables battery level display

    // QTimer
    timer = new QTimer(this); //used to update the battery level over time
    connect(timer, &QTimer::timeout, this, &MainWindow::updateBattery); // connects QTimer object to updateBattery function
    connect(device, &Device::inactivityTimerUpdate, this, &MainWindow::updateDisplay); // connects QTimer object to updateDisplay function
    connect(device, &Device::powerOffSignal, this, &MainWindow::power);
    // Connections for the QProgressBar
    connect(ui->powerButton, SIGNAL(released()), this, SLOT (power()));   // Power
    // Connections for QPushButtons
    connect(ui->decreaseIntensityButton_2, SIGNAL(released()), this, SLOT (decreaseIntensity()));     // Decrease intensity
    connect(ui->increaseIntensityButton_2, SIGNAL(released()), this, SLOT (increaseIntensity()));     // Increase Intensity
    connect(ui->pushButton_4, SIGNAL(released()), this, SLOT (selectSession()));  // Select Session
    connect(ui->pushButton_5, SIGNAL(released()), this, SLOT (recordSession()));  // Record session
    connect(ui->saveIntButton, SIGNAL(released()), device, SLOT(saveIntensity())); // Save Intensity
    connect(ui->pushButton_6, SIGNAL(released()), this, SLOT(previousSession())); // Display Previous sessions
    // Connections for the QComboBoxes
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), SLOT(sessionGroup())); // Sessions group
    connect(ui->comboBox_2, SIGNAL(currentIndexChanged(int)), SLOT(sessionType())); // Sessions type

    // Connections for Connection buttons

    connect(ui->pushButton_7, SIGNAL(released()), this, SLOT (disconnect()));  // Select Session
    connect(ui->pushButton_8, SIGNAL(released()), this, SLOT (reconnect()));  // Record session

    // Setup Intensity bar
    ui->IntensityBar->setMinimum(1);
    ui->IntensityBar->setMaximum(8);
    ui->IntensityBar->setValue(1);



    std::cout<<ui->IntensityBar->value()<<std::endl;
}
// The deconstructor for mainWindow
MainWindow::~MainWindow()
{
    delete ui;
    delete device;
}

void MainWindow::enableIcons(){
    ui->icon->setStyleSheet("image: url(:/icons/Alpha.svg);");
    ui->icon_2->setStyleSheet("image: url(:/icons/Beta 1.svg);");
    ui->icon_3->setStyleSheet("image: url(:/icons/Theta.svg);");
    ui->icon_4->setStyleSheet("image: url(:/icons/100hz.svg);");
    ui->icon_5->setStyleSheet("image: url(:/icons/twentymin.svg);");
    ui->icon_6->setStyleSheet("image: url(:/icons/fortyfivemin.svg);");
    ui->icon_7->setStyleSheet("image: url(:/icons/customMin.svg);");
}

void MainWindow::disableIcons(){
    ui->icon->setStyleSheet("");
    ui->icon_2->setStyleSheet("");
    ui->icon_3->setStyleSheet("");
    ui->icon_4->setStyleSheet("");
    ui->icon_5->setStyleSheet("");
    ui->icon_6->setStyleSheet("");
    ui->icon_7->setStyleSheet("");
}

void MainWindow::timeout(){
    //if time elapsed hits 2 minutes and run power()
    if(!device->isSessionInProgress() && !device->isDeviceOn()){
        std::cout<<"powering off due to inactivity"<<std::endl;
        power(); // will power down
    }
}

// Function for the "power" pushbutton
void MainWindow::power(){
    ui->textBrowser->clear();
    ui->plainTextEdit->setDisabled(true);
    // if the power is off then turn it on
    if(!device->isDeviceOn()){
        disableIcons(); // disable icons
        // Enable most input widgets
        ui->decreaseIntensityButton_2->setDisabled(false);
        ui->increaseIntensityButton_2->setDisabled(false);
        ui->pushButton_4->setDisabled(false);
        ui->pushButton_5->setDisabled(false);
        ui->pushButton_6->setDisabled(false);
        ui->pushButton_7->setDisabled(false);
        ui->comboBox->setDisabled(false);
        ui->comboBox_2->setDisabled(false);
        device->powerOn();
        // Set battery level to full and make the text visible for it
        ui->battery->setValue(device->getBattery());
        ui->battery->setTextVisible(true); // enables battery level display
        timer->start(1000); // Battery updated every second

        //Set so warning will fire
        if(device->getBattery() <= 18){
            flashes = 0;
        } else if (device->getBattery() < 6){
            ui->pushButton_4->setDisabled(true);
        }
    }else if(device->isDeviceOn() && device->isSessionInProgress()){
        ui->graphicsView->setStyleSheet("");
        disableIcons();
        // If there is an active session and the power is on then terminate the current session and clear the text browser
        ui->textBrowser->clear();
        ui->saveIntButton->setDisabled(true);
        ui->IntensityBar->setValue(1);
        device->stopSession();

        //display low battery flashes
        if(device->getBattery() < 6){
            flashes = 0;
            ui->pushButton_4->setDisabled(true);
        }
    }else if(device->isDeviceOn() && !device->isSessionInProgress()){
        ui->plainTextEdit->setPlainText("Disabled until User Designated Session is selected.");
        ui->graphicsView->setStyleSheet("");
        //if the power is on and there is no active sessions turn the power off
        enableIcons();
        // Disable all input widgets
        ui->decreaseIntensityButton_2->setDisabled(true);
        ui->increaseIntensityButton_2->setDisabled(true);
        ui->pushButton_4->setDisabled(true);
        ui->pushButton_5->setDisabled(true);
        ui->pushButton_7->setDisabled(true);
        ui->pushButton_8->setDisabled(true);
        ui->comboBox->setDisabled(true);
        ui->comboBox_2->setDisabled(true);
        ui->IntensityBar->setValue(1);
        device->powerOff();
        ui->battery->setValue(0);
        ui->battery->setTextVisible(false);
        timer->stop();
    }
}

// Function to update the afk timer
void MainWindow::updateDisplay(int t){
    ui->label_5->setNum(t);
}

// Function for comboBox (more specifically the user designated session part?)
void MainWindow::sessionGroup(){
    // The QPlainText widget is disabled until "user designated session" is selected in comboBox
    ui->plainTextEdit->setDisabled(true);

    if(ui->comboBox->currentText() == "User Designated Session"){
        Wave_t w;
        QString lengthInMinutes;
        ui->plainTextEdit->setDisabled(false);
            if(ui->comboBox_2->currentText() == "Alpha"){
                w = WAVE_ALPHA;
            }else if(ui->comboBox_2->currentText() == "Beta 1"){
                w = WAVE_BETA;
            }else if(ui->comboBox_2->currentText() == "Theta"){
                w = WAVE_THETA;
            }else if(ui->comboBox_2->currentText() == "100 Hz"){
                w = WAVE_100HZ;
            }
        lengthInMinutes.fromStdString(std::to_string(device->getCustomSessionLength(w)));
        ui->plainTextEdit->setPlainText(lengthInMinutes);
    }else if(ui->comboBox->currentText() == "45 Minute Session"){
        ui->plainTextEdit->setPlainText("Disabled until User Designated Session is selected.");
    }else if(ui->comboBox->currentText() == "20 Minute Session"){
        ui->plainTextEdit->setPlainText("Disabled until User Designated Session is selected.");
    }

}

// Function for comboBox_2
void MainWindow::sessionType(){
    if(ui->comboBox_2->currentText() == "Alpha"){

    }else if(ui->comboBox_2->currentText() == "Beta"){

    }else if(ui->comboBox_2->currentText() == "Theta"){

    }else if(ui->comboBox_2->currentText() == "100 Hz"){

    }
}

// Function for the "disconnect" pushbutton
void MainWindow::disconnect(){
    if(device->isSessionInProgress()){
        device->pauseSession();
        flashes = 0;
        device->setConnection(NO_CONNECTION);
        ui->pushButton_7->setDisabled(true);
        ui->pushButton_8->setDisabled(false);
    }

}
// Function for the "reconnect" pushbutton
void MainWindow::reconnect(){
    if(device->isSessionPaused()){
        ui->pushButton_7->setDisabled(false);
        ui->pushButton_8->setDisabled(true);
        if(flashes < 8){
            flashes = 8;
        }
        device->resumeSession();
        if(ui->IntensityBar->value() < 5){
            // If the intensity is between 1 and 4 then it is a good connection
            ui->graphicsView->setStyleSheet("background-image: url(:/icons/excellentconnection.png);");
            device->setConnection(GOOD_CONNECTION);
        }else {
            // If the intensity is between 5 and 8 then it is an okay connection
            ui->graphicsView->setStyleSheet("background-image: url(:/icons/okayconnection.png);");
            device->setConnection(OKAY_CONNECTION);
        }
    }

}
// Function to decrease intensity
void MainWindow::decreaseIntensity(){
    ui->IntensityBar->setValue(ui->IntensityBar->value()-1); // decreases progress bar intensity by 1

    //If a session is active then it will update the textbrowser by calling the selectSession() function
    if(device->isSessionInProgress()){
        device->decreaseIntensity();

        updateSessionTextDisplay();

        //Clear the graphics view because the intensity can only be adjusted when the connection test ends
        ui->graphicsView->setStyleSheet("");
    }
}

// Function to increase intensity
void MainWindow::increaseIntensity(){
    ui->IntensityBar->setValue(ui->IntensityBar->value()+1); // increases progress bar intensity by 1

    // If a session is running, update intensity and update display
    if(device->isSessionInProgress()){
        device->increaseIntensity();

        updateSessionTextDisplay();

        //Clear the graphics view because the intensity can only be adjusted when the connection test ends
        ui->graphicsView->setStyleSheet("");
    }
}

// Function for the "select session" pushbutton
// Takes the values from the comboboxes and starts the corresponding session
void MainWindow::selectSession(){
    LengthGroup_t lengthGroup;
    Wave_t wave;
    if(flashes < 8){
        flashes = 8;
    }
    if(!device->isSessionInProgress()){
        testConnection();
    }
    if(ui->comboBox->currentText() == "User Designated Session"){
        lengthGroup = LENGTH_CUSTOM;
        if(ui->comboBox_2->currentText() == "Alpha"){
            wave = WAVE_ALPHA;
        }else if(ui->comboBox_2->currentText() == "Beta 1"){
            wave = WAVE_BETA;
        }else if(ui->comboBox_2->currentText() == "Theta"){
            wave = WAVE_THETA;
        }else if(ui->comboBox_2->currentText() == "100 Hz"){
            wave = WAVE_100HZ;
        }
    }else if(ui->comboBox->currentText() == "45 Minute Session"){
        lengthGroup = LENGTH_45MIN;
        if(ui->comboBox_2->currentText() == "Alpha"){
            wave = WAVE_ALPHA;
        }else if(ui->comboBox_2->currentText() == "Beta 1"){
            wave = WAVE_BETA;
        }else if(ui->comboBox_2->currentText() == "Theta"){
            wave = WAVE_THETA;
        }else if(ui->comboBox_2->currentText() == "100 Hz"){
            wave = WAVE_100HZ;
        }
    }else if(ui->comboBox->currentText() == "20 Minute Session"){
        lengthGroup = LENGTH_20MIN;
        if(ui->comboBox_2->currentText() == "Alpha"){
            wave = WAVE_ALPHA;
        }else if(ui->comboBox_2->currentText() == "Beta 1"){
            wave = WAVE_BETA;
        }else if(ui->comboBox_2->currentText() == "Theta"){
            wave = WAVE_THETA;
        }else if(ui->comboBox_2->currentText() == "100 Hz"){
            wave = WAVE_100HZ;
        }
    }
    device->selectSession(lengthGroup, wave);
    ui->IntensityBar->setValue(device->getCurrentSession()->getDefaultIntensity()); // Set the bar to the session intensity
    ui->saveIntButton->setDisabled(false);
    updateSessionTextDisplay();
    //Pauses session if no connection was found, allowing for a reconnect
    if(device->getConnection() == NO_CONNECTION){
        disconnect();
    }
}

// Function to test the connection
void MainWindow::testConnection(){
    // If intensity is 1-3 then it is an excellent connection
    if(ui->IntensityBar->value() < 4){
        ui->graphicsView->setStyleSheet("background-image: url(:/icons/excellentconnection.png);");
        device->setConnection(GOOD_CONNECTION);
    }else if(ui->IntensityBar->value() < 7 && ui->IntensityBar->value() > 3){
        // If the intensity is between 4 and 6 then it is an okay connection
        ui->graphicsView->setStyleSheet("background-image: url(:/icons/okayconnection.png);");
        device->setConnection(OKAY_CONNECTION);
    }else{
        // If the intensity is between 7 and 8 then it is a no connection
        ui->graphicsView->setStyleSheet("background-image: url(:/icons/noconnection.png);");
        device->setConnection(NO_CONNECTION);
    }
}

// Function to update textBrowser
void MainWindow::updateSessionTextDisplay(){
    if(ui->comboBox->currentText() == "User Designated Session"){
        if(ui->comboBox_2->currentText() == "Alpha"){
            ui->textBrowser->setText("  Session Group:                  "+ui->comboBox->currentText()+" = "+ui->plainTextEdit->toPlainText()+" Minutes");
            ui->textBrowser->append("  Session Type:                    "+ui->comboBox_2->currentText());
            ui->textBrowser->append("  Intensity Level:                 "+ui->IntensityBar->text());
            ui->icon_7->setStyleSheet("image: url(:/icons/customMin.svg);");
            ui->icon->setStyleSheet("image: url(:/icons/Alpha.svg);");
        }else if(ui->comboBox_2->currentText() == "Beta 1"){
            ui->textBrowser->setText("  Session Group:                  "+ui->comboBox->currentText()+" = "+ui->plainTextEdit->toPlainText()+" Minutes");
            ui->textBrowser->append("  Session Type:                    "+ui->comboBox_2->currentText());
            ui->textBrowser->append("  Intensity Level:                 "+ui->IntensityBar->text());
            ui->icon_7->setStyleSheet("image: url(:/icons/customMin.svg);");
            ui->icon_2->setStyleSheet("image: url(:/icons/Beta 1.svg);");
        }else if(ui->comboBox_2->currentText() == "Theta"){
            ui->textBrowser->setText("  Session Group:                  "+ui->comboBox->currentText()+" = "+ui->plainTextEdit->toPlainText()+" Minutes");
            ui->textBrowser->append("  Session Type:                    "+ui->comboBox_2->currentText());
            ui->textBrowser->append("  Intensity Level:                 "+ui->IntensityBar->text());
            ui->icon_7->setStyleSheet("image: url(:/icons/customMin.svg);");
            ui->icon_3->setStyleSheet("image: url(:/icons/Theta.svg);");
        }else if(ui->comboBox_2->currentText() == "100 Hz"){
            ui->textBrowser->setText("  Session Group:                  "+ui->comboBox->currentText()+" = "+ui->plainTextEdit->toPlainText()+" Minutes");
            ui->textBrowser->append("  Session Type:                    "+ui->comboBox_2->currentText());
            ui->textBrowser->append("  Intensity Level:                 "+ui->IntensityBar->text());
            ui->icon_7->setStyleSheet("image: url(:/icons/customMin.svg);");
            ui->icon_4->setStyleSheet("image: url(:/icons/100hz.svg);");
        }
    }else if(ui->comboBox->currentText() == "45 Minute Session"){
        if(ui->comboBox_2->currentText() == "Alpha"){
            ui->textBrowser->setText("  Session Group:                  "+ui->comboBox->currentText());
            ui->textBrowser->append("  Session Type:                    "+ui->comboBox_2->currentText());
            ui->textBrowser->append("  Intensity Level:                 "+ui->IntensityBar->text());
            ui->icon_6->setStyleSheet("image: url(:/icons/fortyfivemin.svg);");
            ui->icon->setStyleSheet("image: url(:/icons/Alpha.svg);");
        }else if(ui->comboBox_2->currentText() == "Beta 1"){
            ui->textBrowser->setText("  Session Group:                  "+ui->comboBox->currentText());
            ui->textBrowser->append("  Session Type:                    "+ui->comboBox_2->currentText());
            ui->textBrowser->append("  Intensity Level:                 "+ui->IntensityBar->text());
            ui->icon_6->setStyleSheet("image: url(:/icons/fortyfivemin.svg);");
            ui->icon_2->setStyleSheet("image: url(:/icons/Beta 1.svg);");
        }else if(ui->comboBox_2->currentText() == "Theta"){
            ui->textBrowser->setText("  Session Group:                  "+ui->comboBox->currentText());
            ui->textBrowser->append("  Session Type:                    "+ui->comboBox_2->currentText());
            ui->textBrowser->append("  Intensity Level:                 "+ui->IntensityBar->text());
            ui->icon_6->setStyleSheet("image: url(:/icons/fortyfivemin.svg);");
            ui->icon_3->setStyleSheet("image: url(:/icons/Theta.svg);");
        }else if(ui->comboBox_2->currentText() == "100 Hz"){
            ui->textBrowser->setText("  Session Group:                  "+ui->comboBox->currentText());
            ui->textBrowser->append("  Session Type:                    "+ui->comboBox_2->currentText());
            ui->textBrowser->append("  Intensity Level:                 "+ui->IntensityBar->text());
            ui->icon_6->setStyleSheet("image: url(:/icons/fortyfivemin.svg);");
            ui->icon_4->setStyleSheet("image: url(:/icons/100hz.svg);");
        }
    }else if(ui->comboBox->currentText() == "20 Minute Session"){
        if(ui->comboBox_2->currentText() == "Alpha"){
            ui->textBrowser->setText("  Session Group:                  "+ui->comboBox->currentText());
            ui->textBrowser->append("  Session Type:                    "+ui->comboBox_2->currentText());
            ui->textBrowser->append("  Intensity Level:                 "+ui->IntensityBar->text());
            ui->icon_5->setStyleSheet("image: url(:/icons/twentymin.svg);");
            ui->icon->setStyleSheet("image: url(:/icons/Alpha.svg);");
        }else if(ui->comboBox_2->currentText() == "Beta 1"){
            ui->textBrowser->setText("  Session Group:                  "+ui->comboBox->currentText());
            ui->textBrowser->append("  Session Type:                    "+ui->comboBox_2->currentText());
            ui->textBrowser->append("  Intensity Level:                 "+ui->IntensityBar->text());
            ui->icon_5->setStyleSheet("image: url(:/icons/twentymin.svg);");
            ui->icon_2->setStyleSheet("image: url(:/icons/Beta 1.svg);");
        }else if(ui->comboBox_2->currentText() == "Theta"){
            ui->textBrowser->setText("  Session Group:                  "+ui->comboBox->currentText());
            ui->textBrowser->append("  Session Type:                    "+ui->comboBox_2->currentText());
            ui->textBrowser->append("  Intensity Level:                 "+ui->IntensityBar->text());
            ui->icon_5->setStyleSheet("image: url(:/icons/twentymin.svg);");
            ui->icon_3->setStyleSheet("image: url(:/icons/Theta.svg);");
        }else if(ui->comboBox_2->currentText() == "100 Hz"){
            ui->textBrowser->setText("  Session Group:                  "+ui->comboBox->currentText());
            ui->textBrowser->append("  Session Type:                    "+ui->comboBox_2->currentText());
            ui->textBrowser->append("  Intensity Level:                 "+ui->IntensityBar->text());
            ui->icon_5->setStyleSheet("image: url(:/icons/twentymin.svg);");
            ui->icon_4->setStyleSheet("image: url(:/icons/100hz.svg);");
        }
    }
}


// Function for the "record session" pushbutton
void MainWindow::recordSession(){
    if(!device->isSessionInProgress()){
        return;
    }
    // creates a txt file in the absolute directory
    QString record = QCoreApplication::applicationDirPath()+ "/Previous Session.txt";
    QFile previous(record);
    previous.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream out (&previous);
    QString textbox = ui->textBrowser->toPlainText();


    // ouputs text from the textbrowser to the txt file
    out << "Previous Session(s)" << endl;
    out << " "<< endl;
    out << textbox << endl;
    previous.close();

}

// Function that will display the last session run
void MainWindow::previousSession(){
    // locates file in the directory
    QFile record (QCoreApplication::applicationDirPath()+ "/Previous Session.txt");
    record.open(QIODevice::ReadOnly);

    // outputs all text from txt file to the textbrowser
    QTextStream previous(&record);
    ui ->textBrowser->setText(previous.readAll());
}

// This function updates the battery level based upon session and intensity
void MainWindow::updateBattery(){
    if(device->isDeviceOn()){
        if (device->isSessionInProgress() && !(device->isSessionPaused())){
            double intensity = ui->IntensityBar->value();
            device->lowerBattery(intensity);
        } else {
            device->lowerBattery();
        }
        int bat = device->getBattery();
        ui->battery->setValue(bat);
        if(bat <=0){
            // If battery is depleted, turn off
            power();
        }
        if (bat <= 5){
            //one bar on graph stop session
            if (device->isSessionInProgress()){
                power();
            }
        }

        //Display low battery signal, alternating, flashes set to 0 wherer a low power is required to be displayed
        if(flashes < 7){
            if(flashes % 2 == 0){
                ui->graphicsView->setStyleSheet("background-image: url(:/icons/nobars.png);");
                flashes += 1;
            } else if (bat <= 5){
                ui->graphicsView->setStyleSheet("background-image: url(:/icons/onebar.png);");
                flashes += 1;
            } else {
                ui->graphicsView->setStyleSheet("background-image: url(:/icons/noconnection.png);");
                flashes += 1;
            }
        }
    }
}
