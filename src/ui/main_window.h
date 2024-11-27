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
#include <QBitArray>    // Qt::Core
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

    void on_a_debug_mode_toggled(bool checked);

    // Tools Menu

    void on_a_serial_dialog_triggered();

    // --- Main Window ---

    void on_cb_serial_name_currentTextChanged(const QString& sel);
    void on_pb_refresh_clicked();

    void on_tb_a_in_clicked();
    void on_tb_b_in_clicked();
    void on_tb_a_out_clicked();
    void on_tb_b_out_clicked();

    // --- Uncategorized ---

  private:
    // NOLINTEND

    // --- Helper Functions ---

    void WriteData(const QBitArray data);

    // --- Data Members ---

    Ui::MainWindow* ui_;

    bool debug_mode_{false};

    QSerialPort* ser_water_{nullptr};
    QBitArray current_cmd_{4};
};
