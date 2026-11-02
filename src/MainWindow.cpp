#include "MainWindow.h"

#include "CanvasWidget.h"
#include "Document.h"
#include "Layer.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QButtonGroup>
#include <QComboBox>
#include <QColorDialog>
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDockWidget>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QInputDialog>
#include <QAbstractButton>
#include <QHash>
#include <QDataStream>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QImageWriter>
#include <QImageReader>
#include <QMessageBox>
#include <QSaveFile>
#include <QLineEdit>
#include <QCheckBox>
#include <QSettings>
#include <QTabWidget>
#include <QListWidget>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QPainter>
#include <QSlider>
#include <QSignalBlocker>
#include <QShortcut>
#include <QKeySequence>
#include <QSpinBox>
#include <QStatusBar>
#include <QStringList>
#include <QToolBar>
#include <QToolButton>
#include <QTransform>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>
#include <cmath>


MainWindow::MainWindow(
    QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(
        "Feather Photo"
    );

    resize(
        1400,
        850
    );

    QSettings appSettings("Eagle Studio", "Feather Photo");
    m_themeName = appSettings.value("appearance/theme", "Midnight").toString();
    m_language = appSettings.value("language", "en").toString();

    // ========================================================
    // GLOBAL DARK UI THEME
    // ========================================================
    m_baseStyleSheet = QString::fromUtf8(R"(
        QMainWindow, QWidget {
            background-color: #15171a;
            color: #e7eaf0;
            font-family: "Segoe UI";
            font-size: 13px;
        }

        QMenuBar {
            background-color: #1b1e22;
            color: #d9dde5;
            border-bottom: 1px solid #292d33;
            padding: 4px 6px;
        }
        QMenuBar::item {
            background: transparent;
            padding: 6px 10px;
            border-radius: 6px;
        }
        QMenuBar::item:selected {
            background-color: #2a2f36;
            color: #ffffff;
        }
        QMenu {
            background-color: #202329;
            color: #e7eaf0;
            border: 1px solid #343941;
            padding: 6px;
        }
        QMenu::item {
            padding: 7px 28px 7px 12px;
            border-radius: 5px;
        }
        QMenu::item:selected {
            background-color: #2f6fed;
            color: #ffffff;
        }
        QMenu::separator {
            height: 1px;
            background: #343941;
            margin: 5px 8px;
        }

        QToolBar {
            background-color: #1b1e22;
            border: none;
            border-bottom: 1px solid #292d33;
            spacing: 5px;
            padding: 6px 8px;
        }
        QToolButton {
            background: transparent;
            color: #cdd2db;
            border: 1px solid transparent;
            border-radius: 6px;
            padding: 6px 10px;
        }
        QToolButton:hover {
            background-color: #292e35;
            color: #ffffff;
        }
        QToolButton:pressed {
            background-color: #343a43;
        }

        QDockWidget {
            background-color: #191c20;
            color: #e7eaf0;
            titlebar-close-icon: none;
            titlebar-normal-icon: none;
        }
        QDockWidget::title {
            background-color: #1f2328;
            color: #e8ebf0;
            text-align: left;
            padding: 9px 10px;
            border-bottom: 1px solid #30353c;
            font-weight: 600;
        }

        QGroupBox {
            background-color: #1c1f24;
            border: 1px solid #30353c;
            border-radius: 9px;
            margin-top: 12px;
            padding: 14px 10px 10px 10px;
            font-weight: 600;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
            color: #aeb6c3;
            background-color: #1c1f24;
        }

        QLabel {
            color: #aeb6c3;
        }

        QPushButton {
            background-color: #24282e;
            color: #e4e7ec;
            border: 1px solid #363b43;
            border-radius: 7px;
            padding: 7px 12px;
            min-height: 18px;
        }
        QPushButton:hover {
            background-color: #2b3037;
            border-color: #4b535e;
            color: #ffffff;
        }
        QPushButton:pressed {
            background-color: #20242a;
        }
        QPushButton:checked {
            background-color: #2f6fed;
            border-color: #5b8df5;
            color: #ffffff;
        }

        QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox {
            background-color: #15181c;
            color: #e8ebf0;
            border: 1px solid #363b43;
            border-radius: 6px;
            padding: 6px 8px;
            selection-background-color: #2f6fed;
            selection-color: #ffffff;
        }
        QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus {
            border-color: #4b7ff0;
        }
        QComboBox::drop-down {
            border: none;
            width: 24px;
        }
        QComboBox QAbstractItemView {
            background-color: #202329;
            color: #e7eaf0;
            border: 1px solid #343941;
            selection-background-color: #2f6fed;
        }

        QSlider::groove:horizontal {
            height: 4px;
            background: #343941;
            border-radius: 2px;
        }
        QSlider::handle:horizontal {
            width: 14px;
            height: 14px;
            margin: -5px 0;
            background: #dce2ea;
            border: 1px solid #8f98a6;
            border-radius: 7px;
        }
        QSlider::handle:horizontal:hover {
            background: #ffffff;
            border-color: #5b8df5;
        }
        QSlider::sub-page:horizontal {
            background: #2f6fed;
            border-radius: 2px;
        }

        QListWidget {
            background-color: #15181c;
            color: #dfe3ea;
            border: 1px solid #30353c;
            border-radius: 7px;
            outline: none;
            padding: 4px;
        }
        QListWidget::item {
            padding: 8px;
            border-radius: 5px;
        }
        QListWidget::item:hover {
            background-color: #252a31;
        }
        QListWidget::item:selected {
            background-color: #2f6fed;
            color: #ffffff;
        }

        QStatusBar {
            background-color: #1b1e22;
            color: #9ea7b5;
            border-top: 1px solid #292d33;
        }
        QScrollBar:vertical {
            background: #181b1f;
            width: 10px;
            margin: 2px;
        }
        QScrollBar::handle:vertical {
            background: #3a4048;
            min-height: 30px;
            border-radius: 5px;
        }
        QScrollBar::handle:vertical:hover {
            background: #4a525d;
        }
        QScrollBar::add-line, QScrollBar::sub-line {
            height: 0px;
        }
        QScrollBar:horizontal {
            background: #181b1f;
            height: 10px;
        }
        QScrollBar::handle:horizontal {
            background: #3a4048;
            min-width: 30px;
            border-radius: 5px;
        }
        QDialog {
            background-color: #1b1e22;
        }
    )");

    createMenus();

    createToolBar();

    // ToolPanel connects CanvasWidget::toolChanged, so the canvas must
    // exist before the panel establishes that connection.
    createCanvas();

    connect(m_canvas, &CanvasWidget::editAboutToChange, this, &MainWindow::pushUndoState);

    m_canvasRefreshTimer.setSingleShot(true);
    m_canvasRefreshTimer.setInterval(33);
    connect(&m_canvasRefreshTimer, &QTimer::timeout, this, [this]()
        {
            const bool fullRefresh = m_canvasNeedsFullRefresh || m_pendingCanvasDirtyRect.isNull();
            const QRect dirtyRect = m_pendingCanvasDirtyRect;
            m_canvasNeedsFullRefresh = false;
            m_pendingCanvasDirtyRect = QRect();
            refreshCanvasImage(fullRefresh ? QRect() : dirtyRect);
        });
    connect(m_canvas, &CanvasWidget::layerContentChanged, this, [this](const QRect& dirtyRect)
        {
            if (dirtyRect.isNull())
                m_canvasNeedsFullRefresh = true;
            else if (!m_canvasNeedsFullRefresh)
                m_pendingCanvasDirtyRect = m_pendingCanvasDirtyRect.isNull()
                    ? dirtyRect
                    : m_pendingCanvasDirtyRect.united(dirtyRect);
            if (!m_canvasRefreshTimer.isActive())
                m_canvasRefreshTimer.start();
        });

    connect(m_canvas, &CanvasWidget::cropRequested, this, [this](const QRect& crop)
        {
            if (!m_document || crop.width() <= 1 || crop.height() <= 1)
                return;
            pushUndoState();
            for (Layer* layer : m_document->layers())
                layer->image() = layer->image().copy(crop);
            m_document->setSize(crop.width(), crop.height());
            if (Layer* active = m_document->activeLayer())
            {
                m_canvas->setActiveLayer(active);
                m_canvas->deselect();
                refreshLayerPanel();
                refreshCanvasImage();
                m_canvas->fitToWindow();
            }
        });

    createToolPanel();

    createLayersPanel();

    // Window-scoped shortcuts work even when focus is on a tool button or panel.
    auto* pickerShortcut = new QShortcut(QKeySequence(Qt::Key_P), this);
    pickerShortcut->setContext(Qt::WindowShortcut);
    connect(pickerShortcut, &QShortcut::activated, this, [this]()
        {
            m_canvas->setPickerEnabled(!m_canvas->pickerEnabled());
        });

    auto* brushShortcut = new QShortcut(QKeySequence(Qt::Key_B), this);
    brushShortcut->setContext(Qt::WindowShortcut);
    connect(brushShortcut, &QShortcut::activated, this, [this]()
        {
            m_canvas->setActiveTool("Brush");
        });

    const QList<QPair<int, QString>> toolShortcuts = {
        { Qt::Key_N, "Pencil" }, { Qt::Key_L, "Line" },
        { Qt::Key_R, "Rectangle" }, { Qt::Key_O, "Ellipse" },
        { Qt::Key_G, "Fill" }, { Qt::Key_V, "Move" }, { Qt::Key_C, "Crop" },
        { Qt::Key_M, "Select" }
    };
    for (const auto& shortcut : toolShortcuts)
    {
        auto* action = new QShortcut(QKeySequence(shortcut.first), this);
        action->setContext(Qt::WindowShortcut);
        connect(action, &QShortcut::activated, this, [this, name = shortcut.second]()
            {
                m_canvas->setActiveTool(name);
            });
    }
    auto* eraserShortcut = new QShortcut(QKeySequence(Qt::Key_E), this);
    eraserShortcut->setContext(Qt::WindowShortcut);
    connect(eraserShortcut, &QShortcut::activated, this, [this]()
        {
            m_canvas->setEraserEnabled(!m_canvas->eraserEnabled());
        });

    createBrushPanel();

    createColorPanel();

    connect(
        m_canvas,
        &CanvasWidget::brushColorChanged,
        this,
        [this](const QColor& color)
        {
            m_colorPreviewButton->setStyleSheet(
                QString(
                    "background-color: %1;"
                    "border: 1px solid #555555;"
                ).arg(
                    color.name()
                )
            );
        }
    );

    createStatusBar();

    applyTheme(m_themeName);
    translateUi();
}


MainWindow::~MainWindow()
{
    delete m_document;
}


// ============================================================
// MENUS
// ============================================================

