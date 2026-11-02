#include "MainWindow.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFrame>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProgressBar>
#include <QPushButton>
#include <QRegularExpression>
#include <QSaveFile>
#include <QScrollArea>
#include <QSettings>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QVersionNumber>
#include <QProcess>
#include <QIcon>
#include <QUrl>

namespace
{
const QUrl kCatalogUrl(QStringLiteral(
    "https://raw.githubusercontent.com/MasterMarcel272/Feather-Photo/main/apps.json"));
const qint64 kMaximumInstallerSize = 512LL * 1024 * 1024;

QNetworkRequest makeRequest(const QUrl& url)
{
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "FeatherManager/0.1.0");
    request.setRawHeader("Accept", "application/vnd.github+json");
    request.setRawHeader("X-GitHub-Api-Version", "2022-11-28");
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);
    request.setTransferTimeout(60000);
    return request;
}
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      m_network(new QNetworkAccessManager(this))
{
    setWindowTitle(tr("Feather Manager"));
    setMinimumSize(760, 540);
    resize(980, 680);

    auto* central = new QWidget(this);
    auto* root = new QVBoxLayout(central);
    root->setContentsMargins(32, 28, 32, 24);
    root->setSpacing(20);

    auto* header = new QHBoxLayout();
    auto* icon = new QLabel(central);
    icon->setPixmap(QIcon(QStringLiteral(":/icons/FeatherManager.png")).pixmap(58, 58));
    icon->setFixedSize(64, 64);
    header->addWidget(icon);

    auto* titleColumn = new QVBoxLayout();
    auto* title = new QLabel(tr("Feather Manager"), central);
    title->setObjectName(QStringLiteral("mainTitle"));
    auto* subtitle = new QLabel(tr("Your creative apps, ready when you are."), central);
    subtitle->setObjectName(QStringLiteral("subtitle"));
    titleColumn->addWidget(title);
    titleColumn->addWidget(subtitle);
    header->addLayout(titleColumn);
    header->addStretch();

    m_refreshButton = new QPushButton(tr("Check for updates"), central);
    m_refreshButton->setObjectName(QStringLiteral("refreshButton"));
    m_refreshButton->setMinimumHeight(42);
    connect(m_refreshButton, &QPushButton::clicked, this, &MainWindow::refreshCatalog);
    header->addWidget(m_refreshButton);
    root->addLayout(header);

    auto* sectionTitle = new QLabel(tr("Applications"), central);
    sectionTitle->setObjectName(QStringLiteral("sectionTitle"));
    root->addWidget(sectionTitle);

    auto* scroll = new QScrollArea(central);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setObjectName(QStringLiteral("appScroll"));

    m_appList = new QWidget(scroll);
    m_appLayout = new QVBoxLayout(m_appList);
    m_appLayout->setContentsMargins(2, 2, 8, 2);
    m_appLayout->setSpacing(14);
    m_appLayout->addStretch();
    scroll->setWidget(m_appList);
    root->addWidget(scroll, 1);

    m_statusLabel = new QLabel(tr("Loading app catalog…"), central);
    m_statusLabel->setObjectName(QStringLiteral("statusLabel"));
    root->addWidget(m_statusLabel);
    setCentralWidget(central);

    setStyleSheet(QStringLiteral(
        "QMainWindow, QWidget { background: #12161d; color: #e7edf6; font-family: 'Segoe UI'; font-size: 10pt; }"
        "QLabel#mainTitle { font-size: 23pt; font-weight: 650; color: #f5f8fc; }"
        "QLabel#subtitle, QLabel#statusLabel { color: #9caabe; }"
        "QLabel#sectionTitle { font-size: 14pt; font-weight: 600; margin-top: 4px; }"
        "QFrame#appCard { background: #1b222c; border: 1px solid #2d3745; border-radius: 12px; }"
        "QLabel#appName { font-size: 15pt; font-weight: 600; color: #f4f7fb; }"
        "QLabel#appDescription { color: #9eabbc; }"
        "QLabel#appVersion { color: #b8c6d9; }"
        "QPushButton { background: #2b3542; color: #eaf0f8; border: 1px solid #3a4758; border-radius: 7px; padding: 9px 15px; }"
        "QPushButton:hover { background: #354458; }"
        "QPushButton:disabled { color: #728093; background: #222a35; }"
        "QPushButton#primaryButton { background: #3478f6; border-color: #3478f6; color: white; font-weight: 600; }"
        "QPushButton#primaryButton:hover { background: #4b8aff; }"
        "QScrollArea { background: transparent; }"
        "QScrollBar:vertical { background: #12161d; width: 10px; margin: 2px; }"
        "QScrollBar::handle:vertical { background: #394657; border-radius: 5px; min-height: 30px; }"
        "QProgressBar { background: #262f3a; border: 0; border-radius: 3px; height: 6px; text-align: center; }"
        "QProgressBar::chunk { background: #3982ff; border-radius: 3px; }"));

    loadBundledCatalog();
    refreshCatalog();
}

