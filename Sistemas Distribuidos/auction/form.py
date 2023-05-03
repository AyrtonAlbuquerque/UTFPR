import os
import re
import sys
from client import Client
from datetime import datetime
from PyQt6 import QtGui
from PyQt6.QtCore import Qt
from PyQt6.QtGui import QColor
from PyQt6.QtWidgets import QMainWindow, QApplication, QMessageBox, QListWidgetItem
from PyQt6.uic import loadUi


def resource(relative_path):
    try:
        base_path = sys._MEIPASS
    except Exception:
        base_path = os.path.abspath(".")

    return os.path.join(base_path, relative_path)


class Form(QMainWindow):
    def __init__(self):
        super(Form, self).__init__()
        loadUi(resource("ui/form.ui"), self)
        self.items = []
        self.client = Client(self)
        self.login = Login(self, self.client)
        self.notifications.setVerticalScrollBar(self.scrollbar)
        self.itemList.setVerticalScrollBar(self.itemScrollbar)
        self.itemList.itemClicked.connect(self.item_clicked)
        self.listButton.clicked.connect(self.update_list)
        self.bidButton.clicked.connect(self.bid_clicked)
        self.registerButton.clicked.connect(self.register_clicked)
        self.hide()
        self.login.show()

    def append(self, message: str):
        if message:
            self.notifications.insertHtml(f'<br/><p align=\"center\" style=\"color:#000000;\">{message}')
            self.notifications.verticalScrollBar().setValue(self.notifications.verticalScrollBar().maximum())

    def notify(self, message: str):
        self.append(message)

    def update_list(self):
        try:
            self.items.clear()
            self.itemList.clear()
            self.items = self.client.list_auctions()

            for item in self.items:
                widget = QListWidgetItem()
                widget.setText(f"{item['id']} - {item['name']}\n{item['description']}\nR$ {item['value']}\nExpires at {item['expiration']}")
                widget.setToolTip(str(item["id"]))
                widget.setFlags(widget.flags() | Qt.ItemFlag.ItemIsUserCheckable)

                if item["owner"] == self.client.uri:
                    widget.setForeground(QColor("#e5c07b"))
                    widget.setCheckState(Qt.CheckState.Checked)
                elif self.client.uri in item["biders"]:
                    widget.setForeground(QColor("#98c379"))
                    widget.setCheckState(Qt.CheckState.Checked)
                else:
                    widget.setForeground(QColor("#abb2bf"))
                    widget.setCheckState(Qt.CheckState.Unchecked)

                self.itemList.addItem(widget)
        except:
            QMessageBox.critical(self, "Error!", "Failed to update list. Is the server running?")

    def bid_clicked(self):
        id = self.itemId.text().strip()
        value = self.bidValue.text().strip().replace(",", ".")

        if not id:
            QMessageBox.critical(self, "Error!", "Item ID is required.")
            return
        else:
            try:
                id = int(id)
            except ValueError:
                QMessageBox.critical(self, "Error!", f"{id} is an invalid Item ID.")
                return

        if not value:
            QMessageBox.critical(self, "Error!", "Bid value is required.")
            return
        else:
            value = re.match(r'\d+(?:\.\d{1,2})?$', value)
            if not value:
                QMessageBox.critical(self, "Error!", f"{self.bidValue.text().strip()} is an invalid bid value.")
                return

        try:
            self.append(self.client.bid(id, float(value.string)))
            self.update_list()
        except:
            QMessageBox.critical(self, "Error!", "Failed to bid. Is the server running?")

    def register_clicked(self):
        self.window = Register(self, self.client)

    def item_clicked(self, item: QListWidgetItem):
        item.setCheckState(Qt.CheckState.Checked)
        self.itemList.clearSelection()

        try:
            self.append(self.client.subscribe(int(item.toolTip())))
            self.update_list()
        except:
            QMessageBox.critical(self, "Error!", "Failed to subscribe to item. Is the server running?")


class Login(QMainWindow):
    def __init__(self, parent: Form, client: Client):
        super(Login, self).__init__()
        loadUi(resource("ui/login.ui"), self)
        self.client = client
        self.parent = parent
        self.button.clicked.connect(self.login_clicked)

    def login_clicked(self):
        name = self.name.text().strip()

        if name:
            try:
                if self.client.login(name):
                    self.hide()
                    self.parent.show()
                    self.parent.update_list()
                else:
                    QMessageBox.critical(self, "Error!", "Failed to login. Is the server running?")
            except:
                QMessageBox.critical(self, "Error!", "Failed to login. Is the server running?")
        else:
            QMessageBox.critical(self, "Error!", "You must provide your name to login.")

    def closeEvent(self, event: QtGui.QCloseEvent) -> None:
        if not self.client.logged:
            self.parent.close()


class Register(QMainWindow):
    def __init__(self, parent: Form, client: Client):
        super(Register, self).__init__()
        loadUi(resource("ui/register.ui"), self)
        self.client = client
        self.parent = parent
        self.button.clicked.connect(self.register_clicked)
        self.show()

    def register_clicked(self):
        name = self.name.text().strip()
        description = self.description.text().strip()
        value = self.value.text().strip().replace(",", ".")
        date = self.date.dateTime().toPyDateTime()

        if not name:
            QMessageBox.critical(self, "Error!", "Item name is required.")
            return

        if not description:
            QMessageBox.critical(self, "Error!", "Item description is required.")
            return

        if not value:
            QMessageBox.critical(self, "Error!", "Miniamal Bid value is required.")
            return
        else:
            value = re.match(r'\d+(?:\.\d{1,2})?$', value)
            if not value:
                QMessageBox.critical(self, "Error!", f"{self.value.text().strip()} is an invalid bid value.")
                return

        if datetime.now() >= date:
            QMessageBox.critical(self, "Error!", "The final auction time must be greater than the current time.")
            return

        try:
            self.client.register_auction(name, description, float(value.string), date)
            self.parent.update_list()
            self.close()
        except:
            QMessageBox.critical(self, "Error!", "Failed to register item. Is the server running?")


app = QApplication(sys.argv)
window = Form()
sys.exit(app.exec())
