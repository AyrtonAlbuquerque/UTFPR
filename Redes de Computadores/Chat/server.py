# ------------------------------------------ Server ----------------------------------------- #
# Autor: Ayrton
# Disciplina: Redes de Computadores
# Data: 03/12/2022
# Descrição: Aplicação servidor para o chat cliente - servidor
# ------------------------------------------- END ------------------------------------------- #
import os
import sys
import json
import socket
import threading
from contextlib import suppress
from PyQt5 import QtGui
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QIcon, QColor
from PyQt5.QtWidgets import QMainWindow, QApplication, QMessageBox, QListWidget, QLineEdit, QTextEdit, QPushButton, QColorDialog, QListWidgetItem, QWidget, QHBoxLayout
from PyQt5.uic import loadUi


def resource(relative_path):
    try:
        base_path = sys._MEIPASS
    except Exception:
        base_path = os.path.abspath(".")

    return os.path.join(base_path, relative_path)


class Server(QMainWindow):
    def __init__(self):
        super(Server, self).__init__()
        loadUi(resource("ui/server.ui"), self)
        self.ip = socket.gethostbyname(socket.gethostname())
        self.size = 1024
        self.color = QColor(0, 0, 0)
        self.running = False
        self.socket: None
        self.list = []
        self.kicked = []
        self.selected = []
        self.map = {}
        self.items = {}
        self.connections = {}
        self.server.setText(self.ip)
        self.history.setVerticalScrollBar(self.scrollbar)
        self.clients.setVerticalScrollBar(self.clientscroll)
        self.colorpicker.clicked.connect(self.onColorPickerClicked)
        self.start.clicked.connect(self.onStartClicked)
        self.message.returnPressed.connect(self.onEnterMessage)
        self.clients.itemClicked.connect(self.onItemClicked)
        self.kick.clicked.connect(self.onKickClicked)

    def broadcast(self, message, name):
        if message:
            clients = self.connections.copy()
            for client in clients.values():
                try:
                    if client not in self.kicked:
                        self.send(client, "MESSAGE", name, self.color.name(), message, None)
                except:
                    self.append(None, f'Não foi possível enviar a mensagem para o cliente {client}', "#000000", "center")

    def append(self, name, content, color, alignment):
        if content:
            if name:
                self.history.insertHtml(f'<br/><p align=\"{alignment}\" style=\"color:{color};\">{name}: {content}')
            else:
                self.history.insertHtml(f'<br/><p align=\"{alignment}\" style=\"color:{color};\">{content}')

            self.history.verticalScrollBar().setValue(self.history.verticalScrollBar().maximum())

    def update(self, excludes: list):
        clients = self.connections.copy()

        for client in clients.values():
            if client not in excludes:
                with suppress(ValueError):
                    aList = self.list.copy()
                    aList.remove(self.map[client])
                    self.send(client, "UPDATE", None, None, None, aList)

        for client in excludes:
            if client in self.items:
                self.clients.takeItem(self.clients.row(self.items[client]))
                with suppress(KeyError):
                    self.items.pop(client)
            else:
                item = QListWidgetItem()
                item.setText(self.map[client][0])
                item.setToolTip(self.map[client][1])
                item.setFlags(item.flags() | Qt.ItemIsUserCheckable)
                item.setCheckState(Qt.Unchecked)
                item.setForeground(Qt.white)
                self.clients.addItem(item)
                self.items[client] = item

    def send(self, client, type, name, color, content, aList):
        if self.running and client:
            message = {
                "type": type,
                "name": name,
                "color": color,
                "content": content,
                "clients": aList
            }

            try:
                if client not in self.kicked:
                    client.send(json.dumps(message).encode())
            except:
                self.append(None, f'Não foi possível enviar a mensagem para o cliente {client}', "#000000", "center")

    def receive(self, client, address):
        while self.running:
            try:
                message = json.loads(client.recv(self.size))

                if message["type"] == "CONNECT":
                    self.send(client, "CONNECTED", None, None, None, self.list)
                    self.map[client] = (message["name"], message["ip"])
                    self.list.append((message["name"], message["ip"]))
                    with suppress(ValueError):
                        self.kicked.remove(client)
                    self.update([client])
                    self.broadcast(f'{message["name"]} entrou', "Servidor")
                    self.append(None, f'{message["name"]} entrou', "#000000", "center")
                elif message["type"] == "MESSAGE":
                    self.append(None, f'De {message["name"]} Para {message["clients"]}: {message["content"]}', message["color"], "left")
                    for user in message["clients"]:
                        for key, value in self.map.items():
                            if value == tuple(user):
                                self.send(key, "MESSAGE", message["name"], message["color"], message["content"], message["clients"])
                elif message["type"] == "DISCONNECT":
                    with suppress(ValueError, KeyError):
                        self.list.remove(self.map.pop(client))
                    with suppress(ValueError):
                        self.selected.remove((message["name"], message["ip"]))
                    with suppress(KeyError):
                        self.connections.pop(address)
                    self.send(client, "DISCONNECTED", None, None, None, None)
                    self.update([client])
                    self.broadcast(f'{message["name"]} saiu', "Servidor")
                    self.append(None, f'{message["name"]} saiu', "#000000", "center")
                    break
            except:
                with suppress(ValueError, KeyError):
                    self.list.remove(self.map.pop(client))
                with suppress(ValueError, UnboundLocalError):
                    self.selected.remove((message["name"], message["ip"]))
                with suppress(KeyError):
                    self.connections.pop(address)
                client.close()
                if not self.selected:
                    self.message.setPlaceholderText("Broadcast")
                break

    def connection(self):
        while True:
            try:
                client, address = self.socket.accept()
                self.connections[address] = client
                threading.Thread(target=self.receive, args=(client, address,)).start()
            except:
                self.running = False
                self.append(None, "O servidor foi desligado", "#000000", "center")
                self.start.setIcon(QIcon(QtGui.QPixmap(resource("images/Start.png"))))
                self.status.setPixmap(QtGui.QPixmap(resource("images/Off.png")))
                break

    def begin(self):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.bind((self.ip, int(self.port.text().strip())))
        self.socket.listen()
        self.start.setIcon(QIcon(QtGui.QPixmap(resource("images/Stop.png"))))
        self.status.setPixmap(QtGui.QPixmap(resource("images/On.png")))
        self.running = True
        self.append(None, f"O servidor está escutando em {self.ip}:{self.port.text()}", "#000000", "center")
        threading.Thread(target=self.connection).start()

    def stop(self):
        clients = self.connections.copy()
        for client in clients.values():
            self.send(client, "DISCONNECTED", None, None, None, None)
        self.list.clear()
        self.map.clear()
        self.items.clear()
        self.kicked.clear()
        self.clients.clear()
        self.selected.clear()
        self.connections.clear()
        self.socket.close()

    def validade(self, port) -> bool:
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

        return True

    def onKickClicked(self):
        if self.selected:
            excludes = []
            kicked = self.selected.copy()
            clients = self.map.copy()

            for user in kicked:
                for client, value in clients.items():
                    if value == user:
                        with suppress(ValueError, KeyError):
                            self.list.remove(self.map.pop(client))
                        excludes.append(client)
                        self.send(client, "DISCONNECTED", None, None, None, None)
                        self.kicked.append(client)
                self.broadcast(f'{user[0]} foi kickado', "Servidor")
                self.append(None, f'{user[0]} foi kickado', "#000000", "center")
            self.update(excludes)

    def onItemClicked(self, item):
        with suppress(ValueError):
            if item.checkState() == Qt.Checked:
                item.setCheckState(Qt.Unchecked)
                self.selected.remove((item.text(), item.toolTip()))
            else:
                item.setCheckState(Qt.Checked)
                if (item.text(), item.toolTip()) not in self.selected:
                    self.selected.append((item.text(), item.toolTip()))

        if self.selected:
            text = ""
            for user in self.selected:
                text = text + user[0] + ", "
            self.message.setPlaceholderText('Para: ' + ''.join(text.rsplit(',', 1)))
        else:
            self.message.setPlaceholderText("Broadcast")

    def onStartClicked(self):
        if not self.running:
            if self.validade(self.port.text().strip()):
                try:
                    self.begin()
                except:
                    QMessageBox.warning(self, "Error!", "Houve um problema ao inicializar o servidor")
            else:
                return
        else:
            self.stop()

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
        self.append(None, self.message.text().strip(), self.color.name(), "right")

        if self.selected:
            for user in self.selected:
                for client, value in self.map.items():
                    if value == user:
                        self.send(client, "MESSAGE", "Administrador", self.color.name(), self.message.text().strip(), None)
        else:
            self.broadcast(self.message.text().strip(), "Administrador")

        self.message.clear()

    def closeEvent(self, event):
        if self.running:
            self.stop()


app = QApplication(sys.argv)
window = Server()
window.show()
sys.exit(app.exec_())