void MainWindow::loadBundledCatalog()
{
    QFile file(QStringLiteral(":/catalog/apps.json"));
    if (file.open(QIODevice::ReadOnly))
        populateCatalog(file.readAll());
}

void MainWindow::refreshCatalog()
{
    m_refreshButton->setEnabled(false);
    m_statusLabel->setText(tr("Checking the app catalog and latest versions…"));

    QNetworkReply* reply = m_network->get(makeRequest(kCatalogUrl));
    reply->setProperty("requestType", QStringLiteral("catalog"));
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        handleReply(reply);
    });
}

void MainWindow::populateCatalog(const QByteArray& json)
{
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(json, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject())
        return;

    const QJsonArray appArray = document.object().value(QStringLiteral("apps")).toArray();
    if (appArray.isEmpty())
        return;

    while (QLayoutItem* item = m_appLayout->takeAt(0)) {
        if (QWidget* widget = item->widget())
            widget->deleteLater();
        delete item;
    }
    m_apps.clear();

    for (const QJsonValue& value : appArray) {
        if (value.isObject())
            addApp(value.toObject());
    }
    m_appLayout->addStretch();
}

void MainWindow::addApp(const QJsonObject& object)
{
    AppEntry app;
    app.id = object.value(QStringLiteral("id")).toString();
    app.name = object.value(QStringLiteral("name")).toString();
    app.description = object.value(QStringLiteral("description")).toString();
    app.repository = object.value(QStringLiteral("repository")).toString();
    app.installerAsset = object.value(QStringLiteral("installerAsset")).toString();
    app.installedRegistryKey = object.value(QStringLiteral("installedRegistryKey")).toString();

    static const QRegularExpression idPattern(QStringLiteral("^[A-Za-z0-9_-]+$"));
    static const QRegularExpression repoPattern(QStringLiteral("^[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+$"));
    if (!idPattern.match(app.id).hasMatch() ||
        !repoPattern.match(app.repository).hasMatch() ||
        app.name.isEmpty() || app.installerAsset.isEmpty())
        return;

    auto* card = new QFrame(m_appList);
    card->setObjectName(QStringLiteral("appCard"));
    auto* cardLayout = new QHBoxLayout(card);
    cardLayout->setContentsMargins(18, 18, 18, 18);
    cardLayout->setSpacing(18);

    auto* icon = new QLabel(card);
    icon->setPixmap(QIcon(QStringLiteral(":/icons/FeatherManager.png")).pixmap(64, 64));
    icon->setFixedSize(68, 68);
    cardLayout->addWidget(icon, 0, Qt::AlignTop);

    auto* details = new QVBoxLayout();
    auto* name = new QLabel(app.name, card);
    name->setObjectName(QStringLiteral("appName"));
    auto* description = new QLabel(app.description, card);
    description->setObjectName(QStringLiteral("appDescription"));
    description->setWordWrap(true);
    app.versionLabel = new QLabel(tr("Checking for a release…"), card);
    app.versionLabel->setObjectName(QStringLiteral("appVersion"));
    details->addWidget(name);
    details->addWidget(description);
    details->addWidget(app.versionLabel);
    app.progressBar = new QProgressBar(card);
    app.progressBar->setRange(0, 100);
    app.progressBar->setValue(0);
    app.progressBar->setTextVisible(false);
    app.progressBar->hide();
    details->addWidget(app.progressBar);
    details->addStretch();
    cardLayout->addLayout(details, 1);

    app.installButton = new QPushButton(tr("Install"), card);
    app.installButton->setObjectName(QStringLiteral("primaryButton"));
    app.installButton->setMinimumWidth(150);
    app.installButton->setMinimumHeight(42);
    app.installButton->setEnabled(false);
    connect(app.installButton, &QPushButton::clicked, this, [this, appId = app.id] {
        startDownload(appId);
    });
    cardLayout->addWidget(app.installButton, 0, Qt::AlignVCenter);

    app.card = card;
    const QString appId = app.id;
    m_appLayout->addWidget(card);
    m_apps.insert(appId, app);
}

void MainWindow::fetchLatestRelease(const QString& appId)
{
    auto it = m_apps.find(appId);
    if (it == m_apps.end())
        return;

    QUrl url(QStringLiteral("https://api.github.com/repos/%1/releases/latest").arg(it->repository));
    QNetworkReply* reply = m_network->get(makeRequest(url));
    reply->setProperty("requestType", QStringLiteral("release"));
    reply->setProperty("appId", appId);
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        handleReply(reply);
    });
}