void MainWindow::createMenus()
{
    QMenu* fileMenu =
        menuBar()->addMenu(
            "File"
        );

    m_newProjectAction =
        new QAction(
            "New Project",
            this
        );

    m_openProjectAction =
        new QAction(
            "Open Project",
            this
        );

    m_saveProjectAction =
        new QAction(
            "Save Project",
            this
        );

    m_exportAction =
        new QAction(
            "Export",
            this
        );

    QAction* exitAction =
        new QAction(
            "Exit",
            this
        );

    fileMenu->addAction(
        m_newProjectAction
    );

    fileMenu->addAction(
        m_openProjectAction
    );

    fileMenu->addSeparator();

    fileMenu->addAction(
        m_saveProjectAction
    );

    fileMenu->addAction(
        m_exportAction
    );

    fileMenu->addSeparator();

    fileMenu->addAction(
        exitAction
    );

    connect(
        m_newProjectAction,
        &QAction::triggered,
        this,
        &MainWindow::newProject
    );

    connect(m_openProjectAction, &QAction::triggered, this, &MainWindow::openProject);
    connect(m_saveProjectAction, &QAction::triggered, this, &MainWindow::saveProject);
    connect(m_exportAction, &QAction::triggered, this, &MainWindow::exportImage);

    m_newProjectAction->setShortcut(QKeySequence::New);
    m_openProjectAction->setShortcut(QKeySequence::Open);
    m_saveProjectAction->setShortcut(QKeySequence::Save);
    m_exportAction->setShortcut(QKeySequence("Ctrl+Shift+E"));

    connect(
        exitAction,
        &QAction::triggered,
        qApp,
        &QApplication::quit
    );


    // EDIT

    QMenu* editMenu =
        menuBar()->addMenu(
            "Edit"
        );

    m_undoAction = editMenu->addAction("Undo");
    m_undoAction->setShortcut(QKeySequence("Ctrl+Z"));
    m_undoAction->setShortcutContext(Qt::WindowShortcut);
    connect(m_undoAction, &QAction::triggered, this, &MainWindow::undo);

    m_redoAction = editMenu->addAction("Redo");
    m_redoAction->setShortcut(QKeySequence("Ctrl+Shift+Z"));
    m_redoAction->setShortcutContext(Qt::WindowShortcut);
    connect(m_redoAction, &QAction::triggered, this, &MainWindow::redo);

    editMenu->addSeparator();

    QAction* cutAction = editMenu->addAction("Cut");
    cutAction->setShortcut(QKeySequence::Cut);
    cutAction->setShortcutContext(Qt::WindowShortcut);
    connect(cutAction, &QAction::triggered, this, &MainWindow::cutSelection);

    QAction* copyAction = editMenu->addAction("Copy");
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setShortcutContext(Qt::WindowShortcut);
    connect(copyAction, &QAction::triggered, this, &MainWindow::copySelection);

    QAction* pasteAction = editMenu->addAction("Paste");
    pasteAction->setShortcut(QKeySequence::Paste);
    pasteAction->setShortcutContext(Qt::WindowShortcut);
    connect(pasteAction, &QAction::triggered, this, &MainWindow::pasteImage);
    updateHistoryActions();


    // IMAGE

    QMenu* imageMenu =
        menuBar()->addMenu(
            "Image"
        );

    QAction* imageSizeAction = imageMenu->addAction("Image Size");
    QAction* canvasSizeAction = imageMenu->addAction("Canvas Size");
    QAction* rotateAction = imageMenu->addAction("Rotate");
    QAction* flipHorizontalAction = imageMenu->addAction("Flip Horizontal");
    QAction* flipVerticalAction = imageMenu->addAction("Flip Vertical");
    connect(imageSizeAction, &QAction::triggered, this, &MainWindow::showImageSizeDialog);
    connect(canvasSizeAction, &QAction::triggered, this, &MainWindow::showCanvasSizeDialog);
    connect(rotateAction, &QAction::triggered, this, &MainWindow::showRotateDialog);
    connect(flipHorizontalAction, &QAction::triggered, this, [this]() { flipDocument(true); });
    connect(flipVerticalAction, &QAction::triggered, this, [this]() { flipDocument(false); });


    // LAYER

    QMenu* layerMenu =
        menuBar()->addMenu(
            "Layer"
        );

    QAction* newLayerAction = layerMenu->addAction("New Layer");
    QAction* duplicateLayerAction = layerMenu->addAction("Duplicate Layer");
    QAction* deleteLayerAction = layerMenu->addAction("Delete Layer");
    layerMenu->addSeparator();
    QAction* mergeDownAction = layerMenu->addAction("Merge Down");

    connect(newLayerAction, &QAction::triggered, this, &MainWindow::addLayer);
    connect(duplicateLayerAction, &QAction::triggered, this, &MainWindow::duplicateActiveLayer);
    connect(deleteLayerAction, &QAction::triggered, this, &MainWindow::deleteActiveLayer);
    connect(mergeDownAction, &QAction::triggered, this, &MainWindow::mergeActiveLayerDown);


    // SELECT

    QMenu* selectMenu =
        menuBar()->addMenu(
            "Select"
        );

    QAction* selectAllAction = selectMenu->addAction("Select All");
    QAction* deselectAction = selectMenu->addAction("Deselect");
    QAction* invertSelectionAction = selectMenu->addAction("Invert Selection");
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    deselectAction->setShortcut(QKeySequence("Ctrl+D"));
    invertSelectionAction->setShortcut(QKeySequence("Ctrl+Shift+I"));
    connect(selectAllAction, &QAction::triggered, this, [this]() { if (m_canvas) m_canvas->selectAll(); });
    connect(deselectAction, &QAction::triggered, this, [this]() { if (m_canvas) m_canvas->deselect(); });
    connect(invertSelectionAction, &QAction::triggered, this, [this]() { if (m_canvas) m_canvas->invertSelection(); });


    // FILTER

    QMenu* filterMenu =
        menuBar()->addMenu(
            "Filter"
        );

    QAction* blurAction = filterMenu->addAction("Blur");
    QAction* sharpenAction = filterMenu->addAction("Sharpen");
    QAction* grayscaleAction = filterMenu->addAction("Grayscale");
    QAction* invertAction = filterMenu->addAction("Invert");
    connect(blurAction, &QAction::triggered, this, [this]() { applyImageFilter("Blur"); });
    connect(sharpenAction, &QAction::triggered, this, [this]() { applyImageFilter("Sharpen"); });
    connect(grayscaleAction, &QAction::triggered, this, [this]() { applyImageFilter("Grayscale"); });
    connect(invertAction, &QAction::triggered, this, [this]() { applyImageFilter("Invert"); });


    // VIEW

    QMenu* viewMenu =
        menuBar()->addMenu(
            "View"
        );

    QAction* zoom100Action =
        viewMenu->addAction(
            "100%"
        );

    QAction* fitAction =
        viewMenu->addAction(
            "Fit Canvas"
        );

    connect(
        zoom100Action,
        &QAction::triggered,
        this,
        [this]()
        {
            if (m_canvas)
            {
                m_canvas->resetZoom();
            }
        }
    );

    connect(
        fitAction,
        &QAction::triggered,
        this,
        [this]()
        {
            if (m_canvas)
            {
                m_canvas->fitToWindow();
            }
        }
    );


    QMenu* settingsMenu = menuBar()->addMenu("Settings");
    QAction* preferencesAction = settingsMenu->addAction("Preferences...");
    connect(preferencesAction, &QAction::triggered, this, &MainWindow::openSettingsDialog);
}

void MainWindow::openSettingsDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle(translated("Settings"));
    dialog.resize(480, 360);
    auto* layout = new QVBoxLayout(&dialog);
    auto* tabs = new QTabWidget(&dialog);

    auto* appearancePage = new QWidget(tabs);
    auto* appearanceLayout = new QFormLayout(appearancePage);
    auto* themeCombo = new QComboBox(appearancePage);
    const QStringList themes = { "Midnight", "Light", "Ocean", "Forest", "Violet" };
    themeCombo->addItems(themes);
    themeCombo->setCurrentText(m_themeName);
    appearanceLayout->addRow(translated("Theme"), themeCombo);
    tabs->addTab(appearancePage, translated("Appearance"));

    auto* languagePage = new QWidget(tabs);
    auto* languageLayout = new QFormLayout(languagePage);
    auto* languageCombo = new QComboBox(languagePage);
    languageCombo->addItem("English", "en");
    languageCombo->addItem("Polski", "pl");
    languageCombo->setCurrentIndex(languageCombo->findData(m_language));
    languageLayout->addRow(translated("Language"), languageCombo);
    tabs->addTab(languagePage, translated("Language"));

    auto* aboutPage = new QWidget(tabs);
    auto* aboutLayout = new QVBoxLayout(aboutPage);
    auto* aboutLabel = new QLabel(
        translated("Feather Photo\nVersion 0.1.0\nA desktop drawing and photo editing application."),
        aboutPage);
    aboutLabel->setAlignment(Qt::AlignCenter);
    aboutLayout->addStretch();
    aboutLayout->addWidget(aboutLabel);
    aboutLayout->addStretch();
    tabs->addTab(aboutPage, translated("About"));

    layout->addWidget(tabs);
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttons->button(QDialogButtonBox::Ok)->setText(translated("OK"));
    buttons->button(QDialogButtonBox::Cancel)->setText(translated("Cancel"));
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted)
        return;

    m_themeName = themeCombo->currentText();
    m_language = languageCombo->currentData().toString();
    QSettings settings("Eagle Studio", "Feather Photo");
    settings.setValue("appearance/theme", m_themeName);
    settings.setValue("language", m_language);
    applyTheme(m_themeName);
    translateUi();
}

void MainWindow::applyTheme(const QString& themeName)
{
    struct ThemeColors
    {
        QString window, panel, elevated, input, border, text, secondary, accent, accentLight, hover, line, scrollbar;
    };
    static const QHash<QString, ThemeColors> themes = {
        { "Midnight", { "#15171a", "#1b1e22", "#202329", "#15181c", "#30353c", "#e7eaf0", "#aeb6c3", "#2f6fed", "#5b8df5", "#2a2f36", "#292d33", "#3a4048" } },
        { "Light",    { "#f3f5f7", "#e6e9ed", "#ffffff", "#ffffff", "#c5cbd3", "#202630", "#536171", "#2878d0", "#6ba5e8", "#d9e1ea", "#cbd1d8", "#9aa4b0" } },
        { "Ocean",    { "#101a24", "#172635", "#1c2d3d", "#101d29", "#2b4054", "#e1eff8", "#9bb4c7", "#00a8cc", "#48c6df", "#23394b", "#203343", "#38566d" } },
        { "Forest",   { "#151d19", "#1c2821", "#24332a", "#17221b", "#34463a", "#e3eee6", "#a8b9ac", "#438c5c", "#72b786", "#293a30", "#2b392f", "#405849" } },
        { "Violet",   { "#1c1824", "#241f2f", "#2c2639", "#1a1522", "#3b3348", "#eee8f5", "#b6a9c5", "#9063c7", "#b08be0", "#342c41", "#30283b", "#514660" } }
    };
    const ThemeColors colors = themes.value(themeName, themes.value("Midnight"));
    QHash<QString, QString> replacements = {
        { "#15171a", colors.window }, { "#1b1e22", colors.panel }, { "#202329", colors.elevated },
        { "#15181c", colors.input }, { "#30353c", colors.border }, { "#363b43", colors.border },
        { "#343941", colors.border }, { "#e7eaf0", colors.text }, { "#e8ebf0", colors.text },
        { "#e4e7ec", colors.text }, { "#d9dde5", colors.text }, { "#dfe3ea", colors.text },
        { "#cdd2db", colors.secondary }, { "#aeb6c3", colors.secondary }, { "#9ea7b5", colors.secondary },
        { "#292d33", colors.line }, { "#2f6fed", colors.accent }, { "#5b8df5", colors.accentLight },
        { "#4b7ff0", colors.accentLight }, { "#2a2f36", colors.hover }, { "#292e35", colors.hover },
        { "#343a43", colors.border }, { "#24282e", colors.elevated }, { "#20242a", colors.elevated },
        { "#252a31", colors.hover }, { "#48505b", colors.border }, { "#3a4048", colors.scrollbar },
        { "#4a525d", colors.secondary }, { "#bfc6d1", colors.secondary }
    };
    auto recolor = [&replacements](QString style)
        {
            for (auto it = replacements.cbegin(); it != replacements.cend(); ++it)
                style.replace(it.key(), it.value(), Qt::CaseInsensitive);
            return style;
        };

    setStyleSheet(recolor(m_baseStyleSheet));
    const auto widgets = findChildren<QWidget*>();
    for (QWidget* widget : widgets)
    {
        if (widget == m_colorPreviewButton)
            continue;
        if (!widget->property("featherBaseStyle").isValid())
            widget->setProperty("featherBaseStyle", widget->styleSheet());
        const QString baseStyle = widget->property("featherBaseStyle").toString();
        if (!baseStyle.isEmpty())
            widget->setStyleSheet(recolor(baseStyle));
    }
    m_themeName = themeName;
}

