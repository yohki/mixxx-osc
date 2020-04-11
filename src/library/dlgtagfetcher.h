#pragma once

#include <QDialog>
#include <QList>
#include <QTreeWidget>

#include "library/trackmodel.h"
#include "library/ui_dlgtagfetcher.h"
#include "musicbrainz/tagfetcher.h"
#include "track/track.h"

class DlgTagFetcher : public QDialog,  public Ui::DlgTagFetcher {
  Q_OBJECT

  public:
    DlgTagFetcher(QWidget* parent, const TrackModel* trackModel = nullptr);
    ~DlgTagFetcher() override = default;

    void init();

  public slots:
    void loadTrack(const TrackPointer& track);
    void loadTrack(const QModelIndex& index);

  signals:
    void next();
    void previous();

  private slots:
    void fetchTagFinished(
            TrackPointer pTrack,
            QList<mixxx::musicbrainz::TrackRelease> guessedTrackReleases);
    void resultSelected();
    void fetchTagProgress(QString);
    void slotNetworkResult(int httpStatus, QString app, QString message, int code);
    void slotTrackChanged(TrackId trackId);
    void apply();
    void quit();
    void slotNext();
    void slotPrev();

  private:
    void changeTrack(const TrackPointer& track);
    void updateStack();
    void addDivider(const QString& text, QTreeWidget* parent) const;

    TagFetcher m_tagFetcher;

    TrackPointer m_track;

    struct Data {
        Data() : m_pending(true), m_selectedResult(-1) {}

        bool m_pending;
        int m_selectedResult;
        QList<mixxx::musicbrainz::TrackRelease> m_results;
    };
    Data m_data;

    enum class NetworkResult {
        Ok,
        HttpError,
        UnknownError,
    };
    NetworkResult m_networkResult;
    const TrackModel* m_pTrackModel;
    QModelIndex m_currentTrackIndex;
};
