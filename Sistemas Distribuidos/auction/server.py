import threading
import rsa
import serpent
from threading import Timer
from datetime import datetime
from Pyro5.api import *


class Server:
    def __init__(self):
        threading.Thread(target=start_ns_loop).start()
        self.daemon = Daemon()
        self.nameServer = locate_ns()
        self.uri = self.daemon.register(Auction)
        self.nameServer.register("auction", self.uri)
        threading.Thread(target=self.daemon.requestLoop).start()


@behavior(instance_mode="single")
class Auction:
    def __init__(self):
        self.id = 1
        self.clients = []
        self.proxy = {}
        self.items = {}

    @expose
    def login(self, uri: str, name: str, key: rsa.PublicKey) -> bool:
        try:
            for client in self.clients[:]:
                try:
                    self.proxy[client["uri"]]._pyroClaimOwnership()
                    self.proxy[client["uri"]].notify(f"{name} has entered the auction.")
                except:
                    if client in self.clients:
                        self.clients.remove(client)

            self.proxy[uri] = Proxy(uri)
            client = {"uri": uri, "name": name, "key": rsa.PublicKey(key[0], key[1])}

            if client not in self.clients:
                self.clients.append(client)
                return True
            else:
                return False
        except:
            return False

    @expose
    def bid(self, uri: str, id: int, value: float, signature: bytes) -> str:
        client = next((cli for cli in self.clients if cli["uri"] == uri), None)

        try:
            rsa.verify(f"{uri}-{id}".encode(), serpent.tobytes(signature), client["key"])
            item = self.items[id]

            if uri not in item['biders']:
                item['biders'].append(uri)

            if item['active']:
                if value > item['value']:
                    item['value'] = value
                    item['owner'] = uri

                    for bider in item['biders'][:]:
                        if bider != uri:
                            try:
                                self.proxy[bider]._pyroClaimOwnership()
                                self.proxy[bider].notify(f"{client['name']} now has the highest bid on item ({item['id']}): {item['name']} for: R$ {item['value']}.")
                            except:
                                if bider in item['biders']:
                                    item['biders'].remove(bider)

                    return "Bid accepted."
                else:
                    return "Bid refused, value is lower than current highest bid."
            else:
                return "Bid refused, auction is closed for this item."
        except rsa.VerificationError:
            return "Bid refused, invalid signature."
        except KeyError:
            return "Bid refused, item not found."

    @expose
    def list(self) -> list:
        return [item for item in self.items.values() if item["active"]]

    @expose
    def register(self, uri: str, name: str, description: str, value: float, expiration: datetime) -> dict:
        try:
            time = datetime.strptime(expiration, "%Y-%m-%dT%H:%M:%S")
            item = {
                "id": self.id,
                "owner": uri,
                "name": name,
                "description": description,
                "value": value,
                "expiration": time,
                "active": datetime.now() < time,
                "biders": [uri]
            }

            if item["active"]:
                self.items[self.id] = item
                self.id += 1

                Timer((time - datetime.now()).total_seconds(), self.expire, [item]).start()

                for client in self.clients[:]:
                    try:
                        self.proxy[client["uri"]]._pyroClaimOwnership()
                        self.proxy[client["uri"]].notify(f"A new item is up for auction ({item['id']}): {item['name']} - R$ {item['value']}. Expires at: {item['expiration']}.")
                    except:
                        if client in self.clients:
                            self.clients.remove(client)
            else:
                return None

            return item
        except:
            return None

    @expose
    def subscribe(self, uri: str, id: int) -> str:
        try:
            item = self.items[id]

            if item["active"]:
                if uri not in item["biders"]:
                    item["biders"].append(uri)
                    return f"Successfully subscribed to item ({item['id']}): {item['name']}"
                else:
                    return "Subscription failed, you are already subscribed to this item."
            else:
                return "Subscription failed, auction is closed for this item."
        except:
            return "Subscription failed."

    def expire(self, item: dict):
        client = next((cli for cli in self.clients if cli["uri"] == item['owner']), None)
        item['active'] = False

        for bider in item['biders'][:]:
            try:
                self.proxy[bider]._pyroClaimOwnership()
                self.proxy[bider].notify(f"Auction closed for item ({item['id']}): {item['name']}. {client['name']} won with a bid of: R$ {item['value']}.")
            except:
                if bider in item['biders']:
                    item['biders'].remove(bider)


if __name__ == "__main__":
    Server()