QString MainWindow::translated(const QString& englishText) const
{
    if (m_language != "pl")
        return englishText;
    static const QHash<QString, QString> polish = {
        { "File", "Plik" }, { "Edit", "Edycja" }, { "Image", "Obraz" }, { "Layer", "Warstwa" },
        { "Select", "Zaznaczenie" }, { "Filter", "Filtr" }, { "View", "Widok" }, { "Settings", "Ustawienia" },
        { "Preferences...", "Preferencje..." }, { "New Project", "Nowy projekt" }, { "Open Project", "Otwórz projekt" },
        { "Save Project", "Zapisz projekt" }, { "Export", "Eksportuj" }, { "Exit", "Wyjdź" },
        { "Feather Photo Project", "Projekt Feather Photo" }, { "Images", "Obrazy" },
        { "Project opened", "Projekt otwarty" }, { "Image opened", "Obraz otwarty" },
        { "Project saved", "Projekt zapisany" }, { "Image exported", "Obraz wyeksportowany" },
        { "Create or open a project first", "Najpierw utwórz lub otwórz projekt" },
        { "Export Image", "Eksportuj obraz" },
        { "Quality", "Jakość" }, { "Preserve transparency", "Zachowaj przezroczystość" },
        { "Browse...", "Przeglądaj..." }, { "Choose where to save the image", "Wybierz miejsce zapisu obrazu" },
        { "Save to", "Zapisz do" }, { "Format", "Format" },
        { "Qt does not provide any supported image writer formats.", "Qt nie udostępnia obsługiwanych formatów zapisu obrazów." },
        { "The image file could not be created.", "Nie udało się utworzyć pliku obrazu." },
        { "Undo", "Cofnij" }, { "Redo", "Ponów" }, { "Cut", "Wytnij" }, { "Copy", "Kopiuj" }, { "Paste", "Wklej" },
        { "New", "Nowy" }, { "Open", "Otwórz" }, { "Save", "Zapisz" }, { "Brush", "Pędzel" },
        { "Eraser", "Gumka" }, { "Pencil", "Ołówek" }, { "Line", "Linia" }, { "Rectangle", "Prostokąt" },
        { "Rect", "Prostokąt" }, { "Ellipse", "Elipsa" }, { "Fill", "Wypełnij" }, { "Picker", "Pobierak" },
        { "Move", "Przesuń" }, { "Crop", "Kadruj" }, { "Select All", "Zaznacz wszystko" },
        { "Blur", "Rozmycie" }, { "Sharpen", "Wyostrzenie" }, { "Grayscale", "Skala szarości" }, { "Invert", "Odwróć" },
        { "Deselect", "Odznacz" }, { "Invert Selection", "Odwróć zaznaczenie" }, { "Image Size", "Rozmiar obrazu" },
        { "Canvas Size", "Rozmiar płótna" }, { "Rotate", "Obróć" }, { "Flip Horizontal", "Odbij poziomo" },
        { "Flip Vertical", "Odbij pionowo" }, { "New Layer", "Nowa warstwa" }, { "Duplicate Layer", "Duplikuj warstwę" },
        { "Delete Layer", "Usuń warstwę" }, { "Merge Down", "Scal w dół" }, { "100%", "100%" },
        { "Fit Canvas", "Dopasuj płótno" }, { "Tools", "Narzędzia" }, { "Layers", "Warstwy" },
        { "Opacity", "Krycie" }, { "Color", "Kolor" }, { "Current Color", "Bieżący kolor" },
        { "Size", "Rozmiar" }, { "Hardness", "Twardość" }, { "Flow", "Przepływ" }, { "Color...", "Kolor..." },
        { "Appearance", "Wygląd" }, { "Language", "Język" }, { "About", "O programie" },
        { "Theme", "Motyw" }, { "OK", "OK" }, { "Cancel", "Anuluj" },
        { "Feather Photo\nVersion 0.1.0\nA desktop drawing and photo editing application.", "Feather Photo\nWersja 0.1.0\nAplikacja do rysowania i edycji zdjęć." }
    };
    return polish.value(englishText, englishText);
}

void MainWindow::translateUi()
{
    const auto actions = findChildren<QAction*>();
    for (QAction* action : actions)
    {
        if (!action->property("featherOriginalText").isValid())
            action->setProperty("featherOriginalText", action->text());
        action->setText(translated(action->property("featherOriginalText").toString()));
    }

    const auto widgets = findChildren<QWidget*>();
    for (QWidget* widget : widgets)
    {
        if (auto* toolButton = qobject_cast<QToolButton*>(widget); toolButton && toolButton->defaultAction())
            continue;
        if (!widget->property("featherOriginalText").isValid())
        {
            QString text;
            if (auto* button = qobject_cast<QAbstractButton*>(widget)) text = button->text();
            else if (auto* label = qobject_cast<QLabel*>(widget)) text = label->text();
            else if (auto* group = qobject_cast<QGroupBox*>(widget)) text = group->title();
            else if (auto* dock = qobject_cast<QDockWidget*>(widget)) text = dock->windowTitle();
            if (!text.isNull()) widget->setProperty("featherOriginalText", text);
        }
        const QString original = widget->property("featherOriginalText").toString();
        if (original.isNull()) continue;
        const QString text = translated(original);
        if (auto* button = qobject_cast<QAbstractButton*>(widget)) button->setText(text);
        else if (auto* label = qobject_cast<QLabel*>(widget)) label->setText(text);
        else if (auto* group = qobject_cast<QGroupBox*>(widget)) group->setTitle(text);
        else if (auto* dock = qobject_cast<QDockWidget*>(widget)) dock->setWindowTitle(text);
    }
    for (QTabWidget* tabs : findChildren<QTabWidget*>())
    {
        for (int index = 0; index < tabs->count(); ++index)
        {
            const QByteArray propertyName = QByteArray("featherOriginalTabText_") + QByteArray::number(index);
            if (!tabs->property(propertyName.constData()).isValid())
                tabs->setProperty(propertyName.constData(), tabs->tabText(index));
            tabs->setTabText(index, translated(tabs->property(propertyName.constData()).toString()));
        }
    }
    setWindowTitle("Feather Photo");
}


// ============================================================
// TOOLBAR
// ============================================================

void MainWindow::createToolBar()
{
    QToolBar* toolbar =
        addToolBar(
            "Main Toolbar"
        );

    toolbar->setMovable(
        false
    );

    QAction* newToolbarAction = toolbar->addAction("New");
    QAction* openToolbarAction = toolbar->addAction("Open");
    QAction* saveToolbarAction = toolbar->addAction("Save");
    connect(newToolbarAction, &QAction::triggered, this, &MainWindow::newProject);
    connect(openToolbarAction, &QAction::triggered, this, &MainWindow::openProject);
    connect(saveToolbarAction, &QAction::triggered, this, &MainWindow::saveProject);
}


// ============================================================
// TOOL PANEL
// ============================================================

void MainWindow::createToolPanel()
{
    QWidget* panel =
        new QWidget(
            this
        );

    panel->setMinimumWidth(
        76
    );

    panel->setMaximumWidth(
        90
    );

    QVBoxLayout* layout =
        new QVBoxLayout(
            panel
        );

    layout->setContentsMargins(
        6,
        8,
        6,
        8
    );

    layout->setSpacing(
        5
    );

    m_toolButtonGroup =
        new QButtonGroup(
            this
        );

    m_toolButtonGroup->setExclusive(
        true
    );

    const QString buttonStyle =
        R"(
            QPushButton {
                background-color: #20242a;
                color: #bfc6d1;
                border: 1px solid #30353c;
                border-radius: 8px;
                min-height: 42px;
                max-height: 42px;
                font-size: 11px;
                font-weight: 600;
                padding: 2px;
            }

            QPushButton:hover {
                background-color: #2a2f36;
                border: 1px solid #48505b;
                color: #ffffff;
            }

            QPushButton:checked {
                background-color: #2f6fed;
                border: 1px solid #5b8df5;
                color: #ffffff;
                font-weight: 700;
            }

            QPushButton:pressed {
                background-color: #2563c7;
            }
        )";

    auto createToolButton =
        [this, &buttonStyle](
            const QString& text,
            QPushButton*& member
            )
        {
            member =
                new QPushButton(
                    text,
                    this
                );

            member->setCheckable(
                true
            );

            member->setCursor(
                Qt::PointingHandCursor
            );

            const QString shortcut =
                text == "Brush" ? "B" :
                text == "Eraser" ? "E" :
                text == "Picker" ? "P" :
                text == "Pencil" ? "N" :
                text == "Line" ? "L" :
                text == "Rect" ? "R" :
                text == "Ellipse" ? "O" :
                text == "Fill" ? "G" :
                text == "Move" ? "V" :
                text == "Crop" ? "C" :
                text == "Select" ? "M" : QString();

            member->setToolTip(
                shortcut.isEmpty()
                ? text
                : QString("%1 (%2)").arg(text, shortcut)
            );

            member->setStyleSheet(
                buttonStyle
            );

            m_toolButtonGroup->addButton(
                member
            );

            return member;
        };

    m_brushToolButton =
        createToolButton(
            "Brush",
            m_brushToolButton
        );

    m_eraserToolButton =
        createToolButton(
            "Eraser",
            m_eraserToolButton
        );

    m_pencilToolButton =
        createToolButton(
            "Pencil",
            m_pencilToolButton
        );

    m_lineToolButton =
        createToolButton(
            "Line",
            m_lineToolButton
        );

    m_rectToolButton =
        createToolButton(
            "Rect",
            m_rectToolButton
        );

    m_ellipseToolButton =
        createToolButton(
            "Ellipse",
            m_ellipseToolButton
        );

    m_fillToolButton =
        createToolButton(
            "Fill",
            m_fillToolButton
        );

    m_pickerToolButton =
        createToolButton(
            "Picker",
            m_pickerToolButton
        );

    m_selectToolButton =
        createToolButton(
            "Select",
            m_selectToolButton
        );

    m_moveToolButton =
        createToolButton(
            "Move",
            m_moveToolButton
        );

    m_cropToolButton =
        createToolButton(
            "Crop",
            m_cropToolButton
        );

    layout->addWidget(m_brushToolButton);
    layout->addWidget(m_eraserToolButton);
    layout->addSpacing(4);
    layout->addWidget(m_pencilToolButton);
    layout->addWidget(m_lineToolButton);
    layout->addWidget(m_rectToolButton);
    layout->addWidget(m_ellipseToolButton);
    layout->addWidget(m_fillToolButton);
    layout->addWidget(m_pickerToolButton);
    layout->addWidget(m_selectToolButton);
    layout->addWidget(m_moveToolButton);
    layout->addWidget(m_cropToolButton);
    layout->addStretch();

    m_brushToolButton->setChecked(true);

    connect(
        m_brushToolButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_canvas->setActiveTool("Brush");
        }
    );

    connect(
        m_eraserToolButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_canvas->setActiveTool("Eraser");
        }
    );

    connect(
        m_pickerToolButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_canvas->setActiveTool("Picker");
        }
    );

    const auto activateTool = [this](const QString& name)
    {
        m_canvas->setActiveTool(name);
    };
    connect(m_pencilToolButton, &QPushButton::clicked, this, [activateTool]() { activateTool("Pencil"); });
    connect(m_lineToolButton, &QPushButton::clicked, this, [activateTool]() { activateTool("Line"); });
    connect(m_rectToolButton, &QPushButton::clicked, this, [activateTool]() { activateTool("Rectangle"); });
    connect(m_ellipseToolButton, &QPushButton::clicked, this, [activateTool]() { activateTool("Ellipse"); });
    connect(m_fillToolButton, &QPushButton::clicked, this, [activateTool]() { activateTool("Fill"); });
    connect(m_moveToolButton, &QPushButton::clicked, this, [activateTool]() { activateTool("Move"); });
    connect(m_cropToolButton, &QPushButton::clicked, this, [activateTool]() { activateTool("Crop"); });
    connect(m_selectToolButton, &QPushButton::clicked, this, [activateTool]() { activateTool("Select"); });
    connect(
        m_canvas,
        &CanvasWidget::toolChanged,
        this,
        [this](const QString& toolName)
        {
            setActiveTool(toolName);
        }
    );

    QDockWidget* dock =
        new QDockWidget(
            "Tools",
            this
        );

    dock->setWidget(panel);

    dock->setFeatures(
        QDockWidget::NoDockWidgetFeatures
    );

    addDockWidget(
        Qt::LeftDockWidgetArea,
        dock
    );
}