void MainWindow::updateAppCard(AppEntry& app)
{
    if (!app.versionLabel || !app.installButton)
        return;

    const QString installed = installedVersion(app);
    if (app.latestVersion.isEmpty()) {
        app.versionLabel->setText(app.releaseError.isEmpty()
            ? tr("No public release is available yet.")
            : app.releaseError);
        app.installButton->setEnabled(false);
        return;
    }

    if (app.sha256.isEmpty()) {
        app.versionLabel->setText(app.releaseError.isEmpty()
            ? tr("The release has no SHA-256 checksum; installation is disabled.")
            : app.releaseError);
        app.installButton->setEnabled(false);
        return;
    }

    if (installed.isEmpty()) {
        app.versionLabel->setText(tr("Version %1 is ready to install").arg(app.latestVersion));
        app.installButton->setText(tr("Install"));
    } else {
        const QVersionNumber installedNumber = QVersionNumber::fromString(normalizedVersion(installed));
        const QVersionNumber latestNumber = QVersionNumber::fromString(normalizedVersion(app.latestVersion));
        if (QVersionNumber::compare(installedNumber, latestNumber) >= 0) {
            app.versionLabel->setText(tr("Installed · up to date (%1)").arg(installed));
            app.installButton->setText(tr("Reinstall"));
        } else {
            app.versionLabel->setText(tr("Update available: %1 → %2").arg(installed, app.latestVersion));
            app.installButton->setText(tr("Update"));
        }
    }

    app.installButton->setEnabled(!app.downloadUrl.isEmpty() && !app.sha256.isEmpty());
}

void MainWindow::startDownload(const QString& appId)
{
    auto it = m_apps.find(appId);
    if (it == m_apps.end() || it->downloadUrl.isEmpty() || it->sha256.isEmpty())
        return;

    const QUrl url(it->downloadUrl);
    if (!url.isValid() || url.scheme() != QStringLiteral("https") ||
        url.host().compare(QStringLiteral("github.com"), Qt::CaseInsensitive) != 0) {
        m_statusLabel->setText(tr("The download link is not a trusted GitHub URL."));
        return;
    }

    it->installButton->setEnabled(false);
    it->progressBar->setValue(0);
    it->progressBar->show();
    it->versionLabel->setText(tr("Downloading installer…"));
    m_statusLabel->setText(tr("Downloading %1 from its GitHub release…").arg(it->name));

    QNetworkReply* reply = m_network->get(makeRequest(url));
    reply->setProperty("requestType", QStringLiteral("download"));
    reply->setProperty("appId", appId);
    connect(reply, &QNetworkReply::downloadProgress, this,
            [this, appId, reply](qint64 received, qint64 total) {
        auto appIt = m_apps.find(appId);
        if (appIt == m_apps.end() || !appIt->progressBar)
            return;
        if (received > kMaximumInstallerSize || total > kMaximumInstallerSize) {
            reply->setProperty("tooLarge", true);
            reply->abort();
            return;
        }
        if (total > 0 && total <= kMaximumInstallerSize) {
            appIt->progressBar->setValue(static_cast<int>((received * 100) / total));
        }
    });
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        handleReply(reply);
    });
}

