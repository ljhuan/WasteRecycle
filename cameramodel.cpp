#include <QDebug>
#include "cameramodel.h"

CameraModel::CameraModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

void CameraModel::setCameraModel(const QByteArray& json)
{
    beginResetModel();
	Json::Reader reader;
	Json::Value	value;
    if(!reader.parse(json.data(), value))
    {
        return ;
    }
    Json::Value &devListVal = value["data"];
	if(devListVal.isArray()) {
        m_cameraList.clear();
		int devCount = devListVal.size();
		for(int i = 0; i < devCount; i++) {
            Json::Value &cameraListVal = devListVal[i]["cameraInfo"];
            if (cameraListVal.isArray())
            {
                int cameraCount = cameraListVal.size();
                for (int j = 0; j < cameraCount; j++)
                {
                    Json::Value cameraVal = cameraListVal[j];
                    cameraVal["status"] = devListVal[i]["status"];
                    cameraVal["isEncrypt"] = devListVal[i]["isEncrypt"];
                    m_cameraList.push_back(cameraVal);
                }
            }
		}
	} 
    endResetModel();
}

QString CameraModel::getSerial(const QModelIndex& index)
{
    int row = index.row();
    Json::Value json = m_cameraList[row];
    return json["deviceSerial"].asString().c_str();
}

int CameraModel::getCameraNo(const QModelIndex& index)
{
    int row = index.row();
	Json::Value json = m_cameraList[row];
	return json["cameraNo"].asInt();
}

int CameraModel::getIsEncrypt(const QModelIndex& index)
{
    int row = index.row();
	Json::Value json = m_cameraList[row];
	return json["isEncrypt"].asInt();
}

void CameraModel::setIsEncrypt(const QModelIndex& index, int isEncrypt)
{
    int row = index.row();
    m_cameraList[row]["isEncrypt"] = isEncrypt;
}

int CameraModel::getVideoLevel(const QModelIndex& index)
{
    int row = index.row();
    Json::Value json = m_cameraList[row];
    return json["videoLevel"].asInt();
}

void CameraModel::setVideoLevel(const QModelIndex& index, const int videoLevel)
{
    int row = index.row();
    m_cameraList[row]["videoLevel"] = videoLevel;
}

void CameraModel::removeRow(const QModelIndex& index)
{
    int row = index.row();
    beginRemoveRows(QModelIndex(), row, row);
    m_cameraList.removeAt(row);
    endRemoveRows();
}

int CameraModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_cameraList.count();
}

int CameraModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent)
    return roleNames().count();
}

QVariant CameraModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (index.row() < 0 || index.row() >= m_cameraList.count())
        return QVariant();

    int row = index.row();
	Json::Value json = m_cameraList[row];

    // int column = index.column();
    int roleType = CameraNameRole; // Qt::UserRole + 1 + column;
    switch (roleType) {
    // case DeviceSerialRole:
    case CameraNameRole:
    // case IsSharedRole:
		return json[roleNames()[roleType].data()].asString().c_str();
    // case CameraNoRole:
    // case StatusRole:
    // case IsEncryptRole:
    // case VideoLevelRole:
        // return json[roleNames()[roleType].data()].asInt();
    }

    return QVariant();
}

QVariant CameraModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (Qt::DisplayRole != role)
        return QVariant();

    if (Qt::Horizontal == orientation) {
        // return roleNames()[Qt::UserRole+1+section];
        // return roleNames()[CameraNameRole];
        return QString::fromLocal8Bit("相机名称");
    }
    else {
        return section + 1;
    }
}

QHash<int, QByteArray> CameraModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    // roles[DeviceSerialRole] = "deviceSerial";
    // roles[CameraNoRole]   = "cameraNo";
    roles[CameraNameRole] = "cameraName";
//     roles[StatusRole]     = "status";
//     roles[IsSharedRole]   = "isShared";
//     roles[IsEncryptRole]  = "isEncrypt";
//     roles[VideoLevelRole] = "videoLevel";
    return roles;
}
