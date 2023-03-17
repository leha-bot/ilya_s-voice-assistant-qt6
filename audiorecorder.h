#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

#include <QObject>
#include <QMediaRecorder>
#include <QMediaCaptureSession>
#include <QAudioSource>
#include <QUrl>
#include <QStandardPaths>
#include <qmediadevices.h>
#include <qmediaformat.h>
#include <qaudiodevice.h>
#include <qaudiobuffer.h>
#include <qaudioinput.h>
#include <qimagecapture.h>
#include <QMimeType>
#include <QDir>
#include <QFileDialog>
#include <QUuid>
#include <QTimer>



class AudioProbeDevice : public QIODevice
{
   Q_OBJECT
   QAudioFormat format;
public:
   AudioProbeDevice (QObject* parent = nullptr) : QIODevice(parent) {}

   qint64 readData(char *data, qint64) override {return 0;}
   qint64 writeData(const char *data, qint64 maxSize) override
   {
      format.setSampleRate(8000);
      format.setChannelCount(1);
      format.setSampleFormat(QAudioFormat::Int16);
      QAudioBuffer buffer({data, static_cast<int>(maxSize)}, format);
      emit audioAvailable(buffer);
      return maxSize;
   }
   Q_SIGNAL void audioAvailable(const QAudioBuffer &buffer);
};



class AudioRecorder : public QObject
{
    Q_OBJECT
public:
    explicit AudioRecorder(QObject *parent = nullptr);
    ~AudioRecorder();

private:

    QMediaRecorder* audioRecorder;
    QMediaCaptureSession captureSession;

    QUrl url;
    QString fileName;
    QString filePath;
    const QDir location = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    qreal thresholdOfAudioinputActivation = 60.0;
    bool statusSpeech;
    bool newStatusSpeech;
//    QTimer timerOfFinishedSpeech;

//    const int intervalRecording = 15000;
//    QTimer voiceCommandWaitTimer;
    QString modeAudioInput;


    //Получение необработанных данных и уровня громкости
    AudioProbeDevice* audioProbe;
    QAudioSource* sourceAudio;

signals:

    void sendPathToAudioFile(QString);

public slots:

    void toggleRecord(bool);
    void processBuffer(QAudioBuffer);
    void timeoutOfSpeech();
    void restartWaitTimer();

};

#endif // AUDIORECORDER_H