void MainWindow::setActiveTool(
    const QString& toolName)
{
    if (!m_toolButtonGroup)
        return;

    QPushButton* activeButton = nullptr;
    QString statusMessage;

    if (toolName == "Brush")
    {
        activeButton = m_brushToolButton;
        statusMessage = "Tool: Brush";
    }
    else if (toolName == "Eraser")
    {
        activeButton = m_eraserToolButton;
        statusMessage = "Tool: Eraser";
    }
    else if (toolName == "Picker")
    {
        activeButton = m_pickerToolButton;
        statusMessage = "Tool: Picker";
    }
    else if (toolName == "Pencil")
    {
        activeButton = m_pencilToolButton;
        statusMessage = "Tool: Pencil (N)";
    }
    else if (toolName == "Line")
    {
        activeButton = m_lineToolButton;
        statusMessage = "Tool: Line (L)";
    }
    else if (toolName == "Rectangle")
    {
        activeButton = m_rectToolButton;
        statusMessage = "Tool: Rectangle (R)";
    }
    else if (toolName == "Ellipse")
    {
        activeButton = m_ellipseToolButton;
        statusMessage = "Tool: Ellipse (O)";
    }
    else if (toolName == "Fill")
    {
        activeButton = m_fillToolButton;
        statusMessage = "Tool: Fill (G)";
    }
    else if (toolName == "Move")
    {
        activeButton = m_moveToolButton;
        statusMessage = "Tool: Move (V)";
    }
    else if (toolName == "Crop")
    {
        activeButton = m_cropToolButton;
        statusMessage = "Tool: Crop (C)";
    }
    else if (toolName == "Select")
    {
        activeButton = m_selectToolButton;
        statusMessage = "Tool: Rectangular Select (M)";
    }

    if (!activeButton)
        return;

    // QButtonGroup zarządza wyłącznym zaznaczeniem. Ustawienie nowego
    // przycisku na checked automatycznie odznacza poprzedni.
    // Ręczne setChecked(false) przy grupie exclusive może być ignorowane.
    activeButton->setChecked(true);
    statusBar()->showMessage(statusMessage);
}

// ============================================================
// BRUSH PANEL
// ============================================================

void MainWindow::createBrushPanel()
{
    QWidget* panel =
        new QWidget(
            this
        );

    panel->setMinimumWidth(
        300
    );

    QVBoxLayout* layout =
        new QVBoxLayout(
            panel
        );

    layout->setContentsMargins(
        10,
        10,
        10,
        10
    );

    QGroupBox* brushGroup =
        new QGroupBox(
            "Brush",
            panel
        );

    QVBoxLayout* brushLayout =
        new QVBoxLayout(
            brushGroup
        );


    // ========================================================
    // SIZE
    // ========================================================

    QLabel* sizeLabel =
        new QLabel(
            "Size",
            brushGroup
        );

    m_brushSizeSlider =
        new QSlider(
            Qt::Horizontal,
            brushGroup
        );

    m_brushSizeSlider->setRange(
        1,
        2000
    );

    m_brushSizeSlider->setValue(
        40
    );

    m_brushSizeSpinBox =
        new QDoubleSpinBox(
            brushGroup
        );

    m_brushSizeSpinBox->setRange(
        1.0,
        2000.0
    );

    m_brushSizeSpinBox->setValue(
        40.0
    );

    m_brushSizeSpinBox->setDecimals(
        0
    );

    QHBoxLayout* sizeLayout =
        new QHBoxLayout();

    sizeLayout->addWidget(
        m_brushSizeSlider
    );

    sizeLayout->addWidget(
        m_brushSizeSpinBox
    );

    brushLayout->addWidget(
        sizeLabel
    );

    brushLayout->addLayout(
        sizeLayout
    );


    // ========================================================
    // OPACITY
    // ========================================================

    QLabel* opacityLabel =
        new QLabel(
            "Opacity",
            brushGroup
        );

    m_brushOpacitySlider =
        new QSlider(
            Qt::Horizontal,
            brushGroup
        );

    m_brushOpacitySlider->setRange(
        0,
        100
    );

    m_brushOpacitySlider->setValue(
        100
    );

    m_brushOpacitySpinBox =
        new QDoubleSpinBox(
            brushGroup
        );

    m_brushOpacitySpinBox->setRange(
        0.0,
        100.0
    );

    m_brushOpacitySpinBox->setValue(
        100.0
    );

    m_brushOpacitySpinBox->setDecimals(
        0
    );

    m_brushOpacitySpinBox->setSuffix(
        "%"
    );

    QHBoxLayout* opacityLayout =
        new QHBoxLayout();

    opacityLayout->addWidget(
        m_brushOpacitySlider
    );

    opacityLayout->addWidget(
        m_brushOpacitySpinBox
    );

    brushLayout->addWidget(
        opacityLabel
    );

    brushLayout->addLayout(
        opacityLayout
    );


    // ========================================================
    // HARDNESS
    // ========================================================

    QLabel* hardnessLabel =
        new QLabel(
            "Hardness",
            brushGroup
        );

    m_brushHardnessSlider =
        new QSlider(
            Qt::Horizontal,
            brushGroup
        );

    m_brushHardnessSlider->setRange(
        0,
        100
    );

    m_brushHardnessSlider->setValue(
        100
    );

    m_brushHardnessSpinBox =
        new QDoubleSpinBox(
            brushGroup
        );

    m_brushHardnessSpinBox->setRange(
        0.0,
        100.0
    );

    m_brushHardnessSpinBox->setValue(
        100.0
    );

    m_brushHardnessSpinBox->setDecimals(
        0
    );

    m_brushHardnessSpinBox->setSuffix(
        "%"
    );

    QHBoxLayout* hardnessLayout =
        new QHBoxLayout();

    hardnessLayout->addWidget(
        m_brushHardnessSlider
    );

    hardnessLayout->addWidget(
        m_brushHardnessSpinBox
    );

    brushLayout->addWidget(
        hardnessLabel
    );

    brushLayout->addLayout(
        hardnessLayout
    );


    // ========================================================
    // FLOW
    // ========================================================

    QLabel* flowLabel =
        new QLabel(
            "Flow",
            brushGroup
        );

    m_brushFlowSlider =
        new QSlider(
            Qt::Horizontal,
            brushGroup
        );

    m_brushFlowSlider->setRange(
        0,
        100
    );

    m_brushFlowSlider->setValue(
        100
    );

    m_brushFlowSpinBox =
        new QDoubleSpinBox(
            brushGroup
        );

    m_brushFlowSpinBox->setRange(
        0.0,
        100.0
    );

    m_brushFlowSpinBox->setValue(
        100.0
    );

    m_brushFlowSpinBox->setDecimals(
        0
    );

    m_brushFlowSpinBox->setSuffix(
        "%"
    );

    QHBoxLayout* flowLayout =
        new QHBoxLayout();

    flowLayout->addWidget(
        m_brushFlowSlider
    );

    flowLayout->addWidget(
        m_brushFlowSpinBox
    );

    brushLayout->addWidget(
        flowLabel
    );

    brushLayout->addLayout(
        flowLayout
    );


    layout->addWidget(
        brushGroup
    );

    layout->addStretch();


    // ========================================================
    // DOCK
    // ========================================================

    QDockWidget* dock =
        new QDockWidget(
            "Brush",
            this
        );

    dock->setWidget(
        panel
    );

    addDockWidget(
        Qt::RightDockWidgetArea,
        dock
    );


    // ========================================================
    // SIZE CONNECTIONS
    // ========================================================

    connect(
        m_brushSizeSlider,
        &QSlider::valueChanged,
        this,
        [this](int value)
        {
            m_brushSizeSpinBox->setValue(
                static_cast<double>(
                    value
                    )
            );

            if (m_canvas)
            {
                m_canvas->setBrushSize(
                    static_cast<double>(
                        value
                        )
                );
            }
        }
    );


    connect(
        m_brushSizeSpinBox,
        &QDoubleSpinBox::valueChanged,
        this,
        [this](double value)
        {
            m_brushSizeSlider->setValue(
                static_cast<int>(
                    value
                    )
            );

            if (m_canvas)
            {
                m_canvas->setBrushSize(
                    value
                );
            }
        }
    );


    // ========================================================
    // OPACITY CONNECTIONS
    // ========================================================

    connect(
        m_brushOpacitySlider,
        &QSlider::valueChanged,
        this,
        [this](int value)
        {
            m_brushOpacitySpinBox->setValue(
                static_cast<double>(
                    value
                    )
            );

            if (m_canvas)
            {
                m_canvas->setBrushOpacity(
                    value / 100.0
                );
            }
        }
    );


    connect(
        m_brushOpacitySpinBox,
        &QDoubleSpinBox::valueChanged,
        this,
        [this](double value)
        {
            m_brushOpacitySlider->setValue(
                static_cast<int>(
                    value
                    )
            );

            if (m_canvas)
            {
                m_canvas->setBrushOpacity(
                    value / 100.0
                );
            }
        }
    );


    // ========================================================
    // HARDNESS CONNECTIONS
    // ========================================================

    connect(
        m_brushHardnessSlider,
        &QSlider::valueChanged,
        this,
        [this](int value)
        {
            m_brushHardnessSpinBox->setValue(
                static_cast<double>(
                    value
                    )
            );

            if (m_canvas)
            {
                m_canvas->setBrushHardness(
                    value / 100.0
                );
            }
        }
    );


    connect(
        m_brushHardnessSpinBox,
        &QDoubleSpinBox::valueChanged,
        this,
        [this](double value)
        {
            m_brushHardnessSlider->setValue(
                static_cast<int>(
                    value
                    )
            );

            if (m_canvas)
            {
                m_canvas->setBrushHardness(
                    value / 100.0
                );
            }
        }
    );


    // ========================================================
    // FLOW CONNECTIONS
    // ========================================================

    connect(
        m_brushFlowSlider,
        &QSlider::valueChanged,
        this,
        [this](int value)
        {
            m_brushFlowSpinBox->setValue(
                static_cast<double>(
                    value
                    )
            );

            if (m_canvas)
            {
                m_canvas->setBrushFlow(
                    value / 100.0
                );
            }
        }
    );


    connect(
        m_brushFlowSpinBox,
        &QDoubleSpinBox::valueChanged,
        this,
        [this](double value)
        {
            m_brushFlowSlider->setValue(
                static_cast<int>(
                    value
                    )
            );

            if (m_canvas)
            {
                m_canvas->setBrushFlow(
                    value / 100.0
                );
            }
        }
    );


    // ========================================================
    // INITIAL VALUES
    // ========================================================

    if (m_canvas)
    {
        m_canvas->setBrushSize(
            40.0
        );

        m_canvas->setBrushOpacity(
            1.0
        );

        m_canvas->setBrushHardness(
            1.0
        );

        m_canvas->setBrushFlow(
            1.0
        );
    }
}


