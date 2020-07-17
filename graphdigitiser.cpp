#include "graphdigitiser.h"
#include "ui_graphdigitiser.h"

GraphDigitiser::GraphDigitiser(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GraphDigitiser)
{
    ui->setupUi(this);

    ui->graphicsView->setScene(scene); // connecting the Scene to the View
    ui->XcomboBox->addItem("Lin");
    ui->XcomboBox->addItem("Log");

    ui->YcomboBox->addItem("Lin");
    ui->YcomboBox->addItem("Log");
}

GraphDigitiser::~GraphDigitiser()
{
    delete ui;
}

void GraphDigitiser::mousePressEvent(QMouseEvent *event)
{
    if(X_isCalibrating) // This part of code is executed in the calibration phase only
      {
         X_CalibrationVector.push_back(event->x());  // Add the point object to the calibration vector
         X_CalibrationCheck(); // A function to verify that the calibration is done andto calculate the Lengend's length
      }

    if(Y_isCalibrating) // This part of code is executed in the calibration phase only
      {
         Y_CalibrationVector.push_back(event->y());  // Add the point object to the calibration vector
         Y_CalibrationCheck(); // A function to verify that the calibration is done andto calculate the Lengend's length
      }

    if(AutorRun_isCalibrating) //This part of the code is executed to calibrate autorun
    {
        X_AutoRunCalibVector.push_back(event->x());
        Y_AutoRunCalibVector.push_back(event->y());
        AutorRun_isCalibrating = false;
        AutoRun_CalibrationCheck();
    }

    if (!X_isCalibrating && !Y_isCalibrating) {     //calulates most recent value for clicked point and sets LCD to show it
        if(xLin){
            xValue = ((event->x())-xOrigin)/xScale;
            ui -> lcdNumber_X -> display(xValue);
        } else{
            xValue = qPow(10,(((event->x())-xOrigin)/xScale));
            ui -> lcdNumber_X -> display(xValue);
        }

        if(yLin){
            yValue = ((event->y())-yOrigin)/yScale;
            ui -> lcdNumber_Y -> display(yValue);
        }  else{
            yValue = qPow(10,(-((event->y())-yOrigin)/yScale));
            ui -> lcdNumber_Y -> display(yValue);
        }
    }

    if(isRecording) { // This part of code is executed when the user intends to record p

         X_AxisVector.push_back(xValue); // Add the x coord of the click to x vector
         Y_AxisVector.push_back(yValue); // Add the y coord of the click to y vector
      }
}

//******************************************Calibration Checks***********************************************

void GraphDigitiser::X_CalibrationCheck() { // This function is called to check and finish X-axis calibration
    if (X_CalibrationVector.size()==2) // We need only two points to finish calibration
        {
          X_isCalibrating=false; // Switching off calibration as soon as we have two points

          bool ok; // This boolean varaible is True if the user accepts the dialog and false if they rejec the dialog (press "Cancel" intead of "OK")
          X0_Calibration_Value = QInputDialog::getDouble(this, tr("X-Axis Calibration"),tr("Enter the LOWER value which was clicked"), 0, -10000000000, 10000000000, 1, &ok);
          X1_Calibration_Value = QInputDialog::getDouble(this, tr("X-Axis Calibration"),tr("Enter the LARGER value which was clicked"), 0, -10000000000, 10000000000, 1, &ok);


          if(xLin) {     //x axis is linear

              xScale=(qAbs(X_CalibrationVector[0]-X_CalibrationVector[1]))/(X1_Calibration_Value - X0_Calibration_Value); // Calculates the calibration factor
              double Xmin = *std::min_element(X_CalibrationVector.constBegin(), X_CalibrationVector.constEnd());
              xOrigin = Xmin - (X0_Calibration_Value * xScale);
          }

          else if(xLog){     //x axis is log

              xScale = (qAbs(X_CalibrationVector[0]-X_CalibrationVector[1]))/(log10(X1_Calibration_Value/X0_Calibration_Value));    //pixels per unit log
              double Xmin = *std::min_element(X_CalibrationVector.constBegin(), X_CalibrationVector.constEnd());
              xOrigin = Xmin - log10(X0_Calibration_Value) * xScale;    // origin = lowerX pos - (log lower value * xScale)
          }

              X_CalibrationVector.clear(); // Clears the vector for calibration to allow for recalibration if needed
              xCanRecord = true;
       }
}

