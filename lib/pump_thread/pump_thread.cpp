/******************************************************************************
 * @file   pump_thread.cpp
 * @brief  Pump control thread implementation file.
 *
 * @author brice.c.aa
 ******************************************************************************/

// Related Header
#include "pump_thread.h"

// C++ Standard Library Headers
//   (none)

// Other Library Headers
#include <QDebug>           // Qt::Core
#include <QSerialPortInfo>  // Qt::SerialPort

// Project Headers
//   (none)

/* --- TABLE OF CONTENTS ---
 * !Helper Functions
 * !Thread Overrides
 * !Slots
 */

/**
 * @brief Standard constructor.
 *
 * @param parent Owning Qt widget
 * @param ser_water Pointer to QSerialPort communicating w/ SerialWater Arduino
 * @param debug_mode_ Whether verbose debug text should be output
 */
PumpThread::PumpThread(QObject* parent, bool debug_mode)
    : QThread(parent),
      debug_mode_(debug_mode),
      ser_water_(new QSerialPort(this)) {
    // Set port options (except for PortName)
    ser_water_->setBaudRate(QSerialPort::Baud115200);
    ser_water_->setDataBits(QSerialPort::Data8);
    ser_water_->setParity(QSerialPort::NoParity);
    ser_water_->setStopBits(QSerialPort::OneStop);
    ser_water_->setFlowControl(QSerialPort::NoFlowControl);
}

/**
 * @brief Standard destructor.
 */
PumpThread::~PumpThread() {
    // Close connection to SerialWater Arduino
    if (ser_water_->isOpen()) {
        ser_water_->close();
    }
}

//------------------------------------------------------------------------------
// !Helper Functions
//------------------------------------------------------------------------------

namespace {  // local to this file

/**
 * @brief Converts QBitArray to QByteArray, since Qt doesn't seem to support
 *        this natively.
 *
 * @param bits The QBitArray to convert
 * @return A QByteArray logically equivalent to the input QBitArray
 *
 * @note See the following Stack Overflow thread:
 *   https://stackoverflow.com/questions/8776261/qbitarray-to-qbytearray
 */
QByteArray BitsToBytes(QBitArray bits) {
    // Prepare byte array object
    QByteArray bytes;
    bytes.resize(bits.count() / 8 + 1);
    bytes.fill(0);

    // Convert from QBitArray to QByteArray
    for (int b = 0; b < bits.count(); ++b) {
        bytes[b / 8] = (bytes.at(b / 8) | ((bits[b] ? 1 : 0) << (b % 8)));
    }

    return bytes;
}

/**
 * @brief Converts a QBitArray to QString (for printing), since Qt doesn't seem
 *        to support this natively.
 *
 * @param bits The QBitArray to convert
 * @return A QString representing the input QBitArray
 *
 * @note See the following Stack Overflow thread:
 *   https://stackoverflow.com/questions/46101782/qt-how-do-i-convert-qbitarray-to-qstring
 */
QString BitsToStr(QBitArray bits) {
    QString str;

    for (auto i = bits.size() - 1; i >= 0; --i) {
        str += bits.at(i) ? '1' : '0';
    }

    return str;
}

/**
 * @brief Converts a QByteArray to QString in binary format (for printing),
 *        since Qt doesn't seem to support this natively.
 *
 * @param bytes The QByteArray to convert
 * @return A QString representing the input QBitArray in binary format
 */
QString BytesToStr(QByteArray bytes) {
    QString str;

    for (const auto& byte : bytes)
        for (auto i = 7; i >= 0; --i) {  // hard-coded range of 0-7 = one byte
            str += (byte & (1 << i)) ? '1' : '0';
        }

    return str;
}

}  // namespace

//------------------------------------------------------------------------------
// !Thread Overrides
//------------------------------------------------------------------------------

/**
 * @brief Main pump command loop.
 */