// ============================================================
// COLOR PANEL
// ============================================================

void MainWindow::createColorPanel()
{
    QWidget* panel =
        new QWidget(
            this
        );

    panel->setMinimumWidth(
        250
    );

    QVBoxLayout* layout =
        new QVBoxLayout(
            panel
        );

    layout->setContentsMargins(
        10,
        10,
        10,
        10
    );

    QLabel* colorLabel =
        new QLabel(
            "Current Color",
            panel
        );

    m_colorPreviewButton =
        new QPushButton(
            panel
        );

    m_colorPreviewButton->setMinimumHeight(
        50
    );

    m_colorPreviewButton->setStyleSheet(
        "background-color: #000000;"
        "border: 1px solid #555555;"
    );

    m_colorButton =
        new QPushButton(
            "Color...",
            panel
        );

    layout->addWidget(
        colorLabel
    );

    layout->addWidget(
        m_colorPreviewButton
    );

    layout->addWidget(
        m_colorButton
    );

    layout->addStretch();

    QDockWidget* dock =
        new QDockWidget(
            "Color",
            this
        );

    dock->setWidget(
        panel
    );

    addDockWidget(
        Qt::RightDockWidgetArea,
        dock
    );

    connect(
        m_colorButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            const QColor currentColor =
                m_canvas->brushColor();

            const QColor color =
                QColorDialog::getColor(
                    currentColor,
                    this,
                    "Choose Color"
                );

            if (!color.isValid())
                return;

            m_colorPreviewButton->setStyleSheet(
                QString(
                    "background-color: %1;"
                    "border: 1px solid #555555;"
                ).arg(
                    color.name()
                )
            );

            if (m_canvas)
            {
                m_canvas->setBrushColor(
                    color
                );
            }
        }
    );
}


// ============================================================
// LAYERS
// ============================================================

void MainWindow::createLayersPanel()
{
    QWidget* panel = new QWidget(this);
    panel->setMinimumWidth(250);
    auto* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(10, 10, 10, 10);

    layout->addWidget(new QLabel("Layers", panel));
    m_layersList = new QListWidget(panel);
    m_layersList->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(m_layersList, 1);

    auto* opacityLabel = new QLabel("Opacity", panel);
    layout->addWidget(opacityLabel);
    m_layerOpacitySlider = new QSlider(Qt::Horizontal, panel);
    m_layerOpacitySlider->setRange(0, 100);
    m_layerOpacitySlider->setValue(100);
    layout->addWidget(m_layerOpacitySlider);

    auto* buttons = new QHBoxLayout();
    auto* addButton = new QPushButton("+", panel);
    auto* deleteButton = new QPushButton("−", panel);
    auto* upButton = new QPushButton("↑", panel);
    auto* downButton = new QPushButton("↓", panel);
    addButton->setToolTip("Add layer");
    deleteButton->setToolTip("Delete selected layer");
    upButton->setToolTip("Move layer up");
    downButton->setToolTip("Move layer down");
    buttons->addWidget(addButton);
    buttons->addWidget(deleteButton);
    buttons->addWidget(upButton);
    buttons->addWidget(downButton);
    layout->addLayout(buttons);

    auto* dock = new QDockWidget("Layers", this);
    dock->setWidget(panel);
    addDockWidget(Qt::RightDockWidgetArea, dock);

    connect(m_layersList, &QListWidget::currentRowChanged, this, [this](int row)
        {
            if (m_updatingLayerPanel || !m_document || row < 0)
                return;
            const int index = m_layersList->count() - 1 - row;
            m_document->setActiveLayer(index);
            m_canvas->setActiveLayer(m_document->activeLayer());
            m_layerOpacitySlider->setValue(static_cast<int>(m_document->activeLayer()->opacity() * 100.0f));
            refreshCanvasImage();
        });

    connect(m_layersList, &QListWidget::itemChanged, this, [this](QListWidgetItem* item)
        {
            if (m_updatingLayerPanel || !m_document || !item)
                return;
            const int index = m_layersList->count() - 1 - m_layersList->row(item);
            if (index < 0 || index >= m_document->layers().size())
                return;
            Layer* layer = m_document->layers().at(index);
            pushUndoState();
            layer->setName(item->text());
            layer->setVisible(item->checkState() == Qt::Checked);
            refreshCanvasImage();
        });

    connect(m_layerOpacitySlider, &QSlider::sliderPressed, this, [this]()
        {
            if (!m_updatingLayerPanel && m_document && m_document->activeLayer())
            {
                m_opacityHistoryStarted = true;
                pushUndoState();
            }
        });
    connect(m_layerOpacitySlider, &QSlider::sliderReleased, this, [this]()
        {
            m_opacityHistoryStarted = false;
        });
    connect(m_layerOpacitySlider, &QSlider::valueChanged, this, [this](int value)
        {
            if (m_updatingLayerPanel || !m_document || !m_document->activeLayer())
                return;
            if (!m_opacityHistoryStarted)
                pushUndoState();
            m_document->activeLayer()->setOpacity(value / 100.0f);
            refreshCanvasImage();
        });

    connect(addButton, &QPushButton::clicked, this, &MainWindow::addLayer);

    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteActiveLayer);

    connect(upButton, &QPushButton::clicked, this, [this]()
        {
            if (!m_document) return;
            const int row = m_layersList->currentRow();
            const int index = m_layersList->count() - 1 - row;
            if (row <= 0) return;
            pushUndoState();
            m_document->moveLayer(index, index + 1);
            refreshLayerPanel();
            refreshCanvasImage();
        });

    connect(downButton, &QPushButton::clicked, this, [this]()
        {
            if (!m_document) return;
            const int row = m_layersList->currentRow();
            const int index = m_layersList->count() - 1 - row;
            if (row < 0 || row >= m_layersList->count() - 1) return;
            pushUndoState();
            m_document->moveLayer(index, index - 1);
            refreshLayerPanel();
            refreshCanvasImage();
        });
}

void MainWindow::refreshLayerPanel()
{
    if (!m_layersList) return;
    m_updatingLayerPanel = true;
    m_layersList->clear();
    if (m_document)
    {
        for (int index = m_document->layers().size() - 1; index >= 0; --index)
        {
            Layer* layer = m_document->layers().at(index);
            auto* item = new QListWidgetItem(layer->name(), m_layersList);
            item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            item->setCheckState(layer->isVisible() ? Qt::Checked : Qt::Unchecked);
        }
        const int activeRow = m_document->layers().size() - 1 - m_document->activeLayerIndex();
        if (activeRow >= 0 && activeRow < m_layersList->count())
            m_layersList->setCurrentRow(activeRow);
        if (m_document->activeLayer())
            m_layerOpacitySlider->setValue(static_cast<int>(m_document->activeLayer()->opacity() * 100.0f));
    }
    m_updatingLayerPanel = false;
}

MainWindow::DocumentHistoryState MainWindow::captureDocumentState() const
{
    DocumentHistoryState state;
    if (!m_document)
        return state;

    state.hasDocument = true;
    state.size = QSize(m_document->width(), m_document->height());
    state.activeLayerIndex = m_document->activeLayerIndex();
    state.projectPath = m_projectFilePath;
    if (m_canvas && m_canvas->hasSelection())
        state.selection = m_canvas->selectionRegion();
    for (Layer* layer : m_document->layers())
    {
        LayerHistoryState layerState;
        layerState.name = layer->name();
        layerState.image = layer->image();
        layerState.visible = layer->isVisible();
        layerState.opacity = layer->opacity();
        state.layers.append(layerState);
    }
    return state;
}

void MainWindow::restoreDocumentState(const DocumentHistoryState& state)
{
    m_canvasRefreshTimer.stop();
    m_pendingCanvasDirtyRect = QRect();
    m_canvasNeedsFullRefresh = false;
    delete m_document;
    m_document = nullptr;
    m_compositeImage = QImage();
    m_projectFilePath = state.projectPath;

    if (state.hasDocument)
    {
        m_document = new Document(state.size.width(), state.size.height());
        for (const LayerHistoryState& layerState : state.layers)
        {
            Layer* layer = m_document->addLayer(layerState.name);
            layer->image() = layerState.image;
            layer->setVisible(layerState.visible);
            layer->setOpacity(layerState.opacity);
        }
        if (state.activeLayerIndex >= 0 && state.activeLayerIndex < m_document->layers().size())
            m_document->setActiveLayer(state.activeLayerIndex);
        m_canvas->setActiveLayer(m_document->activeLayer());
        refreshLayerPanel();
        refreshCanvasImage();
        m_canvas->setSelectionRegion(state.selection);
        m_canvas->fitToWindow();
    }
    else
    {
        m_canvas->setActiveLayer(nullptr);
        m_canvas->clearImage();
        m_canvas->setSelectionRegion(QRegion());
        refreshLayerPanel();
    }
}

void MainWindow::pushUndoState()
{
    m_undoStates.append(captureDocumentState());
    constexpr int maxHistoryStates = 20;
    if (m_undoStates.size() > maxHistoryStates)
        m_undoStates.removeFirst();
    m_redoStates.clear();
    updateHistoryActions();
}

void MainWindow::undo()
{
    if (m_undoStates.isEmpty())
        return;
    m_redoStates.append(captureDocumentState());
    const DocumentHistoryState previous = m_undoStates.takeLast();
    restoreDocumentState(previous);
    updateHistoryActions();
}

void MainWindow::redo()
{
    if (m_redoStates.isEmpty())
        return;
    m_undoStates.append(captureDocumentState());
    const DocumentHistoryState next = m_redoStates.takeLast();
    restoreDocumentState(next);
    updateHistoryActions();
}

void MainWindow::updateHistoryActions()
{
    if (m_undoAction)
        m_undoAction->setEnabled(!m_undoStates.isEmpty());
    if (m_redoAction)
        m_redoAction->setEnabled(!m_redoStates.isEmpty());
}