void GraphDigitiser::Y_CalibrationCheck() { // This function is called to check and finish Y-axis calibration
    if (Y_CalibrationVector.size()==2) // We need only two points to finish calibration
         {
          Y_isCalibrating=false; // Switching off calibration as soon as we have two points

          bool ok; // This boolean varaible is True if the user accepts the dialog and false if they rejec the dialog (press "Cancel" intead of "OK")
          Y0_Calibration_Value = QInputDialog::getDouble(this, tr("Y-Axis Calibration"),tr("Enter the LOWER value which was clicked"), 0, -10000000000, 1000000000, 1, &ok);
          Y1_Calibration_Value = QInputDialog::getDouble(this, tr("Y-Axis Calibration"),tr("Enter the LARGER value which was clicked"), 0, -10000000000, 10000000000, 1, &ok);

          if(yLin) {     //y axis is linear

              yScale=(abs(Y_CalibrationVector[0]-Y_CalibrationVector[1]))/(Y0_Calibration_Value - Y1_Calibration_Value); // Calculates the yScale
              double Ymax = *std::max_element(Y_CalibrationVector.constBegin(), Y_CalibrationVector.constEnd());
              yOrigin = Ymax - (Y0_Calibration_Value * yScale);

          }
          else if(yLog) {    //y axis is Logarithmic

              yScale = (qAbs(Y_CalibrationVector[0]-Y_CalibrationVector[1]))/(log10(Y1_Calibration_Value/Y0_Calibration_Value));    //pixels per unit log
              double Ymax = *std::max_element(Y_CalibrationVector.constBegin(), Y_CalibrationVector.constEnd());
              yOrigin = Ymax + log10(Y0_Calibration_Value) * yScale;    // origin = lower("higher pixel count")Y pos + (log lower value * yScale)

          }
          Y_CalibrationVector.clear(); // Clears the vector for calibration to allow for recalibration if needed
          yCanRecord = true;
       }
}

void GraphDigitiser::AutoRun_CalibrationCheck()
{
    if (Y_AutoRunCalibVector.size()==1) {// We need only one point to finish calibration

        //calculating pixeel disatance of known point to origin
        xOrigDistance = X_AutoRunCalibVector[0] - xOrigin;
        yOrigDistance = Y_AutoRunCalibVector[0] - yOrigin;

        QMessageBox::information(this,"AutoScan", "Using the 'eyedropper tool' in the following window, select the colour of the line you would like to scan.");
        QColor color = QColorDialog::getColor();

        QImage myGraph(filename);
        myGraph.load(filename);
        myGraph = myGraph.scaled(800, 800, Qt::KeepAspectRatio);

        for (int y = 0; y< myGraph.height(); y=y+3) {
            for (int x = 0 ; x < myGraph.width(); x=x+3) {
                if (myGraph.pixelColor(x,y) == color) {
                    X_AutoRunVector.push_back(x);
                    Y_AutoRunVector.push_back(y);
                }
            }
        }
        double xImageMax = *std::max_element(X_AutoRunVector.constBegin(), X_AutoRunVector.constEnd());
        double MaxIndex = X_AutoRunVector.indexOf(xImageMax);

        //Calculating pixel origin of scanned image (this enables values to be calculated)
        new_xOrigin = xImageMax - xOrigDistance;
        new_yOrigin = Y_AutoRunVector[MaxIndex] - yOrigDistance;


        QFile file("AutoScan.csv");
        file.open(QIODevice::WriteOnly | QIODevice::Text);

        QTextStream output(&file);
        output << "X" << "," << "Y" << endl;

        for (int i = 0; i < X_AutoRunVector.size(); i++) {
                if (xLin) {
                    xNewVal = (X_AutoRunVector[i]-new_xOrigin)/xScale;
                } else {
                    xNewVal = qPow(10,((X_AutoRunVector[i]-new_xOrigin)/xScale));
                }

                if (yLin) {
                    yNewVal = (Y_AutoRunVector[i]-new_yOrigin)/yScale;
                } else {
                    yNewVal = pow(10,(-(Y_AutoRunVector[i]-new_yOrigin))/yScale);
                }

                output << xNewVal << "," << yNewVal << endl;
            }


        file.flush();
        file.close();
        X_AutoRunCalibVector.clear();
        Y_AutoRunCalibVector.clear();
        X_AutoRunVector.clear();
        Y_AutoRunVector.clear();
        QMessageBox::information(this,"AutoScan", "Autoscan complete. The digitised graph has been saved as 'AutoScan.csv'");
    }
    else{
        QMessageBox::warning(this,"X and Y axis not calibrated", "Calibrate both axis before using AutoScan.");
    }
}