void PumpThread::run() {
    if (debug_mode_) {
        qDebug() << "[DEBUG] Initialized PumpThread";
    }

    // Loop until MainWindow calls QThread::requestInterruption()
    while (!isInterruptionRequested()) {
        if (ser_water_ != nullptr && ser_water_->isOpen()) {
            //  Send data (and verify)
            auto byte_cmd = BitsToBytes(current_state_);
            const qint64 written = ser_water_->write(byte_cmd);
            if (written != byte_cmd.size()) {
                qCritical() << "[ERROR] Failed to write all data to"
                            << ser_water_->portName() << "with error"
                            << ser_water_->errorString();
            }

            if (debug_mode_ && last_state_ != current_state_) {
                qDebug() << "[DEBUG] Sending:" << BitsToStr(current_state_);
            }

            last_state_ = current_state_;
        }

        // Don't overwhelm the network
        QThread::msleep(500);  // ms
    }
}

//------------------------------------------------------------------------------
// !Slots
//------------------------------------------------------------------------------

/**
 * @brief Updates the thread's debug mode.
 *
 * @param enabled Whether verbose debug text should be output
 */
void PumpThread::SetDebugMode(const bool& enabled) {
    debug_mode_ = enabled;
}

/**
 * @brief Attempts to establish a connection.
 *
 * @param port_name The interface to connect to
 */
void PumpThread::OpenConnection(const QString& port_name) {
    // Clear the existing QSerialPort object, if it exists
    if (ser_water_->isOpen()) {
        ser_water_->close();
    }

    // Update port name (other options set in constructor)
    ser_water_->setPortName(port_name);

    // Try to connect
    if (!ser_water_->open(QIODevice::ReadWrite)) {
        qCritical() << "[ERROR] Failed to open port " << ser_water_->portName()
                    << "with error" << ser_water_->errorString();
    }
}

/**
 * @brief Scans all interfaces for serial devices and emits a signal with a
 *        vector of available port names.
 */
void PumpThread::RefreshConnections() {
    if (debug_mode_) {
        qDebug() << "[DEBUG] Refreshing available serial ports...";
    }

    // Populate serial device selector combo box
    std::vector<QString> available;
    const auto ports = QSerialPortInfo::availablePorts();
    for (const auto& port_info : ports) {
        if (debug_mode_) {
            qDebug() << "[DEBUG] Found SerialPort with following metadata\n"
                     << "  Port:" << port_info.portName() << "\n"
                     << "  Location:" << port_info.systemLocation() << "\n"
                     << "  Description:" << port_info.description() << "\n"
                     << "  Manufacturer:" << port_info.manufacturer() << "\n"
                     << "  Serial number:" << port_info.serialNumber();
        }

        // Populate ComboBox (new items are appended to existing list)
        available.push_back(port_info.portName());
    }

    if (available.empty()) {
        qWarning() << "[WARN] No serial devices were found!";
        return;
    }

    emit InformAvailable(available);
}

/**
 * @brief Overrides the command the thread continuously sends to SerialWater.
 *
 * @param state New pump (fluid system) state.
 *
 * @note Bit field "0b1234":
 *         1: A_IN | 2: B_IN | 3: A_OUT | 4: B_OUT
 *       These are indexed backwards when using QBitArray, since it starts from
 *       the least significant bit:
 *         0: B_OUT | 1: A_OUT | 2: B_IN | 3: A_IN
 */
void PumpThread::UpdateState(const State& state) {
    switch (state) {
        case State::kLeftStop:
            current_state_.setBit(1, false);  // A_OUT: off
            current_state_.setBit(3, false);  // A_IN : off
            break;

        case State::kRightStop:
            current_state_.setBit(0, false);  // B_OUT: off
            current_state_.setBit(2, false);  // B_IN : off
            break;

        case State::kLeftFill:
            current_state_.setBit(3, true);   // A_IN : on
            current_state_.setBit(1, false);  // A_OUT: off
            break;

        case State::kRightFill:
            current_state_.setBit(2, true);   // B_IN : on
            current_state_.setBit(0, false);  // B_OUT: off
            break;

        case State::kLeftDrain:
            current_state_.setBit(1, true);   // A_OUT: on
            current_state_.setBit(3, false);  // A_IN : off
            break;

        case State::kRightDrain:
            current_state_.setBit(0, true);   // B_OUT: on
            current_state_.setBit(2, false);  // B_IN : off
            break;

        default:
            // Should never get here
            qCritical() << "[ERROR] Unknown pump state:" << state;
    }
}