void MainWindow::copySelection()
{
    if (!m_document || !m_document->activeLayer())
        return;
    const QImage& source = m_document->activeLayer()->image();
    const bool useSelection = m_canvas->hasSelection();
    const QRegion selection = m_canvas->selectionRegion();
    const QRect bounds = useSelection ? selection.boundingRect() : source.rect();
    if (bounds.isEmpty())
        return;

    QImage copied(bounds.size(), QImage::Format_ARGB32_Premultiplied);
    copied.fill(Qt::transparent);
    QPainter painter(&copied);
    if (useSelection)
        painter.setClipRegion(selection.translated(-bounds.topLeft()));
    painter.drawImage(-bounds.topLeft(), source);
    painter.end();
    QApplication::clipboard()->setImage(copied);
}

void MainWindow::cutSelection()
{
    if (!m_document || !m_document->activeLayer())
        return;
    copySelection();
    pushUndoState();

    QImage& image = m_document->activeLayer()->image();
    QPainter painter(&image);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    if (m_canvas->hasSelection())
        painter.setClipRegion(m_canvas->selectionRegion());
    painter.fillRect(image.rect(), Qt::transparent);
    painter.end();
    refreshCanvasImage();
}

void MainWindow::pasteImage()
{
    if (!m_document)
        return;
    const QImage pastedImage = QApplication::clipboard()->image();
    if (pastedImage.isNull())
        return;

    pushUndoState();
    const int insertIndex = m_document->activeLayerIndex() >= 0
        ? m_document->activeLayerIndex() + 1
        : m_document->layers().size();
    Layer* pastedLayer = m_document->insertLayer(insertIndex, "Pasted");
    const QPoint position = m_canvas->hasSelection()
        ? m_canvas->selectionRegion().boundingRect().topLeft()
        : QPoint((m_document->width() - pastedImage.width()) / 2,
                 (m_document->height() - pastedImage.height()) / 2);
    QPainter painter(&pastedLayer->image());
    painter.drawImage(position, pastedImage);
    painter.end();
    m_canvas->setActiveLayer(pastedLayer);
    refreshLayerPanel();
    refreshCanvasImage();
}

QImage MainWindow::boxBlurImage(const QImage& image, int radius) const
{
    if (image.isNull() || radius <= 0)
        return image;

    const QImage source = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    QImage horizontal(source.size(), QImage::Format_ARGB32_Premultiplied);
    QImage output(source.size(), QImage::Format_ARGB32_Premultiplied);
    const int width = source.width();
    const int height = source.height();
    const int divisor = radius * 2 + 1;

    for (int y = 0; y < height; ++y)
    {
        const auto* input = reinterpret_cast<const QRgb*>(source.constScanLine(y));
        auto* result = reinterpret_cast<QRgb*>(horizontal.scanLine(y));
        int red = 0, green = 0, blue = 0, alpha = 0;
        for (int k = -radius; k <= radius; ++k)
        {
            const QRgb pixel = input[std::clamp(k, 0, width - 1)];
            red += qRed(pixel); green += qGreen(pixel); blue += qBlue(pixel); alpha += qAlpha(pixel);
        }
        for (int x = 0; x < width; ++x)
        {
            result[x] = qRgba(red / divisor, green / divisor, blue / divisor, alpha / divisor);
            const QRgb leaving = input[std::clamp(x - radius, 0, width - 1)];
            const QRgb entering = input[std::clamp(x + radius + 1, 0, width - 1)];
            red += qRed(entering) - qRed(leaving);
            green += qGreen(entering) - qGreen(leaving);
            blue += qBlue(entering) - qBlue(leaving);
            alpha += qAlpha(entering) - qAlpha(leaving);
        }
    }

    for (int x = 0; x < width; ++x)
    {
        int red = 0, green = 0, blue = 0, alpha = 0;
        for (int k = -radius; k <= radius; ++k)
        {
            const auto* row = reinterpret_cast<const QRgb*>(horizontal.constScanLine(std::clamp(k, 0, height - 1)));
            const QRgb pixel = row[x];
            red += qRed(pixel); green += qGreen(pixel); blue += qBlue(pixel); alpha += qAlpha(pixel);
        }
        for (int y = 0; y < height; ++y)
        {
            auto* result = reinterpret_cast<QRgb*>(output.scanLine(y));
            result[x] = qRgba(red / divisor, green / divisor, blue / divisor, alpha / divisor);
            const auto* leavingRow = reinterpret_cast<const QRgb*>(horizontal.constScanLine(std::clamp(y - radius, 0, height - 1)));
            const auto* enteringRow = reinterpret_cast<const QRgb*>(horizontal.constScanLine(std::clamp(y + radius + 1, 0, height - 1)));
            const QRgb leaving = leavingRow[x];
            const QRgb entering = enteringRow[x];
            red += qRed(entering) - qRed(leaving);
            green += qGreen(entering) - qGreen(leaving);
            blue += qBlue(entering) - qBlue(leaving);
            alpha += qAlpha(entering) - qAlpha(leaving);
        }
    }
    return output;
}

void MainWindow::applyImageFilter(const QString& filterName)
{
    if (!m_document || !m_document->activeLayer())
    {
        statusBar()->showMessage("Create a project and select a layer first", 3000);
        return;
    }

    int radius = 4;
    double sharpenAmount = 1.0;
    bool accepted = true;
    if (filterName == "Blur")
        radius = QInputDialog::getInt(this, "Blur", "Radius (pixels)", 4, 1, 100, 1, &accepted);
    else if (filterName == "Sharpen")
        sharpenAmount = QInputDialog::getDouble(this, "Sharpen", "Amount", 1.0, 0.1, 3.0, 1, &accepted);
    if (!accepted)
        return;

    Layer* layer = m_document->activeLayer();
    QImage& target = layer->image();
    const QImage original = target;
    QImage output;

    if (filterName == "Blur")
    {
        output = boxBlurImage(original, radius);
    }
    else if (filterName == "Sharpen")
    {
        const QImage source = original.convertToFormat(QImage::Format_ARGB32_Premultiplied);
        const QImage blurred = boxBlurImage(source, 1);
        output = QImage(source.size(), QImage::Format_ARGB32_Premultiplied);
        for (int y = 0; y < source.height(); ++y)
        {
            const auto* sourceRow = reinterpret_cast<const QRgb*>(source.constScanLine(y));
            const auto* blurRow = reinterpret_cast<const QRgb*>(blurred.constScanLine(y));
            auto* outputRow = reinterpret_cast<QRgb*>(output.scanLine(y));
            for (int x = 0; x < source.width(); ++x)
            {
                const QRgb pixel = sourceRow[x];
                const QRgb softened = blurRow[x];
                const int alpha = qAlpha(pixel);
                const auto sharpenChannel = [sharpenAmount, alpha](int value, int blurValue)
                    {
                        return std::clamp(qRound(value + sharpenAmount * (value - blurValue)), 0, alpha);
                    };
                outputRow[x] = qRgba(
                    sharpenChannel(qRed(pixel), qRed(softened)),
                    sharpenChannel(qGreen(pixel), qGreen(softened)),
                    sharpenChannel(qBlue(pixel), qBlue(softened)),
                    alpha);
            }
        }
    }
    else
    {
        const QImage source = original.convertToFormat(QImage::Format_ARGB32);
        output = QImage(source.size(), QImage::Format_ARGB32);
        for (int y = 0; y < source.height(); ++y)
        {
            const auto* sourceRow = reinterpret_cast<const QRgb*>(source.constScanLine(y));
            auto* outputRow = reinterpret_cast<QRgb*>(output.scanLine(y));
            for (int x = 0; x < source.width(); ++x)
            {
                const QRgb pixel = sourceRow[x];
                const int alpha = qAlpha(pixel);
                if (filterName == "Grayscale")
                {
                    const int gray = (qRed(pixel) * 299 + qGreen(pixel) * 587 + qBlue(pixel) * 114 + 500) / 1000;
                    outputRow[x] = qRgba(gray, gray, gray, alpha);
                }
                else
                {
                    outputRow[x] = qRgba(255 - qRed(pixel), 255 - qGreen(pixel), 255 - qBlue(pixel), alpha);
                }
            }
        }
    }

    output = output.convertToFormat(original.format());
    pushUndoState();
    if (m_canvas->hasSelection())
    {
        QPainter painter(&target);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.setClipRegion(m_canvas->selectionRegion());
        painter.drawImage(0, 0, output);
    }
    else
    {
        target = output;
    }
    refreshCanvasImage();
    statusBar()->showMessage(filterName + " applied", 2000);
}

void MainWindow::refreshCanvasImage(const QRect& dirtyRect)
{
    if (!m_canvas || !m_document || m_document->width() <= 0 || m_document->height() <= 0)
        return;

    const QSize documentSize(m_document->width(), m_document->height());
    const bool canRefreshRegion = !dirtyRect.isNull() &&
        !m_compositeImage.isNull() && m_compositeImage.size() == documentSize;
    if (canRefreshRegion)
    {
        const QRect clipped = dirtyRect.intersected(m_compositeImage.rect());
        if (clipped.isEmpty())
            return;

        QPainter painter(&m_compositeImage);
        painter.setClipRect(clipped);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(clipped, Qt::transparent);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        for (Layer* layer : m_document->layers())
        {
            if (!layer->isVisible()) continue;
            painter.setOpacity(layer->opacity());
            painter.drawImage(0, 0, layer->image());
        }
        painter.end();
        m_canvas->setImageRegion(clipped, m_compositeImage.copy(clipped));
        return;
    }

    QImage composite(m_document->width(), m_document->height(), QImage::Format_ARGB32_Premultiplied);
    composite.fill(Qt::transparent);
    QPainter painter(&composite);
    for (Layer* layer : m_document->layers())
    {
        if (!layer->isVisible()) continue;
        painter.setOpacity(layer->opacity());
        painter.drawImage(0, 0, layer->image());
    }
    painter.end();
    m_compositeImage = std::move(composite);
    m_canvas->setImage(m_compositeImage);
}

void MainWindow::addLayer()
{
    if (!m_document)
    {
        statusBar()->showMessage("Create a project before adding layers", 3000);
        return;
    }
    pushUndoState();
    Layer* layer = m_document->addLayer(QString("Layer %1").arg(m_document->layers().size() + 1));
    m_canvas->setActiveLayer(layer);
    m_canvas->deselect();
    refreshLayerPanel();
    refreshCanvasImage();
}

void MainWindow::duplicateActiveLayer()
{
    if (!m_document || !m_document->activeLayer())
    {
        statusBar()->showMessage("Select a layer to duplicate", 3000);
        return;
    }
    pushUndoState();
    const int sourceIndex = m_document->activeLayerIndex();
    Layer* source = m_document->activeLayer();
    Layer* duplicate = m_document->insertLayer(sourceIndex + 1, source->name() + " copy");
    duplicate->image() = source->image().copy();
    duplicate->setVisible(source->isVisible());
    duplicate->setOpacity(source->opacity());
    m_canvas->setActiveLayer(duplicate);
    refreshLayerPanel();
    refreshCanvasImage();
}

void MainWindow::deleteActiveLayer()
{
    if (!m_document || m_document->layers().size() <= 1)
    {
        statusBar()->showMessage("A document must keep at least one layer", 3000);
        return;
    }
    pushUndoState();
    m_document->removeLayer(m_document->activeLayerIndex());
    m_canvas->setActiveLayer(m_document->activeLayer());
    refreshLayerPanel();
    refreshCanvasImage();
}