//******************************************CHECK BOX CODE*************************************************

void GraphDigitiser::on_xLogCheckBox_stateChanged(int arg1)
{
    if(arg1){       //if user ticks this box...
        ui->xLinCheckBox->setChecked(false);
        ui->XcomboBox->setCurrentText("Log");
        ui->xLinLog_Button->setText("Log");
        xLin = false;
        xLog = true;
    }
    else{
        ui->xLogCheckBox->setChecked(false);
        ui->XcomboBox->setCurrentText("Lin");
        ui->xLinLog_Button->setText("Lin");
        xLin = true;
        xLog = false;
    }
}

void GraphDigitiser::on_xLinCheckBox_stateChanged(int arg1)
{
    if(arg1){       //if user ticks this box...
        ui->xLogCheckBox->setChecked(false);
        ui->XcomboBox->setCurrentText("Lin");
        ui->xLinLog_Button->setText("Lin");
        xLin = true;
        xLog = false;
    }
    else{
        ui->xLinCheckBox->setChecked(false);
        ui->XcomboBox->setCurrentText("Log");
        ui->xLinLog_Button->setText("Log");
        xLin = false;
        xLog = true;
    }
}

void GraphDigitiser::on_yLogCheckBox_stateChanged(int arg1)
{
    if(arg1){       //if user ticks this box...
        ui->yLinCheckBox->setChecked(false);
        ui->YcomboBox->setCurrentText("Log");
        ui->yLinLog_Button->setText("Log");
        yLin = false;
        yLog = true;
    }
    else{
        ui->yLogCheckBox->setChecked(false);
        ui->YcomboBox->setCurrentText("Lin");
        ui->yLinLog_Button->setText("Lin");
        yLin = true;
        yLog = false;
    }
}

void GraphDigitiser::on_yLinCheckBox_stateChanged(int arg1)
{
    if(arg1){       //if user ticks this box...
        ui->yLogCheckBox->setChecked(false);
        ui->YcomboBox->setCurrentText("Lin");
        ui->yLinLog_Button->setText("Lin");
        yLin = true;
        yLog = false;
    }
    else{
        ui->yLinCheckBox->setChecked(false);
        ui->YcomboBox->setCurrentText("Log");
        ui->yLinLog_Button->setText("Log");
        yLin = false;
        yLog = true;
    }
}

//******************************************COMBO BOX CODE*************************************************

void GraphDigitiser::on_XcomboBox_currentIndexChanged(int index)
{
    if(ui->XcomboBox->currentText()=="Lin") {
        ui->xLinCheckBox->setChecked(true);
        ui->xLogCheckBox->setChecked(false);
        xLin = true;
        xLog = false;
    }
    else if(ui->XcomboBox->currentText()=="Log") {
        ui->xLinCheckBox->setChecked(false);
        ui->xLogCheckBox->setChecked(true);
        xLin = false;
        xLog = true;

    }
}

void GraphDigitiser::on_YcomboBox_currentIndexChanged(int index)
{
    if(ui->YcomboBox->currentText()=="Lin") {
        ui->yLinCheckBox->setChecked(true);
        ui->yLogCheckBox->setChecked(false);
        yLin = true;
        yLog = false;
    }
    else if(ui->YcomboBox->currentText()=="Log") {
        ui->yLinCheckBox->setChecked(false);
        ui->yLogCheckBox->setChecked(true);
        yLin = false;
        yLog = true;

    }
}

//*****************************************Button Code*******************************************************

void GraphDigitiser::on_LoadGraphButton_clicked()
{// Called whenever the user clicks on "Load Graph"

   scene->clear();
   filename=QFileDialog::getOpenFileName(this,tr("Load Graph"),tr("*.png")); // Allows the usr to choose an input file
   QPixmap map(filename); // Saves the file in a a QPixmap object
   scene->addPixmap(map.scaled(800,800,Qt::KeepAspectRatio)); // Adds the pixmap to the scene
   GraphImported = true; //update flag to say that a graph has been imported
}

