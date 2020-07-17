#ifndef GRAPHDIGITISER_H
#define GRAPHDIGITISER_H

#include <QFile>
#include <QMessageBox>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QVector>
#include <QPointF>
#include <QInputDialog>
#include <QtMath>
#include <QDebug>
#include <QImage>
#include "QColorDialog"


QT_BEGIN_NAMESPACE
namespace Ui { class GraphDigitiser; }
QT_END_NAMESPACE

class GraphDigitiser : public QMainWindow
{
    Q_OBJECT

public:
    GraphDigitiser(QWidget *parent = nullptr);
    ~GraphDigitiser();

    void mousePressEvent(QMouseEvent *event);
    void X_CalibrationCheck(); // Function to check if X-axis is calibrated
    void Y_CalibrationCheck(); // Function to check if X-axis is calibrated
    void AutoRun_CalibrationCheck();

private slots:
    void on_LoadGraphButton_clicked();
    void on_CalibXpushButton_clicked();
    void on_CalibYpushButton_clicked();
    void on_RecordPushButton_clicked();
    void on_StopRecordpushButton_clicked();
    void on_FlushRecordingButton_clicked();

    void on_XcomboBox_currentIndexChanged(int index);
    void on_YcomboBox_currentIndexChanged(int index);

    void on_xLinLog_Button_clicked();
    void on_yLinLog_Button_clicked();

    void on_xLogCheckBox_stateChanged(int arg1);
    void on_xLinCheckBox_stateChanged(int arg1);
    void on_yLogCheckBox_stateChanged(int arg1);
    void on_yLinCheckBox_stateChanged(int arg1);

    void on_Export_DataSetButton_clicked();
    void on_AutoScan_clicked();


private:
    Ui::GraphDigitiser *ui;
    QGraphicsScene *scene=new QGraphicsScene();

    QString filename;
    QVector<double> X_CalibrationVector;  // Vector for calibrating x-axis
    QVector<double> Y_CalibrationVector;  // Vector for calibrating y-axis

    bool X_isCalibrating=false; // flag to indicate whether the app is in calibration mode or not
    bool Y_isCalibrating=false; // flag to indicate whether the app is in calibration mode or not
    bool AutorRun_isCalibrating = false; //autorun flag
    bool GraphImported = false;  // flag to indicate if graph has been imported yet
    bool xCanRecord = false;
    bool yCanRecord = false;    //require both x and y flags to be set before recording
    bool xLin;   //variables for setting the x/y axis to lin/log
    bool yLin;
    bool xLog;
    bool yLog;

    QVector<double> X_AxisVector; // Vector for saving X-Axis Co-ords
    QVector<double> Y_AxisVector; // Vector for saving Y-Axis Co-ords
    QVector<double> X_AutoRunCalibVector;//Vector for storing x autorun coord
    QVector<double> Y_AutoRunCalibVector;//Vector for storing y autorun coord
    QVector<double> X_AutoRunVector;//Vector for storing x autorun coords
    QVector<double> Y_AutoRunVector;//Vector for storing y autorun coords

    bool isRecording=false; // flag to indicate whether the app is in line point recording mode or not

    double X0_Calibration_Value=0;
    double X1_Calibration_Value=0;
    double Y0_Calibration_Value=0;
    double Y1_Calibration_Value=0;
    double xOrigDistance = 0;   //distance of coordinate to x origin
    double yOrigDistance = 0;   //distance of coordinate to y origin

    double xScale=0; // Varaible to save the calibration factor (units of pixel/unit on axis)
    double yScale=0; // Varaible to save the calibration factor (units of pixel/unit on axis)

    double xOrigin = 0;    //origin location for x axis (in pixels)
    double yOrigin = 0;    //origin location for y axis (in pixels)

    double new_xOrigin = 0; //x origin of autoscanned image
    double new_yOrigin = 0; //y origin of autoscanned image

    double xValue = 0;      //Value of of last clicked x point
    double yValue = 0;      //Value of of last clicked y point
    double xNewVal;         //value of each AutoScanned x point
    double yNewVal;         //value of each AutoScanned y point
};
#endif // GRAPHDIGITISER_H
