/******************************************************************************
 * @file   pump_thread.h
 * @brief  Pump control thread header file.
 *
 * @author brice.c.aa
 ******************************************************************************/

#pragma once

// C++ Standard Library Headers
//   (none)

// Other Library Headers
#include <QBitArray>    // Qt::Core
#include <QSerialPort>  // Qt::SerialPort
#include <QThread>      // Qt::Core

// Project Headers
//   (none)

/**
 * @brief TODO: documentation.
 */
class PumpThread : public QThread {
    // NOLINTBEGIN: required by Qt
    Q_OBJECT

    // NOLINTEND

  public:
    /**
     * @brief LIBRA-I fluid system states.
     *
     * @note LIBRA-II only uses the right half of the system
     *       (i.e., kRightFill and kRightDrain).
     */
    enum State {
        kLeftStop = 0,
        kRightStop,
        kLeftFill,
        kRightFill,
        kLeftDrain,
        kRightDrain
    };

    explicit PumpThread(QObject* parent, bool debug_mode = false);
    ~PumpThread() override;

  public slots:
    void SetDebugMode(const bool& enabled);

    // --- Pump Commands ---

    void OpenConnection(const QString& port_name);
    void RefreshConnections();
    void UpdateState(const State& state);

  signals:
    // --- Pump Updates ---

    void InformAvailable(const std::vector<QString>& available_ports);

  private:
    void run() override;

    // --- Helper Functions ---

    // --- Data Members ---

    bool debug_mode_{false};

    QSerialPort* ser_water_{nullptr};

    QBitArray current_state_{4};  // 4 bits
    QBitArray last_state_{4};     // only used for debug output
};