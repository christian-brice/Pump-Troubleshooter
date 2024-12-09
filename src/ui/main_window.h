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
#include <QThread>      // Qt::Core

// Project Headers
//   (none)

QT_BEGIN_NAMESPACE

namespace Ui {  // NOLINT: Qt-generated
class MainWindow;
}  // namespace Ui

QT_END_NAMESPACE

class PumpThread : public QThread {
    // NOLINTBEGIN: required by Qt
    Q_OBJECT
    // NOLINTEND

  public:
    explicit PumpThread(QObject* parent, bool debug_mode = false);
    ~PumpThread() override;

    void SetSerialWater(const QString& sel);

  public slots:
    void SetDebugMode(const bool& enabled);
    void UpdatePumps(const QBitArray& new_cmd);

  private:
    void run() override;

    // --- Helper Functions ---

    // --- Data Members ---

    bool debug_mode_{false};

    QSerialPort* ser_water_{nullptr};
    QByteArray current_state_{1, '\x00'};
    QByteArray last_state_{};  // only used for debug output
};

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

  signals:
    void UpdateDebugMode(const bool& enabled);
    void CommandPumps(const QBitArray& new_cmd);

    // NOLINTBEGIN: Qt-generated
  private slots:
    // --- Menu Bar ---

    // Preferences Menu

    void on_a_debug_mode_toggled(bool checked);

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

    // --- Data Members ---

    Ui::MainWindow* ui_;
    PumpThread* pump_thread_;

    bool debug_mode_{false};

    QBitArray current_cmd_{4};  // 4 bits
};
