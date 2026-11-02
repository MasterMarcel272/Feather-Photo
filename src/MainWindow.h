#pragma once

#include <QMainWindow>

#include <QString>
#include <QPoint>
#include <QTimer>
#include <QImage>
#include <QRect>
#include <QVector>
#include <QList>
#include <QSize>
#include <QRegion>

class QListWidget;
class CanvasWidget;
class Document;

class QDoubleSpinBox;
class QSlider;
class QPushButton;
class QButtonGroup;
class QAction;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void createMenus();
    void createToolBar();
    void createToolPanel();
    void setActiveTool(const QString& toolName);
    void createBrushPanel();
    void createColorPanel();
    void createLayersPanel();
    void createCanvas();
    void createStatusBar();
    void refreshLayerPanel();
    void refreshCanvasImage(const QRect& dirtyRect = QRect());
    struct LayerHistoryState
    {
        QString name;
        QImage image;
        bool visible = true;
        float opacity = 1.0f;
    };
    struct DocumentHistoryState
    {
        bool hasDocument = false;
        QSize size;
        QList<LayerHistoryState> layers;
        int activeLayerIndex = -1;
        QRegion selection;
        QString projectPath;
    };
    DocumentHistoryState captureDocumentState() const;
    void restoreDocumentState(const DocumentHistoryState& state);
    void pushUndoState();
    void undo();
    void redo();
    void copySelection();
    void cutSelection();
    void pasteImage();
    void applyImageFilter(const QString& filterName);
    QImage boxBlurImage(const QImage& image, int radius) const;
    void openSettingsDialog();
    void applyTheme(const QString& themeName);
    void translateUi();
    QString translated(const QString& englishText) const;
    void updateHistoryActions();
    void addLayer();
    void duplicateActiveLayer();
    void deleteActiveLayer();
    void mergeActiveLayerDown();
    void showImageSizeDialog();
    void showCanvasSizeDialog();
    void showRotateDialog();
    void flipDocument(bool horizontal);
    void scaleDocument(int width, int height);
    void resizeDocumentCanvas(int width, int height, const QPoint& offset);

    void newProject();
    void openProject();
    void saveProject();
    void exportImage();

    QListWidget* m_layersList = nullptr;
    QSlider* m_layerOpacitySlider = nullptr;
    bool m_updatingLayerPanel = false;

    CanvasWidget* m_canvas = nullptr;
    QTimer m_canvasRefreshTimer;
    QImage m_compositeImage;
    QRect m_pendingCanvasDirtyRect;
    bool m_canvasNeedsFullRefresh = false;
    QVector<DocumentHistoryState> m_undoStates;
    QVector<DocumentHistoryState> m_redoStates;
    QAction* m_undoAction = nullptr;
    QAction* m_redoAction = nullptr;
    QAction* m_newProjectAction = nullptr;
    QAction* m_openProjectAction = nullptr;
    QAction* m_saveProjectAction = nullptr;
    QAction* m_exportAction = nullptr;
    bool m_opacityHistoryStarted = false;
    QString m_projectFilePath;
    QString m_baseStyleSheet;
    QString m_themeName = "Midnight";
    QString m_language = "en";

    Document* m_document = nullptr;

    // Brush Size
    QDoubleSpinBox* m_brushSizeSpinBox = nullptr;
    QSlider* m_brushSizeSlider = nullptr;

    // Brush Opacity
    QDoubleSpinBox* m_brushOpacitySpinBox = nullptr;
    QSlider* m_brushOpacitySlider = nullptr;

    // Brush Hardness
    QDoubleSpinBox* m_brushHardnessSpinBox = nullptr;
    QSlider* m_brushHardnessSlider = nullptr;

    // Brush Flow
    QDoubleSpinBox* m_brushFlowSpinBox = nullptr;
    QSlider* m_brushFlowSlider = nullptr;

    // Tools
    QButtonGroup* m_toolButtonGroup = nullptr;
    QPushButton* m_brushToolButton = nullptr;
    QPushButton* m_eraserToolButton = nullptr;
    QPushButton* m_pencilToolButton = nullptr;
    QPushButton* m_lineToolButton = nullptr;
    QPushButton* m_rectToolButton = nullptr;
    QPushButton* m_ellipseToolButton = nullptr;
    QPushButton* m_fillToolButton = nullptr;
    QPushButton* m_pickerToolButton = nullptr;
    QPushButton* m_selectToolButton = nullptr;
    QPushButton* m_moveToolButton = nullptr;
    QPushButton* m_cropToolButton = nullptr;

    // Color
    QPushButton* m_colorPreviewButton = nullptr;
    QPushButton* m_colorButton = nullptr;
};