void MainWindow::handleReply(QNetworkReply* reply)
{
    const QString requestType = reply->property("requestType").toString();
    const QString appId = reply->property("appId").toString();
    const QNetworkReply::NetworkError networkError = reply->error();
    const QString errorText = reply->errorString();
    const QByteArray data = reply->readAll();
    reply->deleteLater();

    if (requestType == QStringLiteral("catalog")) {
        m_refreshButton->setEnabled(true);
        if (networkError == QNetworkReply::NoError)
            populateCatalog(data);
        else
            m_statusLabel->setText(tr("Could not refresh the catalog. Trying the bundled app list…"));

        for (auto it = m_apps.cbegin(); it != m_apps.cend(); ++it)
            fetchLatestRelease(it.key());
        if (m_apps.isEmpty())
            m_statusLabel->setText(tr("No apps are listed in the catalog."));
        else
            m_statusLabel->setText(tr("Checking GitHub releases for %1 app(s)…").arg(m_apps.size()));
        return;
    }

    auto appIt = m_apps.find(appId);
    if (appIt == m_apps.end())
        return;

    AppEntry& app = appIt.value();
    if (requestType == QStringLiteral("release")) {
        if (networkError != QNetworkReply::NoError) {
            app.releaseError = networkError == QNetworkReply::ContentNotFoundError
                ? tr("No public release has been published yet.")
                : tr("Could not check GitHub: %1").arg(errorText);
        } else {
            QJsonParseError parseError;
            const QJsonDocument document = QJsonDocument::fromJson(data, &parseError);
            const QJsonObject release = document.object();
            app.latestVersion = release.value(QStringLiteral("tag_name")).toString();
            app.latestVersion.remove(QRegularExpression(QStringLiteral("^(manager-)?v"),
                                                         QRegularExpression::CaseInsensitiveOption));
            app.releaseError.clear();
            app.downloadUrl.clear();
            app.sha256.clear();

            const QJsonArray assets = release.value(QStringLiteral("assets")).toArray();
            for (const QJsonValue& value : assets) {
                const QJsonObject asset = value.toObject();
                if (asset.value(QStringLiteral("name")).toString() != app.installerAsset)
                    continue;
                app.downloadUrl = asset.value(QStringLiteral("browser_download_url")).toString();
                const QString digest = asset.value(QStringLiteral("digest")).toString();
                if (digest.startsWith(QStringLiteral("sha256:"), Qt::CaseInsensitive))
                    app.sha256 = digest.mid(7).toLower();
                break;
            }

            if (app.latestVersion.isEmpty())
                app.releaseError = tr("GitHub returned release information in an unknown format.");
            else if (app.downloadUrl.isEmpty())
                app.releaseError = tr("Release %1 does not contain %2.").arg(app.latestVersion, app.installerAsset);
            else if (app.sha256.isEmpty())
                app.releaseError = tr("The installer does not have a SHA-256 checksum yet.");
        }
        updateAppCard(app);
        m_statusLabel->setText(tr("Finished checking %1.").arg(app.name));
        return;
    }

    if (requestType == QStringLiteral("download")) {
        if (networkError != QNetworkReply::NoError) {
            const bool tooLarge = reply->property("tooLarge").toBool();
            if (tooLarge)
                app.versionLabel->setText(tr("Installer is larger than the allowed download limit."));
            else
                app.versionLabel->setText(tr("Download failed: %1").arg(errorText));
            app.progressBar->hide();
            updateAppCard(app);
            m_statusLabel->setText(tr("Could not download %1.").arg(app.name));
            return;
        }

        if (data.isEmpty() || data.size() > kMaximumInstallerSize ||
            !data.startsWith("MZ")) {
            app.versionLabel->setText(tr("Downloaded file is not a valid Windows installer."));
            app.progressBar->hide();
            updateAppCard(app);
            m_statusLabel->setText(tr("Installer validation failed."));
            return;
        }

        const QByteArray actualHash = QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
        if (actualHash.compare(app.sha256.toLatin1(), Qt::CaseInsensitive) != 0) {
            app.versionLabel->setText(tr("The installer checksum does not match; it was not launched."));
            app.progressBar->hide();
            updateAppCard(app);
            m_statusLabel->setText(tr("Installer integrity verification failed."));
            return;
        }

        const QString downloadDirectory = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
            + QStringLiteral("/downloads");
        if (!QDir().mkpath(downloadDirectory)) {
            app.versionLabel->setText(tr("Could not create the download folder."));
            app.progressBar->hide();
            updateAppCard(app);
            return;
        }

        const QString installerPath = QDir(downloadDirectory).filePath(
            QStringLiteral("%1-%2-Setup.exe").arg(app.id, app.latestVersion));
        QSaveFile installer(installerPath);
        if (!installer.open(QIODevice::WriteOnly) || installer.write(data) != data.size() || !installer.commit()) {
            app.versionLabel->setText(tr("Could not save the installer to disk."));
            app.progressBar->hide();
            updateAppCard(app);
            return;
        }

        const bool started = QProcess::startDetached(installerPath, {}, downloadDirectory);
        app.progressBar->hide();
        if (!started) {
            app.versionLabel->setText(tr("Installer downloaded, but Windows could not start it."));
            updateAppCard(app);
            m_statusLabel->setText(tr("Installer saved at %1").arg(installerPath));
            return;
        }

        app.versionLabel->setText(tr("Installer started. Follow its setup steps to finish."));
        app.installButton->setEnabled(true);
        m_statusLabel->setText(tr("The %1 installer is running.").arg(app.name));
    }
}

QString MainWindow::installedVersion(const AppEntry& app) const
{
    if (app.installedRegistryKey.isEmpty())
        return {};
    QSettings settings(app.installedRegistryKey, QSettings::NativeFormat);
    return settings.value(QStringLiteral("DisplayVersion")).toString();
}

QString MainWindow::normalizedVersion(QString version)
{
    version.remove(QRegularExpression(QStringLiteral("^(manager-)?v"),
                                     QRegularExpression::CaseInsensitiveOption));
    return version;
}
