import threading
import rsa
from datetime import datetime
from Pyro5.api import *


class Client:
    def __init__(self, owner=None):
        self.logged = False
        self.owner = owner
        self.publicKey, self.privateKey = rsa.newkeys(2048)
        self.daemon = Daemon()
        self.auction = Proxy(locate_ns().lookup("auction"))
        self.uri = self.daemon.register(self)
        threading.Thread(target=self.daemon.requestLoop).start()

    @expose
    @oneway
    @callback
    def notify(self, message: str):
        try:
            if self.owner:
                self.owner.notify(message)
        except:
            pass

    def login(self, name: str) -> bool:
        self.logged = self.auction.login(self.uri, name, self.publicKey)
        return self.logged

    def bid(self, id: int, value: float) -> str:
        return self.auction.bid(self.uri, id, value, rsa.sign(f"{self.uri}-{id}".encode(), self.privateKey, "SHA-256"))

    def subscribe(self, id: int) -> str:
        return self.auction.subscribe(self.uri, id)

    def list_auctions(self) -> list:
        return self.auction.list()

    def register_auction(self, name: str, description: str, value: float, expiration: datetime) -> dict:
        return self.auction.register(self.uri, name, description, value, expiration)