void MainWindow::mergeActiveLayerDown()
{
    if (!m_document || m_document->activeLayerIndex() <= 0)
    {
        statusBar()->showMessage("Select a layer above another layer to merge down", 3000);
        return;
    }
    pushUndoState();
    const int upperIndex = m_document->activeLayerIndex();
    Layer* upper = m_document->layers().at(upperIndex);
    Layer* lower = m_document->layers().at(upperIndex - 1);

    QImage merged(m_document->width(), m_document->height(), QImage::Format_ARGB32_Premultiplied);
    merged.fill(Qt::transparent);
    QPainter painter(&merged);
    if (lower->isVisible())
    {
        painter.setOpacity(lower->opacity());
        painter.drawImage(0, 0, lower->image());
    }
    if (upper->isVisible())
    {
        painter.setOpacity(upper->opacity());
        painter.drawImage(0, 0, upper->image());
    }
    painter.end();

    lower->image() = merged;
    lower->setOpacity(1.0f);
    lower->setVisible(lower->isVisible() || upper->isVisible());
    m_document->setActiveLayer(upperIndex - 1);
    m_document->removeLayer(upperIndex);
    m_canvas->setActiveLayer(m_document->activeLayer());
    refreshLayerPanel();
    refreshCanvasImage();
}

void MainWindow::showImageSizeDialog()
{
    if (!m_document) { statusBar()->showMessage("Create a project first", 3000); return; }
    QDialog dialog(this);
    dialog.setWindowTitle("Image Size");
    auto* layout = new QGridLayout(&dialog);
    auto* width = new QSpinBox(&dialog);
    auto* height = new QSpinBox(&dialog);
    width->setRange(1, 10000); height->setRange(1, 10000);
    width->setValue(m_document->width()); height->setValue(m_document->height());
    auto* keepRatio = new QCheckBox("Keep aspect ratio", &dialog);
    keepRatio->setChecked(true);
    layout->addWidget(new QLabel("Width (px)"), 0, 0); layout->addWidget(width, 0, 1);
    layout->addWidget(new QLabel("Height (px)"), 1, 0); layout->addWidget(height, 1, 1);
    layout->addWidget(keepRatio, 2, 0, 1, 2);
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons, 3, 0, 1, 2);
    const double ratio = static_cast<double>(m_document->width()) / m_document->height();
    connect(width, qOverload<int>(&QSpinBox::valueChanged), &dialog, [=](int value)
        {
            if (!keepRatio->isChecked()) return;
            QSignalBlocker blocker(height);
            height->setValue(std::clamp(static_cast<int>(std::lround(value / ratio)), 1, 10000));
        });
    connect(height, qOverload<int>(&QSpinBox::valueChanged), &dialog, [=](int value)
        {
            if (!keepRatio->isChecked()) return;
            QSignalBlocker blocker(width);
            width->setValue(std::clamp(static_cast<int>(std::lround(value * ratio)), 1, 10000));
        });
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() == QDialog::Accepted)
        scaleDocument(width->value(), height->value());
}

void MainWindow::showCanvasSizeDialog()
{
    if (!m_document) { statusBar()->showMessage("Create a project first", 3000); return; }
    QDialog dialog(this);
    dialog.setWindowTitle("Canvas Size");
    auto* layout = new QGridLayout(&dialog);
    auto* width = new QSpinBox(&dialog);
    auto* height = new QSpinBox(&dialog);
    width->setRange(1, 10000); height->setRange(1, 10000);
    width->setValue(m_document->width()); height->setValue(m_document->height());
    auto* anchor = new QComboBox(&dialog);
    anchor->addItem("Top left", QPoint(0, 0));
    anchor->addItem("Top center", QPoint(1, 0));
    anchor->addItem("Top right", QPoint(2, 0));
    anchor->addItem("Center left", QPoint(0, 1));
    anchor->addItem("Center", QPoint(1, 1));
    anchor->addItem("Center right", QPoint(2, 1));
    anchor->addItem("Bottom left", QPoint(0, 2));
    anchor->addItem("Bottom center", QPoint(1, 2));
    anchor->addItem("Bottom right", QPoint(2, 2));
    layout->addWidget(new QLabel("Width (px)"), 0, 0); layout->addWidget(width, 0, 1);
    layout->addWidget(new QLabel("Height (px)"), 1, 0); layout->addWidget(height, 1, 1);
    layout->addWidget(new QLabel("Anchor"), 2, 0); layout->addWidget(anchor, 2, 1);
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons, 3, 0, 1, 2);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() != QDialog::Accepted) return;
    const QPoint gridAnchor = anchor->currentData().toPoint();
    const QPoint offset((width->value() - m_document->width()) * gridAnchor.x() / 2,
                        (height->value() - m_document->height()) * gridAnchor.y() / 2);
    resizeDocumentCanvas(width->value(), height->value(), offset);
}

void MainWindow::showRotateDialog()
{
    if (!m_document) { statusBar()->showMessage("Create a project first", 3000); return; }
    QDialog dialog(this);
    dialog.setWindowTitle("Rotate Image");
    auto* layout = new QVBoxLayout(&dialog);
    auto* angle = new QDoubleSpinBox(&dialog);
    angle->setRange(-360.0, 360.0); angle->setDecimals(1); angle->setSingleStep(15.0); angle->setValue(90.0);
    layout->addWidget(new QLabel("Angle in degrees (positive rotates clockwise)"));
    layout->addWidget(angle);
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() != QDialog::Accepted || qFuzzyIsNull(angle->value())) return;
    pushUndoState();
    QTransform transform;
    transform.rotate(angle->value());
    QSize newSize;
    for (Layer* layer : m_document->layers())
    {
        QImage rotated = layer->image().transformed(transform, Qt::SmoothTransformation);
        if (newSize.isEmpty()) newSize = rotated.size();
        layer->image() = rotated;
    }
    if (!newSize.isEmpty())
    {
        m_document->setSize(newSize.width(), newSize.height());
        m_canvas->setActiveLayer(m_document->activeLayer());
        m_canvas->deselect();
        refreshCanvasImage();
        m_canvas->fitToWindow();
    }
}

void MainWindow::flipDocument(bool horizontal)
{
    if (!m_document) { statusBar()->showMessage("Create a project first", 3000); return; }
    pushUndoState();
    for (Layer* layer : m_document->layers())
        layer->image() = layer->image().mirrored(horizontal, !horizontal);
    m_canvas->deselect();
    refreshCanvasImage();
}

void MainWindow::scaleDocument(int width, int height)
{
    if (!m_document || width <= 0 || height <= 0) return;
    pushUndoState();
    for (Layer* layer : m_document->layers())
        layer->image() = layer->image().scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    m_document->setSize(width, height);
    m_canvas->setActiveLayer(m_document->activeLayer());
    m_canvas->deselect();
    refreshCanvasImage();
    m_canvas->fitToWindow();
}

void MainWindow::resizeDocumentCanvas(int width, int height, const QPoint& offset)
{
    if (!m_document || width <= 0 || height <= 0) return;
    pushUndoState();
    for (Layer* layer : m_document->layers())
    {
        QImage resized(width, height, QImage::Format_RGBA8888);
        resized.fill(Qt::transparent);
        QPainter painter(&resized);
        painter.drawImage(offset, layer->image());
        layer->image() = resized;
    }
    m_document->setSize(width, height);
    m_canvas->setActiveLayer(m_document->activeLayer());
    m_canvas->deselect();
    refreshCanvasImage();
    m_canvas->fitToWindow();
}
// ============================================================
// CANVAS
// ============================================================

void MainWindow::createCanvas()
{
    m_canvas =
        new CanvasWidget(
            this
        );

    setCentralWidget(
        m_canvas
    );
}


// ============================================================
// STATUS BAR
// ============================================================

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(
        "Ready"
    );
}


// ============================================================
// NEW PROJECT
// ============================================================

void MainWindow::newProject()
{
    QDialog dialog(
        this
    );

    dialog.setWindowTitle(
        "New Project"
    );

    dialog.setMinimumWidth(
        400
    );

    QVBoxLayout* layout =
        new QVBoxLayout(
            &dialog
        );


    // WIDTH

    QLabel* widthLabel =
        new QLabel(
            "Width",
            &dialog
        );

    QSpinBox* widthSpinBox =
        new QSpinBox(
            &dialog
        );

    widthSpinBox->setRange(
        1,
        10000
    );

    widthSpinBox->setValue(
        1920
    );


    // HEIGHT

    QLabel* heightLabel =
        new QLabel(
            "Height",
            &dialog
        );

    QSpinBox* heightSpinBox =
        new QSpinBox(
            &dialog
        );

    heightSpinBox->setRange(
        1,
        10000
    );

    heightSpinBox->setValue(
        1080
    );


    // BACKGROUND

    QLabel* backgroundLabel =
        new QLabel(
            "Background",
            &dialog
        );

    QComboBox* backgroundComboBox =
        new QComboBox(
            &dialog
        );

    backgroundComboBox->addItem(
        "Transparent",
        "transparent"
    );

    backgroundComboBox->addItem(
        "White",
        "white"
    );

    backgroundComboBox->addItem(
        "Black",
        "black"
    );


    // BUTTONS

    QHBoxLayout* buttons =
        new QHBoxLayout();

    QPushButton* createButton =
        new QPushButton(
            "Create",
            &dialog
        );

    QPushButton* cancelButton =
        new QPushButton(
            "Cancel",
            &dialog
        );

    buttons->addStretch();

    buttons->addWidget(
        cancelButton
    );

    buttons->addWidget(
        createButton
    );


    // LAYOUT

    layout->addWidget(
        widthLabel
    );

    layout->addWidget(
        widthSpinBox
    );

    layout->addWidget(
        heightLabel
    );

    layout->addWidget(
        heightSpinBox
    );

    layout->addWidget(
        backgroundLabel
    );

    layout->addWidget(
        backgroundComboBox
    );

    layout->addStretch();

    layout->addLayout(
        buttons
    );


    connect(
        createButton,
        &QPushButton::clicked,
        &dialog,
        &QDialog::accept
    );

    connect(
        cancelButton,
        &QPushButton::clicked,
        &dialog,
        &QDialog::reject
    );


    if (
        dialog.exec()
        !=
        QDialog::Accepted
        )
    {
        return;
    }


    // CREATE DOCUMENT

    pushUndoState();
    m_projectFilePath.clear();

    delete m_document;

    m_document =
        new Document(
            widthSpinBox->value(),
            heightSpinBox->value()
        );


    Layer* layer =
        m_document->addLayer(
            "Background"
        );


    QString background =
        backgroundComboBox
        ->currentData()
        .toString();


    if (
        background ==
        "white"
        )
    {
        layer->image().fill(
            Qt::white
        );
    }
    else if (
        background ==
        "black"
        )
    {
        layer->image().fill(
            Qt::black
        );
    }
    else
    {
        layer->image().fill(
            Qt::transparent
        );
    }


    m_canvas->setActiveLayer(layer);
    m_canvas->deselect();
    setWindowTitle("Feather Photo");
    refreshLayerPanel();
    refreshCanvasImage();

    // STATUS

    statusBar()->showMessage(
        QString(
            "Created %1 x %2 project"
        )
        .arg(
            m_document->width()
        )
        .arg(
            m_document->height()
        )
    );


    m_canvas->fitToWindow();
}

