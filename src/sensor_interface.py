import sys
import socket
import csv
from datetime import datetime, timedelta
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QPushButton, QLabel, QFileDialog
from PyQt5.QtCore import QTimer, Qt
import pyqtgraph as pg

class TimeAxisItem(pg.AxisItem):
    def tickStrings(self, values, scale, spacing):
        return [""] * len(values)

class SensorUI(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Interface do Sensor IR")
        self.resize(900, 500)
        self.UDP_IP = "0.0.0.0"
        self.UDP_PORT = 5000
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind((self.UDP_IP, self.UDP_PORT))
        self.sock.setblocking(False)
        self.values = []
        self.timestamps = []
        self.statuses = []
        self.running = False
        self.start_time = None
        self.time_window = 30
        layout = QVBoxLayout()
        self.label = QLabel("Aguardando Dados...")
        self.label.setStyleSheet("font-size: 18px;")
        layout.addWidget(self.label)
        self.state_label = QLabel("---")
        self.state_label.setAlignment(Qt.AlignCenter)
        self.state_label.setStyleSheet("font-size: 22px; font-weight: bold; color: gray;")
        layout.addWidget(self.state_label)
        self.time_axis = TimeAxisItem(orientation='bottom')
        self.plot_widget = pg.PlotWidget(title="Dados do Sensor", axisItems={'bottom': self.time_axis})
        self.plot_widget.setLabel('left', 'Valor do ADC')
        self.plot_widget.setLabel('bottom', 'Tempo')
        self.plot_widget.showGrid(x=True, y=True, alpha=0.3)
        self.curve = self.plot_widget.plot(pen=pg.mkPen('g', width=2))
        layout.addWidget(self.plot_widget)
        self.start_btn = QPushButton("Iniciar")
        self.stop_btn = QPushButton("Parar")
        self.save_btn = QPushButton("Salvar")
        layout.addWidget(self.start_btn)
        layout.addWidget(self.stop_btn)
        layout.addWidget(self.save_btn)
        self.setLayout(layout)
        self.start_btn.clicked.connect(self.start)
        self.stop_btn.clicked.connect(self.stop)
        self.save_btn.clicked.connect(self.save_to_csv)
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_data)
        self.timer.setInterval(200)
        self.plot_widget.setXRange(0, 30, padding=0)

    def start(self):
        self.running = True
        self.label.setText("Recebendo dados...")
        self.timer.start()
        self.start_time = datetime.now()

    def stop(self):
        self.running = False
        self.label.setText("Parado")
        self.state_label.setText("---")
        self.state_label.setStyleSheet("font-size: 22px; font-weight: bold; color: gray;")
        self.timer.stop()

    def update_data(self):
        if not self.running:
            return
        try:
            data, _ = self.sock.recvfrom(1024)
            msg = data.decode().strip().replace('\n', '')
            if "ADC" in msg and "STATUS" in msg:
                try:
                    val = int(msg.split("ADC =")[1].split('|')[0].strip())
                    status = msg.split("STATUS =")[1].strip().upper()
                except (IndexError, ValueError):
                    return
                now = datetime.now()
                t_sec = (now - self.start_time).total_seconds()
                self.values.append(val)
                self.timestamps.append(t_sec)
                self.statuses.append(status)
                while self.timestamps and self.timestamps[-1] - self.timestamps[0] > self.time_window:
                    self.timestamps.pop(0)
                    self.values.pop(0)
                    self.statuses.pop(0)
                if status == "OBSTRUCTED":
                    pen = pg.mkPen('r', width=2)
                    self.state_label.setText("OBSTRUCTED")
                    self.state_label.setStyleSheet("font-size: 22px; font-weight: bold; color: red;")
                else:
                    pen = pg.mkPen('g', width=2)
                    self.state_label.setText("CLEAR")
                    self.state_label.setStyleSheet("font-size: 22px; font-weight: bold; color: green;")
                self.curve.setPen(pen)
                x = [t - self.timestamps[0] for t in self.timestamps]
                self.curve.setData(x, self.values)
                self.plot_widget.setXRange(0, 30, padding=0)
                self.label.setText(f"Status: {status}  |  ADC = {val}")
        except BlockingIOError:
            pass

    def save_to_csv(self):
        path, _ = QFileDialog.getSaveFileName(self, "Salvar", "", "CSV Files (*.csv)")
        if not path:
            return
        with open(path, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(["Tempo (s)", "ADC", "Status"])
            for t, v, s in zip(self.timestamps, self.values, self.statuses):
                writer.writerow([t, v, s])
        self.label.setText(f"Saved to: {path}")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = SensorUI()
    window.show()
    sys.exit(app.exec_())
