#include "filemodel.h"

FileModel::FileModel(QObject *parent_)
    : QAbstractListModel(parent_) {}


FileModel::~FileModel() {}

int FileModel::rowCount(const QModelIndex & /* index_ */) const
{
    if(_file)
        return _file->tracks();

    return 0;
}


void FileModel::setFile(CxxMidi::File * file_)
{
    _file = file_;
    this->layoutChanged();
}

void FileModel::removeTrack(int num_)
{
    this->beginRemoveRows(QModelIndex(),num_,num_);
    _file->erase(_file->begin() + num_);
    this->endRemoveRows();
}

void FileModel::addTrack(int num_)
{
    this->beginInsertRows(QModelIndex(),num_,num_);
    CxxMidi::File::iterator it = _file->begin() +num_;
    _file->insert(it,CxxMidi::Track());
    this->endInsertRows();
}

QVariant FileModel::data(const QModelIndex & index_, int role_) const
{
    if(!index_.isValid())
        return QVariant();

    int row = index_.row();

    if(role_ == Qt::DisplayRole)
    {
        CxxMidi::Track * track = &_file->at(row);
        return QVariant(QString("%1. %2").arg(row)
                        .arg(track->getName().c_str()));
    }

    return QVariant();
}

void FileModel::clearTrack(int num_)
{
    CxxMidi::Track *track = &_file->at(num_);

    CxxMidi::Track::iterator iter = track->begin();
    while (iter != track->end())
    {
        CxxMidi::Event * event = &(*iter);

        if(!event->size())
        {
            ++iter;
            continue;
        }

        uint8_t type = event->at(0);

        if( (type & 0xf0) != CxxMidi::Event::NoteOn)
            iter = track->erase(iter);
        else
            ++iter;

    }

}
