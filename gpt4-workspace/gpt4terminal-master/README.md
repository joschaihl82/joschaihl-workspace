
## How to Use gpt4terminal

This guide explains how to install and use gpt4terminal, a tool to interact with GPT-4 through your terminal.

### Prerequisites

* **Dependencies:**
  
```bash
sudo apt install build-essential libcjson-dev libcurl4-openssl-dev
```

### Installation

1. Clone the repository:

```bash
git clone https://github.com/rafaelcchaves/gpt4terminal.git
```

2. Build and Install:

```bash
cd gpt4terminal
sudo make install
```

This will  install `gpt4terminal` in `/usr/bin`. 

* Uninstall (Optional):

```bash
sudo make uninstall
```

4.Using:

Before using the application you have to configure your Api Key from OPENAI. To do this, Run `gpt4terminal`.

Now you can make a query using the follow command:

```
  $ gpt4terminal "Hello World"
```
