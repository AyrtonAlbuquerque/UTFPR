import threading
import rsa
import sseclient
import requests
from base64 import b64encode
from datetime import datetime


class Client:
    def __init__(self, owner=None):
        self.id = None
        self.host = None
        self.logged = False
        self.owner = owner
        self.session = requests.Session()
        self.session.headers = {"Content-Type": "application/json"}
        self.publicKey, self.privateKey = rsa.newkeys(2048)

    def sse(self):
        headers = {'Accept': 'text/event-stream'}
        response = requests.get(f"{self.host}/api/notification", headers=headers, verify=False, stream=True)
        client = sseclient.SSEClient(response)

        for event in client.events():
            if self.id == int(event.id):
                self.notify(event.data)

    def notify(self, message: str):
        try:
            if self.owner:
                self.owner.notify(message)
        except:
            pass

    def login(self, name: str, host: str) -> bool:
        self.host = host

        body = {
            "name": name,
            "publicKey": self.publicKey.save_pkcs1().decode('ascii').replace("-----BEGIN RSA PUBLIC KEY-----", "").replace("-----END RSA PUBLIC KEY-----", "")
        }

        try:
            response = self.session.post(f"{self.host}/api/auth", json=body, verify=False)

            if response.status_code == 200:
                response = response.json()
                self.id = response["id"]
                self.session.headers.update({"Authorization": f"{response['type']} {response['token']}"})
                self.logged = True
                threading.Thread(target=self.sse).start()
            else:
                self.logged = False
        except:
            self.logged = False

        return self.logged

    def bid(self, item: int, value: float) -> str:
        body = {
            "clientId": self.id,
            "itemId": item,
            "value": value,
            "signature": b64encode(rsa.sign(f"{item}-{value}".encode('ascii'), self.privateKey, "SHA-256")).decode('ascii')
        }

        try:
            response = self.session.post(f"{self.host}/api/auction/bid", json=body, verify=False)

            if response.status_code == 200:
                return response.content.decode()
            else:
                return "Error making the bid on item"
        except:
            return "Error making the bid on item"

    def subscribe(self, item: int) -> str:
        body = {
            "clientId": self.id,
            "itemId": item
        }

        try:
            response = self.session.patch(f"{self.host}/api/auction/subscribe", json=body, verify=False)

            if response.status_code == 200:
                return response.content.decode()
            else:
                return "Error subscribing to item"
        except:
            return "Error subscribing to item"

    def list_auctions(self) -> list:
        try:
            response = self.session.get(f"{self.host}/api/auction/list", verify=False)

            if response.status_code == 200:
                return response.json()
            else:
                raise Exception("Error listing auctions")
        except Exception as e:
            raise e

    def register_auction(self, name: str, description: str, value: float, expiration: datetime) -> dict:
        body = {
            "clientId": self.id,
            "name": name,
            "description": description,
            "value": value,
            "expiration": expiration.strftime("%Y-%m-%dT%H:%M:%S")
        }

        try:
            response = self.session.post(f"{self.host}/api/auction/register", json=body, verify=False)

            if response.status_code == 200:
                return response.json()
            else:
                raise Exception("Error registering item")
        except:
            raise Exception("Error registering item")