void GraphDigitiser::on_CalibXpushButton_clicked()
{ // Called whenever the user clicks on "Calibrate X-Axis"
    if(GraphImported){
        QMessageBox::about(this, "Calibrate X axis Scale", "Click two defined values on the X-axis.");
        X_isCalibrating = true; // Sets the X calibration flag to True
    }
    else{
        QMessageBox::warning(this,"No Graph Imported", "Load Graph before performing Calibration.");
    }
}

void GraphDigitiser::on_CalibYpushButton_clicked()
{ // Called whenever the user clicks on "Calibrate Y-Axis"
    if(GraphImported){
        QMessageBox::about(this, "Calibrate Y axis Scale", "Click two defined values on the Y-axis.");
        Y_isCalibrating = true; // Sets the Y calibration flag to True
    }
    else{
        QMessageBox::warning(this,"No Graph Imported", "Load Graph before performing Calibration.");
    }
}

void GraphDigitiser::on_xLinLog_Button_clicked()
{
    if(ui->xLinLog_Button->text()=="Lin"){          //if clicked and was on Lin
        ui->xLinLog_Button->setText("Log");
        ui->xLinCheckBox->setChecked(false);
        ui->xLogCheckBox->setChecked(true);
        ui->XcomboBox->setCurrentText("Log");
        xLin = false;
        xLog = true;

    }
    else if(ui->xLinLog_Button->text()=="Log"){     //if clicked and was on Log
        ui->xLinLog_Button->setText("Lin");
        ui->xLinCheckBox->setChecked(true);
        ui->xLogCheckBox->setChecked(false);
        ui->XcomboBox->setCurrentText("Lin");
        xLin = true;
        xLog = false;
    }
}

void GraphDigitiser::on_yLinLog_Button_clicked()
{
    if(ui->yLinLog_Button->text()=="Lin"){          //if clicked and was on Lin
        ui->yLinLog_Button->setText("Log");
        ui->yLinCheckBox->setChecked(false);
        ui->yLogCheckBox->setChecked(true);
        ui->YcomboBox->setCurrentText("Log");
        yLin = false;
        yLog = true;
    }
    else if(ui->yLinLog_Button->text()=="Log"){     //if clicked and was on Log
        ui->yLinLog_Button->setText("Lin");
        ui->yLinCheckBox->setChecked(true);
        ui->yLogCheckBox->setChecked(false);
        ui->YcomboBox->setCurrentText("Lin");
        yLin = true;
        yLog = false;
    }
}

void GraphDigitiser::on_Export_DataSetButton_clicked()
{
    //open csv file
    QFile file("./DigitisedGraph.csv");
    file.open(QIODevice::WriteOnly | QIODevice::Text);

    //write csv file
    QTextStream out(&file);
    out << "X,Y" << endl;       //declaring column titles

    for (int i = 0; i < Y_AxisVector.size(); ++i){              //loop through all stored values writing to csv
        out << X_AxisVector[i] << "," << Y_AxisVector[i] << endl;
    }
    file.flush();
    file.close();
}

void GraphDigitiser::on_AutoScan_clicked()
{
    if (xCanRecord == true && yCanRecord == true){
        QMessageBox::about(this,"AutoScan Commencing", "Click the line where it's x-axis value is the largest. (The furthest to the right point on the line)");
        AutorRun_isCalibrating = true;
    }
else {
    QMessageBox::warning(this,"Not Calibrated", "Calibrate both axis before recording points.");
    }
}

void GraphDigitiser::on_RecordPushButton_clicked()
{   // Called whenever the user clicks on "Record"
    if (xCanRecord == true && yCanRecord == true){
        QMessageBox::about(this,"Application Recording", "Application now recording. Click each point on the line you would like to digitise.");
        isRecording = true; // Sets the recording flag to true
    }
    else {
        QMessageBox::warning(this,"Not Calibrated", "Calibrate both axis before recording points.");
    }
}

void GraphDigitiser::on_StopRecordpushButton_clicked()
{
    isRecording = false; // Sets the recording flag to false
}

void GraphDigitiser::on_FlushRecordingButton_clicked()
{
    X_AxisVector.clear();
    Y_AxisVector.clear();
}