void MainWindow::openProject()
{
    const QString path = QFileDialog::getOpenFileName(
        this, translated("Open Project"), QString(),
        translated("Feather Photo Project") + " (*.fphoto);;" + translated("Images") +
        " (*.png *.jpg *.jpeg *.bmp *.webp *.tif *.tiff)");
    if (path.isEmpty())
        return;

    DocumentHistoryState state;
    if (QFileInfo(path).suffix().compare("fphoto", Qt::CaseInsensitive) != 0)
    {
        QImageReader reader(path);
        reader.setAutoTransform(true);
        const QImage image = reader.read();
        if (image.isNull())
        {
            QMessageBox::warning(this, translated("Open Project"), reader.errorString());
            return;
        }
        state.hasDocument = true;
        state.size = image.size();
        state.activeLayerIndex = 0;
        LayerHistoryState layer;
        layer.name = QFileInfo(path).completeBaseName();
        layer.image = image.convertToFormat(QImage::Format_RGBA8888);
        state.layers.append(layer);
        pushUndoState();
        restoreDocumentState(state);
        m_projectFilePath.clear();
        setWindowTitle("Feather Photo — " + QFileInfo(path).fileName());
        statusBar()->showMessage(translated("Image opened"), 3000);
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, translated("Open Project"), translated("The project file could not be opened."));
        return;
    }

    QDataStream stream(&file);
    stream.setVersion(QDataStream::Qt_6_0);
    quint32 magic = 0;
    quint16 version = 0;
    qint32 width = 0, height = 0, activeLayer = -1;
    quint32 layerCount = 0;
    stream >> magic >> version >> width >> height >> activeLayer >> layerCount;
    if (stream.status() != QDataStream::Ok || magic != 0x46504854u || version != 1 ||
        width <= 0 || height <= 0 || width > 10000 || height > 10000 || layerCount == 0 || layerCount > 256)
    {
        QMessageBox::warning(this, translated("Open Project"), translated("This is not a valid Feather Photo project."));
        return;
    }

    state.hasDocument = true;
    state.size = QSize(width, height);
    state.activeLayerIndex = activeLayer;
    state.projectPath = path;
    for (quint32 index = 0; index < layerCount; ++index)
    {
        LayerHistoryState layer;
        stream >> layer.name >> layer.visible >> layer.opacity >> layer.image;
        if (stream.status() != QDataStream::Ok || layer.image.isNull() || layer.image.size() != state.size)
        {
            QMessageBox::warning(this, translated("Open Project"), translated("The project data is incomplete or damaged."));
            return;
        }
        state.layers.append(layer);
    }
    if (activeLayer < 0 || activeLayer >= static_cast<qint32>(layerCount))
        state.activeLayerIndex = static_cast<int>(layerCount) - 1;

    pushUndoState();
    restoreDocumentState(state);
    m_projectFilePath = path;
    setWindowTitle("Feather Photo — " + QFileInfo(path).fileName());
    statusBar()->showMessage(translated("Project opened"), 3000);
}

void MainWindow::saveProject()
{
    if (!m_document || m_document->layers().isEmpty())
    {
        statusBar()->showMessage(translated("Create or open a project first"), 3000);
        return;
    }

    QString path = m_projectFilePath;
    if (path.isEmpty())
        path = QFileDialog::getSaveFileName(
            this, translated("Save Project"), QString(),
            translated("Feather Photo Project") + " (*.fphoto)");
    if (path.isEmpty())
        return;
    if (QFileInfo(path).suffix().isEmpty())
        path += ".fphoto";

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, translated("Save Project"), translated("The project file could not be saved."));
        return;
    }
    QDataStream stream(&file);
    stream.setVersion(QDataStream::Qt_6_0);
    stream << quint32(0x46504854u) << quint16(1)
           << qint32(m_document->width()) << qint32(m_document->height())
           << qint32(m_document->activeLayerIndex()) << quint32(m_document->layers().size());
    for (Layer* layer : m_document->layers())
        stream << layer->name() << layer->isVisible() << layer->opacity() << layer->image();

    if (stream.status() != QDataStream::Ok || !file.commit())
    {
        QMessageBox::warning(this, translated("Save Project"), translated("The project could not be written completely."));
        return;
    }
    m_projectFilePath = path;
    setWindowTitle("Feather Photo — " + QFileInfo(path).fileName());
    statusBar()->showMessage(translated("Project saved"), 3000);
}

void MainWindow::exportImage()
{
    if (!m_document || m_document->layers().isEmpty())
    {
        statusBar()->showMessage(translated("Create or open a project first"), 3000);
        return;
    }

    struct ExportFormat
    {
        QString name;
        QString extension;
        QByteArray qtFormat;
        bool lossy = false;
        bool supportsAlpha = true;
    };
    QStringList supported;
    for (const QByteArray& format : QImageWriter::supportedImageFormats())
        supported.append(QString::fromLatin1(format).toLower());
    QVector<ExportFormat> formats;
    auto addFormat = [&formats, &supported](const QString& name, const QString& extension,
                                            const QByteArray& qtFormat, std::initializer_list<const char*> aliases,
                                            bool lossy = false, bool supportsAlpha = true)
        {
            for (const char* alias : aliases)
                if (supported.contains(QString::fromLatin1(alias)))
                {
                    formats.append({ name, extension, qtFormat, lossy, supportsAlpha });
                    return;
                }
        };
    addFormat("PNG", "png", "png", { "png" });
    addFormat("JPEG", "jpg", "jpeg", { "jpeg", "jpg" }, true, false);
    addFormat("WebP", "webp", "webp", { "webp" }, true);
    addFormat("TIFF", "tiff", "tiff", { "tiff", "tif" });
    addFormat("BMP", "bmp", "bmp", { "bmp" }, false, false);
    addFormat("GIF", "gif", "gif", { "gif" }, false, false);
    addFormat("TGA", "tga", "tga", { "tga" });
    addFormat("ICO", "ico", "ico", { "ico" });
    addFormat("PPM", "ppm", "ppm", { "ppm" }, false, false);
    addFormat("PGM", "pgm", "pgm", { "pgm" }, false, false);
    addFormat("PBM", "pbm", "pbm", { "pbm" }, false, false);
    addFormat("WBMP", "wbmp", "wbmp", { "wbmp" }, false, false);
    addFormat("XPM", "xpm", "xpm", { "xpm" }, false, false);
    addFormat("XBM", "xbm", "xbm", { "xbm" }, false, false);
    if (formats.isEmpty())
    {
        QMessageBox::warning(this, translated("Export Image"), translated("Qt does not provide any supported image writer formats."));
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(translated("Export Image"));
    dialog.resize(520, 260);
    auto* layout = new QVBoxLayout(&dialog);
    auto* form = new QFormLayout();
    auto* pathEdit = new QLineEdit(&dialog);
    pathEdit->setPlaceholderText(translated("Choose where to save the image"));
    const QString defaultBase = m_projectFilePath.isEmpty() ? QString("FeatherPhoto_export")
                                                            : QFileInfo(m_projectFilePath).completeBaseName() + "_export";
    pathEdit->setText(defaultBase + ".png");
    auto* browseButton = new QPushButton(translated("Browse..."), &dialog);
    auto* pathRow = new QWidget(&dialog);
    auto* pathLayout = new QHBoxLayout(pathRow);
    pathLayout->setContentsMargins(0, 0, 0, 0);
    pathLayout->addWidget(pathEdit, 1);
    pathLayout->addWidget(browseButton);
    form->addRow(translated("Save to"), pathRow);

    auto* formatCombo = new QComboBox(&dialog);
    QStringList filters;
    for (const ExportFormat& format : formats)
    {
        formatCombo->addItem(format.name, QString::fromLatin1(format.qtFormat));
        filters.append(format.name + " (*." + format.extension + ")");
    }
    formatCombo->setCurrentIndex(0);
    form->addRow(translated("Format"), formatCombo);

    auto* qualityLabel = new QLabel(translated("Quality"), &dialog);
    auto* qualitySpin = new QSpinBox(&dialog);
    qualitySpin->setRange(1, 100);
    qualitySpin->setValue(90);
    form->addRow(qualityLabel, qualitySpin);

    auto* transparentCheck = new QCheckBox(translated("Preserve transparency"), &dialog);
    transparentCheck->setChecked(true);
    form->addRow(QString(), transparentCheck);
    layout->addLayout(form);

    bool preserveAlphaPreference = true;
    bool previousFormatSupportsAlpha = true;
    auto updateFormatOptions = [formats, formatCombo, qualityLabel, qualitySpin, transparentCheck,
                                &preserveAlphaPreference, &previousFormatSupportsAlpha]()
        {
            const int index = formatCombo->currentIndex();
            if (index < 0 || index >= formats.size()) return;
            const bool lossy = formats.at(index).lossy;
            qualityLabel->setVisible(lossy);
            qualitySpin->setVisible(lossy);
            const bool supportsAlpha = formats.at(index).supportsAlpha;
            transparentCheck->setEnabled(supportsAlpha);
            if (previousFormatSupportsAlpha && !supportsAlpha)
                preserveAlphaPreference = transparentCheck->isChecked();
            if (supportsAlpha && !previousFormatSupportsAlpha)
                transparentCheck->setChecked(preserveAlphaPreference);
            else if (!supportsAlpha)
                transparentCheck->setChecked(false);
            previousFormatSupportsAlpha = supportsAlpha;
        };
    connect(formatCombo, qOverload<int>(&QComboBox::currentIndexChanged), &dialog,
            [updateFormatOptions](int) { updateFormatOptions(); });
    updateFormatOptions();

    const QString fileFilters = filters.join(";;");
    connect(browseButton, &QPushButton::clicked, &dialog, [this, pathEdit, fileFilters]()
        {
            const QString path = QFileDialog::getSaveFileName(this, translated("Export Image"), pathEdit->text(), fileFilters);
            if (!path.isEmpty()) pathEdit->setText(path);
        });

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);
    buttons->button(QDialogButtonBox::Save)->setText(translated("Export"));
    buttons->button(QDialogButtonBox::Cancel)->setText(translated("Cancel"));
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() != QDialog::Accepted)
        return;

    const int formatIndex = formatCombo->currentIndex();
    if (formatIndex < 0 || pathEdit->text().trimmed().isEmpty())
        return;
    const ExportFormat format = formats.at(formatIndex);
    QFileInfo requestedPath(pathEdit->text().trimmed());
    QString path = requestedPath.absoluteFilePath();
    if (requestedPath.suffix().compare(format.extension, Qt::CaseInsensitive) != 0)
        path = requestedPath.dir().filePath(requestedPath.completeBaseName() + "." + format.extension);

    QImage composite(m_document->width(), m_document->height(), QImage::Format_ARGB32_Premultiplied);
    composite.fill(Qt::transparent);
    QPainter painter(&composite);
    for (Layer* layer : m_document->layers())
    {
        if (!layer->isVisible()) continue;
        painter.setOpacity(layer->opacity());
        painter.drawImage(0, 0, layer->image());
    }
    painter.end();

    if (!transparentCheck->isChecked())
    {
        QImage flattened(composite.size(), QImage::Format_RGB32);
        flattened.fill(Qt::white);
        QPainter flattenPainter(&flattened);
        flattenPainter.drawImage(0, 0, composite);
        flattenPainter.end();
        composite = flattened;
    }
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, translated("Export Image"), translated("The image file could not be created."));
        return;
    }
    QImageWriter writer(&file, format.qtFormat);
    if (format.lossy)
        writer.setQuality(qualitySpin->value());
    if (!writer.write(composite) || !file.commit())
    {
        QMessageBox::warning(this, translated("Export Image"), writer.errorString());
        return;
    }
    statusBar()->showMessage(translated("Image exported"), 3000);
}
