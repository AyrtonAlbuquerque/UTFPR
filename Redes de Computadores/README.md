<!-- ------------------------------------------ Logo ------------------------------------------- -->
<div align="center">
    <div><img src="Chat.png" width="256px" height="256px"></div>
</div>

![Version][version.badge]

<!-- ------------------------------------ Table of Content ------------------------------------- -->
<h1>Table of Content</h1>

- [Overview](#overview)
- [Setup](#setup)
  - [Requirements](#requirements)
  - [Installation](#installation)
- [Execution](#execution)
  - [Compiling](#compiling)
  - [Debugging](#debugging)
- [Contact](#contact)
- [Acknowledgments](#acknowledgments)

<!-- ------------------------------------------------------------------------------------------- -->
<!--                                          Sections                                           -->
<!-- ------------------------------------------------------------------------------------------- -->

<!-- ---------------------------------------- Overview ----------------------------------------- -->
<div align="center">

# Overview

</div>

This project consists of a simple client-server chat application where multiple users can communicate with each other by connecting to a server that handles the clients connections.

<h3 align="center">Server</h3>
<div align="center"><img src="Server.png" vspace="10"></div>

The server `IP` and `Name` can not be modified, being set by default by the application. To start the server simply fill the `Port` edit box and press the <img src="Chat/images/Start.png" width="24px" height="24px"> button. If the server starts correctly the status indicator will change from <img src="Chat/images/Off.png" width="24px" height="24px"> to <img src="Chat/images/On.png" width="24px" height="24px">.

Whenever a client connects his/her name will be added to the list containing all connected clients at the left corner. To send a message from the server to a specific or a group of clients simply select the desired clients in the list and send the message. When no clients are selected the server will broadcast the message to all connected clients.

It's also possible to kick a specific or a group of clients by selecting them in the list and clicking the <img src="Chat/images/Kick.png" width="24px" height="24px"> button. The server displays the events and exchanged messages in the text history area, where significant events are aligned in the middle, client messages aligned left and broadcasted messages from server aligned right.

To stop the server simply click the <img src="Chat/images/Stop.png" width="24px" height="24px"> button or close the application.

<h3 align="center">Client</h3>
<div align="center"><img src="Client.png" vspace="10"></div>

To connect to a server with the client application the `Server IP`, `Port` and `Name` edit boxes must be filled. Similar to the server application, to connect to the desired server simply click the <img src="Chat/images/Start.png" width="24px" height="24px"> button. If the connection succeeds the connection status indicator will change from <img src="Chat/images/Off.png" width="24px" height="24px"> to <img src="Chat/images/On.png" width="24px" height="24px"> just like in the server.

After successfully connecting to a server a list of all connected clients besides you will appear in the left corner, where they can be selected to send a message. Similar to the server application, events and messages sent and received are displayed in the text history area, where events are centered, messages you send are aligned on the right and received messages and aligned left with the name of the sender with his picked color.

It's possible to choose the color of a client and server messages by clicking the <img src="Color.png" width="24px" height="24px"> button beside the `Name` edit box. Finnaly to disconnect from the server simply click the <img src="Chat/images/Stop.png" width="24px" height="24px"> button or close the client application.

<!-- ------------------------------------------ Setup ------------------------------------------ -->
<div align="center">

# Setup

</div>

## Requirements

This project was created using [Python 3.11](https://www.python.org/downloads/release/python-3110/) and [QT Designer](https://build-system.fman.io/qt-designer-download) with the [Pycharm IDE](https://www.jetbrains.com/pt-br/pycharm/download/#section=windows).

<h3>Required</h3>

- [pip](https://pypi.org/project/pip/)
- [PyQt5](https://pypi.org/project/PyQt5/)
- [PyQt5Designer](https://pypi.org/project/PyQt5Designer/)

<h3>Optional</h3>

- [auto-py-to-exe](https://pypi.org/project/auto-py-to-exe/)

## Installation

After installing [Python 3.11](https://www.python.org/downloads/release/python-3110/) and cloning the this repo to your computer, you need to create a virtual environment to be able to compile the project, usually named `venv`.

1. Clone the repository

```sh
git clone https://github.com/AyrtonAlbuquerque/UTFPR.git
```

2. Create a virtual environment `venv` inside the `Chat` folder

```sh
python -m venv venv
```

3. Activate the virtual environment or use the [Python Environment Manager](https://marketplace.visualstudio.com/items?itemName=donjayamanne.python-environment-manager) extension if you are using [Visual Studio Code](https://code.visualstudio.com/)

```sh
.\venv\Scripts\Activate.ps1 (PowerShell)
.\venv\Scripts\Activate.bat (Command Line)
```

4. Install the [PyQt5](https://pypi.org/project/PyQt5/) package

```sh
pip install PyQt5
```

5. Install the [PyQt5Designer](https://pypi.org/project/PyQt5Designer/) package

```sh
pip install PyQt5Designer
```

6. Optionally install the [auto-py-to-exe](https://pypi.org/project/auto-py-to-exe/)

```sh
pip install auto-py-to-exe
```

<!-- ---------------------------------------- Execution ---------------------------------------- -->
<div align="center">

# Execution

</div>

## Compiling

To compile and execute the server application simply run the `server.py` python file inside the `Chat` folder.

To compile and execute the client application simply run the `client.py` python file inside the `Chat` folder.

To create the executable files the [auto-py-to-exe](https://pypi.org/project/auto-py-to-exe/) tool is used. Open a terminal window inside the virtual environment and run the command that will open the tool:

```sh
auto-py-to-exe
```

After the tool UI appears select the follwing options

- Script Location: The `server.py` script inside the `Chat` folder.
- Onefile: **One File** option.
- Icon: The `.ico` file inside the `Chat\images` folder.
- Additional Files: Click `Add Folder` and select the `Chat\images` and `Chat\ui` folders.

After that simply click in `Convert .PY to .EXE` and wait unitl the process is finished. An `output` folder should have been created inside the `Chat` folder containing the `server.exe` file.

Repeat the same steps for the `client.py` script to generate the `client.exe` executable.

Alternatively the [pyinstaller](https://pypi.org/project/pyinstaller/) package can be used to generate the executables, since the [auto-py-to-exe](https://pypi.org/project/auto-py-to-exe/) tool uses it under the hood. The follwing command should generate the same results, where `PATH` is the path up to the `Chat` folder.

1. For the server application

```sh
pyinstaller --noconfirm --onefile --windowed --icon "`PATH`/Chat/images/icon.ico" --add-data "`PATH`/Chat/images;images/" --add-data "`PATH`/Chat/ui;ui/"  "`PATH`/Chat/server.py"
```

2. For the client application

```sh
pyinstaller --noconfirm --onefile --windowed --icon "`PATH`/Chat/images/icon.ico" --add-data "`PATH`/Chat/images;images/" --add-data "`PATH`/Chat/ui;ui/"  "`PATH`/Chat/client.py"
```

## Debugging

To debug the server and client applications simply run the IDE you are using in debug mode.

<!-- ----------------------------------------- Contact ----------------------------------------- -->
<div align="center">

# Contact

</div>

- [👦 Ayrton Albuquerque](https://github.com/AyrtonAlbuquerque) - ayrtonsilva@alunos.utfpr.edu.br

<!-- ------------------------------------- Acknowledgments ------------------------------------- -->
<div align="center">

# Acknowledgments

</div>

- [👦 Hermes Irineu Del Monego](https://www.escavador.com/sobre/8105780/hermes-irineu-del-monego) - hmonego@utfpr.edu.br

<!-- --------------------------------------- References ---------------------------------------- -->

[version.badge]: https://img.shields.io/badge/python-3.11-brightgreen
