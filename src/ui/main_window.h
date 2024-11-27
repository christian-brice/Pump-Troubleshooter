/******************************************************************************
 * @file   main_window.h
 * @brief  Main app window header file.
 *
 * @author brice.c.aa
 ******************************************************************************/

#pragma once

// C++ Standard Library Headers
#include <fstream>

// Other Library Headers
#include <QMainWindow>  // Qt::Widgets
#include <QSerialPort>  // Qt::SerialPort

// Project Headers
//   (none)

QT_BEGIN_NAMESPACE

namespace Ui {  // NOLINT: Qt-generated
class MainWindow;
}  // namespace Ui

QT_END_NAMESPACE

/**
 * @brief The main command app window.
 */
class MainWindow : public QMainWindow {
    // NOLINTBEGIN: required by Qt
    Q_OBJECT
    // NOLINTEND

  public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    // NOLINTBEGIN: Qt-generated
  private slots:
    // --- Menu Bar ---

    // Preferences Menu

    // void on_a_debug_mode_toggled(bool checked);

    // Pumps Menu

    // void on_a_pump_connect_triggered();
    // void on_a_pump_disconnect_triggered();

    // --- Main Window ---

    // --- Uncategorized ---

  private:
    // NOLINTEND

    // --- Helper Functions ---

    // --- Data Members ---

    Ui::MainWindow* ui_;

    bool debug_mode_{true};

    QSerialPort* ser_water_{nullptr};
};
