import sys
import argparse
from collections import deque

import serial
import serial.tools.list_ports

import pyqtgraph as pg

from PySide6.QtCore import QTimer
from PySide6.QtWidgets import (
    QApplication,
    QMainWindow,
    QWidget,
    QVBoxLayout,
    QHBoxLayout,
    QLabel,
    QPushButton,
    QComboBox,
    QSpinBox,
    QPlainTextEdit,
)


class I2CVisualizer(QMainWindow):

    def __init__(self, port, baud, sample_rate):
        super().__init__()

        self.setWindowTitle("STM32 I2C Logic Analyzer")
        self.resize(1200, 850)

        self.port_name = port
        self.baud_rate = baud
        self.sample_rate = sample_rate

        self.serial_port = None
        self.rx_buffer = bytearray()

        self.total_samples = 0
        self.total_records = 0
        self.discarded_bytes = 0

        self.previous_state = None

        # Diagnostic counters
        self.state_counts = [0, 0, 0, 0]
        self.total_run_samples = 0
        self.min_run = None
        self.max_run = 0
        self.debug_records = 0

        # I2C decoder state
        self.i2c_active = False
        self.i2c_current_byte = 0
        self.i2c_bit_count = 0
        self.i2c_byte_index = 0

        self.i2c_transaction_count = 0
        self.i2c_start_count = 0
        self.i2c_stop_count = 0

        # Keep waveform memory bounded.
        self.max_points = 20000

        self.scl_x = deque()
        self.scl_y = deque()

        self.sda_x = deque()
        self.sda_y = deque()

        self.setup_ui()

        # Serial reading timer.
        self.timer = QTimer()
        self.timer.timeout.connect(self.read_serial)
        self.timer.start(5)

    # --------------------------------------------------
    # GUI
    # --------------------------------------------------

    def setup_ui(self):

        central = QWidget()
        self.setCentralWidget(central)

        layout = QVBoxLayout(central)

        # Connection controls
        controls = QHBoxLayout()

        self.port_combo = QComboBox()
        self.refresh_ports()

        self.baud_spin = QSpinBox()
        self.baud_spin.setRange(1200, 3000000)
        self.baud_spin.setValue(self.baud_rate)

        self.connect_button = QPushButton("Connect")
        self.connect_button.clicked.connect(
            self.toggle_connection
        )

        self.clear_button = QPushButton("Clear")
        self.clear_button.clicked.connect(
            self.clear_graph
        )

        controls.addWidget(QLabel("Serial port:"))
        controls.addWidget(self.port_combo)

        controls.addWidget(QLabel("Baud rate:"))
        controls.addWidget(self.baud_spin)

        controls.addWidget(self.connect_button)
        controls.addWidget(self.clear_button)

        layout.addLayout(controls)

        # Status
        self.status_label = QLabel("Disconnected")

        self.statistics_label = QLabel(
            "Samples: 0 | Records: 0"
        )

        layout.addWidget(self.status_label)
        layout.addWidget(self.statistics_label)

        # Waveform plot
        self.plot_widget = pg.PlotWidget()
        self.plot_widget.setBackground("#151515")

        self.plot_widget.setLabel(
            "bottom",
            "Sample Number"
        )

        self.plot_widget.setYRange(
            -0.5, 4.5, padding=0
        )

        self.plot_widget.setXRange(
            0, 10000, padding=0
        )

        self.plot_widget.setMouseEnabled(
            x=True,
            y=False
        )

        self.plot_widget.showGrid(
            x=True,
            y=True,
            alpha=0.25
        )

        # One label per signal track.
        self.plot_widget.getAxis("left").setTicks([
            [
                (0.5, "SCL"),
                (3.5, "SDA"),
            ]
        ])

        layout.addWidget(self.plot_widget)

        # Exactly TWO waveform curves.
        #
        # SCL: Y = 0 (LOW), Y = 1 (HIGH)
        # SDA: Y = 3 (LOW), Y = 4 (HIGH)

        self.scl_curve = self.plot_widget.plot(
            pen=pg.mkPen("#00D5FF", width=2)
        )

        self.sda_curve = self.plot_widget.plot(
            pen=pg.mkPen("#FFB000", width=2)
        )

        # Separator between signal tracks.
        self.plot_widget.addItem(
            pg.InfiniteLine(
                pos=2,
                angle=0,
                pen=pg.mkPen(
                    "#555555",
                    style=pg.QtCore.Qt.PenStyle.DashLine
                )
            )
        )

        # ------------------------------------------
        # I2C decoded output
        # ------------------------------------------

        layout.addWidget(
            QLabel("Decoded I2C Transactions")
        )

        self.i2c_output = QPlainTextEdit()
        self.i2c_output.setReadOnly(True)
        self.i2c_output.setMaximumBlockCount(1000)
        self.i2c_output.setMaximumHeight(200)

        layout.addWidget(self.i2c_output)

    # --------------------------------------------------
    # Serial connection
    # --------------------------------------------------

    def refresh_ports(self):

        self.port_combo.clear()

        ports = serial.tools.list_ports.comports()

        for port in ports:
            self.port_combo.addItem(port.device)

        if self.port_combo.count() == 0:
            self.port_combo.addItem("/dev/ttyACM0")

    def toggle_connection(self):

        if self.serial_port is not None:
            self.disconnect_serial()
        else:
            self.connect_serial()

    def connect_serial(self):

        self.port_name = self.port_combo.currentText()
        self.baud_rate = self.baud_spin.value()

        try:

            self.serial_port = serial.Serial(
                port=self.port_name,
                baudrate=self.baud_rate,
                timeout=0
            )

            self.rx_buffer.clear()

            self.connect_button.setText("Disconnect")

            self.status_label.setText(
                f"Connected: {self.port_name}"
            )

        except serial.SerialException as error:

            self.serial_port = None

            self.status_label.setText(
                f"Connection error: {error}"
            )

    def disconnect_serial(self):

        if self.serial_port is not None:
            self.serial_port.close()
            self.serial_port = None

        self.connect_button.setText("Connect")
        self.status_label.setText("Disconnected")

    # --------------------------------------------------
    # RLE decoder
    # --------------------------------------------------

    def decode_record(self, first, second, third):

        # BYTE 1:
        # Bit 7     = HEAD FLAG (1)
        # Bits 6:2  = COUNT bits 0-4
        # Bits 1:0  = STATE
        #
        # BYTE 2:
        # Bit 7     = CONTINUATION FLAG (0)
        # Bits 6:0  = COUNT bits 5-11
        #
        # BYTE 3:
        # Bit 7     = CONTINUATION FLAG (0)
        # Bits 6:0  = COUNT bits 12-18

        state = first & 0x03

        count = (
            ((first >> 2) & 0x1F)
            | ((second & 0x7F) << 5)
            | ((third & 0x7F) << 12)
        )

        return state, count

    # --------------------------------------------------
    # Serial reading and framing
    # --------------------------------------------------

    def read_serial(self):

        if self.serial_port is None:
            return

        try:

            waiting = self.serial_port.in_waiting

            if waiting == 0:
                return

            incoming = self.serial_port.read(waiting)

            self.rx_buffer.extend(incoming)

            records_processed = 0

            while len(self.rx_buffer) >= 3:

                first = self.rx_buffer[0]
                second = self.rx_buffer[1]
                third = self.rx_buffer[2]

                # First byte MUST have bit 7 = 1.
                if (first & 0x80) == 0:

                    del self.rx_buffer[0]
                    self.discarded_bytes += 1
                    continue

                # Continuation bytes MUST have bit 7 = 0.
                if (second & 0x80) or (third & 0x80):

                    del self.rx_buffer[0]
                    self.discarded_bytes += 1
                    continue

                # Valid three-byte record.
                del self.rx_buffer[:3]

                state, count = self.decode_record(
                    first,
                    second,
                    third
                )

                if count == 0:
                    continue

                self.process_run(state, count)

                self.total_records += 1
                records_processed += 1

            if records_processed > 0:
                self.update_plot()

        except serial.SerialException as error:

            self.status_label.setText(
                f"Serial error: {error}"
            )

            self.disconnect_serial()

    # --------------------------------------------------
    # I2C protocol decoder
    # --------------------------------------------------

    def log_i2c(self, message):

        self.i2c_output.appendPlainText(message)

        print(message)

    def reset_i2c_byte(self):

        self.i2c_current_byte = 0
        self.i2c_bit_count = 0

    def start_i2c_transaction(self, repeated=False):

        self.i2c_active = True

        self.i2c_byte_index = 0
        self.reset_i2c_byte()

        if repeated:

            self.log_i2c("\nREPEATED START")

        else:

            self.i2c_transaction_count += 1

            self.log_i2c(
                f"\n--- I2C TRANSACTION "
                f"{self.i2c_transaction_count} ---"
            )

        self.i2c_start_count += 1

    def stop_i2c_transaction(self):

        self.log_i2c("STOP")

        self.i2c_stop_count += 1

        self.i2c_active = False

        self.reset_i2c_byte()

    def decode_i2c_bit(self, sda):

        # First eight clocks: data bits.
        # I2C transmits MSB first.

        if self.i2c_bit_count < 8:

            self.i2c_current_byte = (
                (self.i2c_current_byte << 1) | sda
            )

            self.i2c_bit_count += 1

            return

        # Ninth clock: ACK/NACK.
        #
        # ACK  = SDA LOW
        # NACK = SDA HIGH

        ack = (sda == 0)

        if self.i2c_byte_index == 0:

            # First byte:
            # Bits 7:1 = address
            # Bit 0    = read/write

            address = (
                self.i2c_current_byte >> 1
            ) & 0x7F

            read_write = (
                self.i2c_current_byte & 0x01
            )

            direction = (
                "READ" if read_write else "WRITE"
            )

            acknowledgement = (
                "ACK" if ack else "NACK"
            )

            self.log_i2c(
                f"ADDRESS: 0x{address:02X} | "
                f"{direction} | {acknowledgement}"
            )

        else:

            acknowledgement = (
                "ACK" if ack else "NACK"
            )

            self.log_i2c(
                f"DATA[{self.i2c_byte_index - 1}]: "
                f"0x{self.i2c_current_byte:02X} | "
                f"{acknowledgement}"
            )

        self.i2c_byte_index += 1

        self.reset_i2c_byte()

    def process_i2c_transition(self, state):

        if self.previous_state is None:
            return

        previous_scl = self.previous_state & 0x01

        previous_sda = (
            (self.previous_state >> 1) & 0x01
        )

        current_scl = state & 0x01

        current_sda = (
            (state >> 1) & 0x01
        )

        # ------------------------------------------
        # START / STOP detection
        # ------------------------------------------

        # SDA changes while SCL remains HIGH.
        #
        # SDA falling = START
        # SDA rising  = STOP

        if previous_scl == 1 and current_scl == 1:

            if previous_sda == 1 and current_sda == 0:

                self.start_i2c_transaction(
                    repeated=self.i2c_active
                )

                return

            elif previous_sda == 0 and current_sda == 1:

                if self.i2c_active:
                    self.stop_i2c_transaction()

                return

        # ------------------------------------------
        # SCL rising edge
        # ------------------------------------------

        if (
            self.i2c_active
            and previous_scl == 0
            and current_scl == 1
        ):

            self.decode_i2c_bit(current_sda)

    # --------------------------------------------------
    # Waveform reconstruction + diagnostics
    # --------------------------------------------------

    def process_run(self, state, count):

        # Diagnostic counters
        self.state_counts[state] += 1
        self.total_run_samples += count

        if self.min_run is None or count < self.min_run:
            self.min_run = count

        if count > self.max_run:
            self.max_run = count

        # Print first 30 decoded records.
        if self.debug_records < 30:

            print(
                f"Record {self.debug_records}: "
                f"state={state:02b}, "
                f"SCL={state & 1}, "
                f"SDA={(state >> 1) & 1}, "
                f"count={count}"
            )

            self.debug_records += 1

        # ------------------------------------------
        # I2C decoder
        # ------------------------------------------

        self.process_i2c_transition(state)

        start = self.total_samples
        end = start + count

        # State mapping:
        # Bit 0 = SCL
        # Bit 1 = SDA

        scl = state & 0x01
        sda = (state >> 1) & 0x01

        # Separate waveform tracks.
        scl_y = scl
        sda_y = sda + 3

        if self.previous_state is None:

            self.scl_x.append(start)
            self.scl_y.append(scl_y)

            self.sda_x.append(start)
            self.sda_y.append(sda_y)

        else:

            previous_scl = self.previous_state & 0x01

            previous_sda = (
                (self.previous_state >> 1) & 0x01
            )

            previous_scl_y = previous_scl
            previous_sda_y = previous_sda + 3

            # SCL transition.
            if scl != previous_scl:

                self.scl_x.append(start)
                self.scl_y.append(previous_scl_y)

                self.scl_x.append(start)
                self.scl_y.append(scl_y)

            # SDA transition.
            if sda != previous_sda:

                self.sda_x.append(start)
                self.sda_y.append(previous_sda_y)

                self.sda_x.append(start)
                self.sda_y.append(sda_y)

        # Extend both signals horizontally to the end.
        self.scl_x.append(end)
        self.scl_y.append(scl_y)

        self.sda_x.append(end)
        self.sda_y.append(sda_y)

        self.previous_state = state
        self.total_samples = end

        # Prevent unlimited memory growth.
        while len(self.scl_x) > self.max_points:
            self.scl_x.popleft()
            self.scl_y.popleft()

        while len(self.sda_x) > self.max_points:
            self.sda_x.popleft()
            self.sda_y.popleft()

    # --------------------------------------------------
    # Plot update
    # --------------------------------------------------

    def update_plot(self):

        # SCL waveform.
        self.scl_curve.setData(
            list(self.scl_x),
            list(self.scl_y)
        )

        # SDA waveform.
        self.sda_curve.setData(
            list(self.sda_x),
            list(self.sda_y)
        )

        duration = (
            self.total_samples / self.sample_rate
        )

        self.statistics_label.setText(
            f"Samples: {self.total_samples:,} | "
            f"Records: {self.total_records:,} | "
            f"States: 00={self.state_counts[0]} "
            f"01={self.state_counts[1]} "
            f"10={self.state_counts[2]} "
            f"11={self.state_counts[3]} | "
            f"Min run: {self.min_run} | "
            f"Max run: {self.max_run} | "
            f"Discarded: {self.discarded_bytes} | "
            f"STARTs: {self.i2c_start_count} | "
            f"STOPs: {self.i2c_stop_count}"
        )

        # Follow newest samples.
        if self.total_samples > 10000:

            self.plot_widget.setXRange(
                max(0, self.total_samples - 10000),
                self.total_samples,
                padding=0
            )

    # --------------------------------------------------
    # Clear
    # --------------------------------------------------

    def clear_graph(self):

        self.total_samples = 0
        self.total_records = 0
        self.discarded_bytes = 0

        self.previous_state = None

        self.state_counts = [0, 0, 0, 0]
        self.total_run_samples = 0
        self.min_run = None
        self.max_run = 0
        self.debug_records = 0

        # Reset I2C decoder.
        self.i2c_active = False
        self.i2c_current_byte = 0
        self.i2c_bit_count = 0
        self.i2c_byte_index = 0

        self.i2c_transaction_count = 0
        self.i2c_start_count = 0
        self.i2c_stop_count = 0

        self.i2c_output.clear()

        self.rx_buffer.clear()

        self.scl_x.clear()
        self.scl_y.clear()

        self.sda_x.clear()
        self.sda_y.clear()

        self.scl_curve.clear()
        self.sda_curve.clear()

        self.statistics_label.setText(
            "Samples: 0 | Records: 0"
        )

        self.plot_widget.setXRange(
            0, 10000, padding=0
        )

    # --------------------------------------------------
    # Shutdown
    # --------------------------------------------------

    def closeEvent(self, event):

        self.disconnect_serial()
        event.accept()


# ------------------------------------------------------
# Main
# ------------------------------------------------------

def main():

    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--port",
        default="/dev/ttyACM0"
    )

    parser.add_argument(
        "--baud",
        type=int,
        default=921600
    )

    parser.add_argument(
        "--sample-rate",
        type=int,
        default=2000000
    )

    args = parser.parse_args()

    app = QApplication(sys.argv)

    window = I2CVisualizer(
        args.port,
        args.baud,
        args.sample_rate
    )

    window.show()

    sys.exit(app.exec())


if __name__ == "__main__":
    main()