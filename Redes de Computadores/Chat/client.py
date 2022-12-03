# ------------------------------------------ Client ----------------------------------------- #
# Autor: Ayrton
# Disciplina: Redes de Computadores
# Data: 03/12/2022
# Descrição: Aplicação cliente para o chat cliente - servidor
# ------------------------------------------- END ------------------------------------------- #
import os
import sys
import json
import socket
import threading
import ipaddress
from contextlib import suppress
from PyQt5 import QtGui
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QIcon, QColor
from PyQt5.QtWidgets import QMainWindow, QApplication, QMessageBox, QListWidget, QLineEdit, QTextEdit, QPushButton, QColorDialog, QListWidgetItem
from PyQt5.uic import loadUi


def resource(relative_path):
    try:
        base_path = sys._MEIPASS
    except Exception:
        base_path = os.path.abspath(".")

    return os.path.join(base_path, relative_path)


class Client(QMainWindow):
    def __init__(self):
        super(Client, self).__init__()
        loadUi(resource("ui/client.ui"), self)
        self.ip = socket.gethostbyname(socket.gethostname())
        self.size = 1024
        self.color = QColor(0, 0, 0)
        self.connected = False
        self.socket = None
        self.list = []
        self.items = {}
        self.history.setVerticalScrollBar(self.scrollbar)
        self.clients.setVerticalScrollBar(self.clientscroll)
        self.colorpicker.clicked.connect(self.onColorPickerClicked)
        self.start.clicked.connect(self.onStartClicked)
        self.message.returnPressed.connect(self.onEnterMessage)
        self.clients.itemClicked.connect(self.onItemClicked)

    def disconnect(self):
        self.socket.close()
        self.connected = False
        self.list.clear()
        self.items.clear()
        self.clients.clear()
        self.server.setReadOnly(False)
        self.port.setReadOnly(False)
        self.name.setReadOnly(False)
        self.start.setIcon(QIcon(QtGui.QPixmap(resource("images/Start.png"))))
        self.status.setPixmap(QtGui.QPixmap(resource("images/Off.png")))
        self.append(self.name.text(), "Você foi desconectado", self.color.name(), "center")

    def connect(self):
        self.connected = True
        self.server.setReadOnly(True)
        self.port.setReadOnly(True)
        self.name.setReadOnly(True)
        self.start.setIcon(QIcon(QtGui.QPixmap(resource("images/Stop.png"))))
        self.status.setPixmap(QtGui.QPixmap(resource("images/On.png")))

    def append(self, name, content, color, alignment):
        if content:
            if name == self.name.text():
                self.history.insertHtml(f'<br/><p align=\"{alignment}\" style=\"color:{color};\">{content}')
            elif name == "Servidor":
                self.history.insertHtml(f'<br/><p align=\"center\" style=\"color:#000000;\">{content}')
            else:
                self.history.insertHtml(f'<br/><p align=\"{alignment}\" style=\"color:{color};\">{name}: {content}')

            self.history.verticalScrollBar().setValue(self.history.verticalScrollBar().maximum())

    def update(self, aList: list):
        if type(aList) == list:
            remove = []

            for i in range(self.clients.count()):
                item = self.clients.item(i)
                if item:
                    if [item.text(), item.toolTip()] not in aList:
                        remove.append(item)
                        with suppress(KeyError):
                            self.items.pop((item.text(), item.toolTip()))
                        with suppress(ValueError):
                            self.list.remove((item.text(), item.toolTip()))

            for item in remove:
                self.clients.takeItem(self.clients.row(item))

            for client in aList:
                client = tuple(client)
                if client not in self.items:
                    item = QListWidgetItem()
                    item.setText(client[0])
                    item.setToolTip(client[1])
                    item.setFlags(item.flags() | Qt.ItemIsUserCheckable)
                    item.setCheckState(Qt.Unchecked)
                    item.setForeground(Qt.white)
                    self.clients.addItem(item)
                    self.items[client] = item

    def send(self, content, flag):
        if (self.connected or flag == "CONNECT") and self.socket:
            message = {
                "ip": self.ip,
                "type": flag,
                "name": self.name.text().strip(),
                "color": self.color.name(),
                "content": content,
                "clients": self.list
            }

            try:
                self.socket.send(json.dumps(message).encode())
            except:
                self.append(None, 'Não foi possível enviar a mensagem para o servidor', "#FF0000", "center")

    def receive(self):
        self.send(None, "CONNECT")

        while True:
            try:
                message = json.loads(self.socket.recv(self.size))

                if message["type"] == "CONNECTED":
                    self.connect()
                    self.update(message["clients"])
                elif message["type"] == "MESSAGE":
                    self.append(message["name"], message["content"], message["color"], "left")
                elif message["type"] == "UPDATE":
                    self.update(message["clients"])
                elif message["type"] == "DISCONNECTED":
                    self.disconnect()
                    break
            except:
                self.disconnect()
                break

    def validade(self, ip, port, name) -> bool:
        if not ip:
            QMessageBox.warning(self, "Error!", "Server IP é obrigatório.")
            self.server.setFocus()
            return False
        else:
            try:
                ipaddress.ip_address(ip)
            except ValueError:
                QMessageBox.warning(self, "Error!", f"O IP {ip} é inválido.")
                self.server.setFocus()
                return False

        if not port:
            QMessageBox.warning(self, "Error!", "Port é obrigatório.")
            self.port.setFocus()
            return False
        else:
            try:
                if not (1 <= int(port) <= 65535):
                    raise ValueError
            except ValueError:
                QMessageBox.warning(self, "Error!", f"A Porta {port} é inválida.")
                self.port.setFocus()
                return False

        if not name:
            QMessageBox.warning(self, "Error!", "Nome é obrigatório.")
            self.name.setFocus()
            return False

        return True

    def onItemClicked(self, item):
        with suppress(ValueError):
            if item.checkState() == Qt.Checked:
                item.setCheckState(Qt.Unchecked)
                self.list.remove((item.text(), item.toolTip()))
            else:
                item.setCheckState(Qt.Checked)
                if (item.text(), item.toolTip()) not in self.list:
                    self.list.append((item.text(), item.toolTip()))

    def onStartClicked(self):
        if not self.connected:
            if self.validade(self.server.text().strip(), self.port.text().strip(), self.name.text().strip()):
                try:
                    self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    self.socket.connect((self.server.text().strip(), int(self.port.text().strip())))
                    threading.Thread(target=self.receive).start()
                except:
                    QMessageBox.warning(self, "Error!", "Houve um problema ao se conectar ao servidor.")
            else:
                return
        else:
            self.send(None, "DISCONNECT")
            self.list.clear()
            self.items.clear()
            self.clients.clear()

    def onColorPickerClicked(self):
        self.color = QColorDialog.getColor()

        if self.color.isValid():
            self.colorpicker.setStyleSheet(f"QPushButton "
                                           f"{{ "
                                           f"     border: 2px solid #61AFEF; "
                                           f"     border-radius: 10px; "
                                           f"     background-color: {self.color.name()}; "
                                           f"}} "
                                           f""
                                           f"QPushButton: pressed "
                                           f"{{ "
                                           f"     border-style: inset; "
                                           f"}}")

    def onEnterMessage(self):
        self.append(self.name.text(), self.message.text().strip(), self.color.name(), "right")
        self.send(self.message.text().strip(), "MESSAGE")
        self.message.clear()

    def closeEvent(self, event):
        if self.connected:
            self.send(None, "DISCONNECT")
            self.disconnect()


app = QApplication(sys.argv)
window = Client()
window.show()
sys.exit(app.exec_())
