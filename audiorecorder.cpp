#include "audiorecorder.h"


AudioRecorder::AudioRecorder(QObject *parent) : QObject{parent}
{
    audioRecorder = new QMediaRecorder(this);
    captureSession.setRecorder(audioRecorder);
    captureSession.setAudioInput(new QAudioInput(this));
    captureSession.audioInput()->setDevice(QMediaDevices::audioInputs().at(0)); //выбранное в ОС всегда идёт 1-ым
    QMediaFormat format;
    format.setAudioCodec(QMediaFormat::AudioCodec::MP3);
    format.setFileFormat(QMediaFormat::FileFormat::MP3);

    //Если ставлю opus/ogg, то упаковывает в контейнер M4A
//    format.setAudioCodec(QMediaFormat::AudioCodec::Opus);
//    format.setFileFormat(QMediaFormat::FileFormat::Ogg);

    audioRecorder->setMediaFormat(format);
    audioRecorder->setAudioSampleRate(44100);
    audioRecorder->setAudioBitRate(128000);
    audioRecorder->setAudioChannelCount(1);
    audioRecorder->setQuality(QMediaRecorder::Quality::VeryHighQuality);
    audioRecorder->setEncodingMode(QMediaRecorder::ConstantBitRateEncoding);

    //-----Чтение необработанных данных с аудиовхода для измерения амплитуды звука----
    audioProbe = new AudioProbeDevice(this);
    audioProbe->setCurrentReadChannel(1);
    connect(audioProbe, &AudioProbeDevice::audioAvailable, this, &AudioRecorder::processBuffer);
    QAudioFormat formatAudio;
    formatAudio.setSampleRate(8000);
    formatAudio.setChannelCount(1);
    formatAudio.setSampleFormat(QAudioFormat::Int16);
    sourceAudio = new QAudioSource(QMediaDevices::audioInputs().at(0), formatAudio);
    sourceAudio->setBufferSize(32);
    //--------------

    statusSpeech = true;
    newStatusSpeech = false;
//    connect(&timerOfFinishedSpeech, &QTimer::timeout, this, &AudioRecorder::timeoutOfSpeech);
//    connect(&voiceCommandWaitTimer, &QTimer::timeout, this, &AudioRecorder::restartWaitTimer);
    modeAudioInput = "voiceStandby";
}

AudioRecorder::~AudioRecorder()
{
    delete audioRecorder;
}

void AudioRecorder::toggleRecord(bool commandRecord)
{
    modeAudioInput = "voiceStandby";
    if (commandRecord) {
        fileName = QUuid::createUuid().toString();
        filePath=QDir::currentPath();
        filePath += "/" + fileName;
        audioRecorder->setOutputLocation(QUrl::fromLocalFile(filePath));
        audioRecorder->record();
        audioProbe->open(QIODevice::WriteOnly);
        sourceAudio->start(audioProbe);


//        voiceCommandWaitTimer.start(intervalRecording);
        qDebug() << "Новый файл создан";
        qDebug() << "Запись стартовала";
    }
    if (!commandRecord) {
        audioRecorder->stop();
        sourceAudio->stop();
        audioProbe->close();


//        voiceCommandWaitTimer.stop();
//        timerOfFinishedSpeech.stop();
//        qDebug() << QFile::remove(fileName + ".mp3");
    }
}

static qreal getPeakValue(const QAudioFormat &format);
static QVector<qreal> getBufferLevels(const QAudioBuffer &buffer);

template <class T>
static QVector<qreal> getBufferLevels(const T *buffer, int frames, int channels);

// This function returns the maximum possible sample value for a given audio format
qreal getPeakValue(const QAudioFormat& format)
{
    // Note: Only the most common sample formats are supported
    if (!format.isValid())
        return qreal(0);

    switch (format.sampleFormat()) {
    case QAudioFormat::Unknown:
        break;
    case QAudioFormat::Float:
        return qreal(1.00003);
        break;
    case QAudioFormat::Int32:
        return qreal(INT_MAX);
        break;
    case QAudioFormat::Int16:
        return qreal(SHRT_MAX);
        break;
    case QAudioFormat::UInt8:
        return qreal(UINT_MAX);
        break;
    case QAudioFormat::NSampleFormats:
        break;
    }

    return qreal(0);
}

// returns the audio level for each channel
QVector<qreal> getBufferLevels(const QAudioBuffer& buffer)
{
    QVector<qreal> values;
    if (!buffer.format().isValid())
        return values;

    int channelCount = buffer.format().channelCount();
    values.fill(0, channelCount);
    qreal peak_value = getPeakValue(buffer.format());
    if (qFuzzyCompare(peak_value, qreal(0)))
        return values;

    switch (buffer.format().sampleFormat()) {
    case QAudioFormat::Unknown:
    case QAudioFormat::Int32:
        values = getBufferLevels(buffer.constData<qint32>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] /= peak_value;
        break;
    case QAudioFormat::Int16:
        values = getBufferLevels(buffer.constData<qint16>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] /= peak_value;
        break;
    case QAudioFormat::UInt8:
        values = getBufferLevels(buffer.constData<quint8>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] = qAbs(values.at(i) - peak_value / 2) / (peak_value / 2);
        break;
    case QAudioFormat::NSampleFormats:
        break;
    case QAudioFormat::Float:
        break;
    }
    return values;
}
template <class T>


QVector<qreal> getBufferLevels(const T *buffer, int frames, int channels)
{
    QVector<qreal> max_values;
    max_values.fill(0, channels);
    for (int i = 0; i < frames; ++i) {
        for (int j = 0; j < channels; ++j) {
            qreal value = qAbs(qreal(buffer[i * channels + j]));
            if (value > max_values.at(j))
                max_values.replace(j, value);
        }
    }
    return max_values;
}

void AudioRecorder::processBuffer(QAudioBuffer buffer)
{
    QVector<qreal> levels = getBufferLevels(buffer);
    for (int i = 0; i < levels.count(); ++i) {
        if (levels.at(i)*1000.0 < thresholdOfAudioinputActivation) {
            statusSpeech = false;
        }
        else {
            statusSpeech = true;
        }
    }
    if (!statusSpeech && newStatusSpeech) {
        newStatusSpeech = statusSpeech;
//        timerOfFinishedSpeech.start(2000);
    }
    if (statusSpeech && !newStatusSpeech) {
//        newStatusSpeech = statusSpeech;
//        timerOfFinishedSpeech.stop();
//        voiceCommandWaitTimer.stop();
    }
}

void AudioRecorder::timeoutOfSpeech()
{
//    timerOfFinishedSpeech.stop();
//    voiceCommandWaitTimer.stop();
    audioRecorder->stop();
    modeAudioInput = "speechRecording";
//    QFile::remove(fileName + ".mp3");

    emit sendPathToAudioFile(filePath);
    restartWaitTimer();
}

void AudioRecorder::restartWaitTimer()
{
    audioRecorder->stop();
    if (modeAudioInput == "voiceStandby") {
        qDebug() << QFile::remove(fileName + ".mp3");
    }
    toggleRecord(true);
}


