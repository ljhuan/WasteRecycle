#ifndef CAMERAMODEL_H
#define CAMERAMODEL_H

#include <QAbstractTableModel>
#include "json/json.h"

class CameraModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum CameraRoles {
        DeviceSerialRole = Qt::UserRole + 1,
        CameraNoRole,
        CameraNameRole,
        StatusRole,
        IsSharedRole,
        IsEncryptRole,
        VideoLevelRole
    };

    explicit CameraModel(QObject *parent = 0);

    void setCameraModel(const QByteArray& json);
    QString getSerial(const QModelIndex& index);
    int getCameraNo(const QModelIndex& index);
    int getIsEncrypt(const QModelIndex& index);
    void setIsEncrypt(const QModelIndex& index, int isEncrypt);
    int getVideoLevel(const QModelIndex& index);
    void setVideoLevel(const QModelIndex& index, const int videoLevel);
    void setVideoLevel(const QModelIndex& index);
    void removeRow(const QModelIndex& index);

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex&  parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

protected:
    virtual QHash<int, QByteArray> roleNames() const;

signals:

public slots:

private:
	QList<Json::Value> m_cameraList;
};

#endif // CAMERAMODEL_H
