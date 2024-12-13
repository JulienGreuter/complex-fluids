#  **Installing Ubuntu via WSL, Git, and GitHub Workflow for Beginners**

This tutorial will guide you through:

1. Installing **Ubuntu** using **WSL (Windows Subsystem for Linux)**.
2. Installing **Git** and configuring it.
3. Setting up **SSH** for GitHub.
4. Cloning a GitHub repository.
5. Following a simple **Git workflow** (avoiding direct changes to `main` and using branches and pull requests).

---

##  **Table of Contents**

1. [Enable Virtualization in BIOS](#1-enable-virtualization-in-bios)
2. [Install WSL and Ubuntu](#2-install-wsl-and-ubuntu)
3. [Update Ubuntu](#3-update-ubuntu)
4. [Install Git on Ubuntu](#4-install-git-on-ubuntu)
5. [Set Up SSH for GitHub](#5-set-up-ssh-for-github)
6. [Clone the Repository](#6-clone-the-repository)
7. [Git Workflow](#7-git-workflow)
8. [Summary](#8-summary)

---

## 1️⃣ **Enable Virtualization in BIOS**

1. **Restart your computer**.
2. While booting, press **`DEL`** or **`F2`** (this depends on your motherboard manufacturer) to enter BIOS settings.
3. Look for an option called **"Virtualization," "Intel VT-x," "AMD-V," or "SVM Mode."**
4. **Enable** this option.
5. Save changes and **exit BIOS** (usually by pressing `F10`).

---

## 2️⃣ **Install WSL and Ubuntu**

1. **Open Command Prompt as Administrator**:

   - Click **Start**, type **`cmd`**, right-click **"Command Prompt"**, and select **"Run as Administrator."**

2. **Install WSL** by running this command:

   ```bash
   wsl --install
   ```
3. **Restart your computer**

4. **Launch Ubuntu**

5. **Create User Account**

## 3 **Update Ubuntu**

1. **Open Ubuntu terminal** and run :
   ```bash
   sudo apt-get update
   sudo apt-get upgrade
   ```
2. **Install Git on Ubuntu** by running:
   ```bash
   sudo apt-get install git
   ```
3. **Check installation** with:
   ```bash
   git --version
   ```
4. **Configure Git** with your user information:
   ```bash
   git config --global user.name "Your Name"
   git config --global user.email "your.email@example.qqch"
   ```

## **Set Up SSH for GitHub**

1. **Generate an SSH key**
   ```bash
   ssh-keygen -t ed25519 -C "your.mail@example.qqch"
   ```
2. **Display the Public Key**
   ```bash
   cat ~/.shh/id_ed25519.pub
   ```
3. **Add the SSH Key to GitHub**
Copy the SSH key.
Go to GitHub > Settings > SSH and GPG keys > New SSH key.
Paste the key and click "Add SSH key."

## **Clone the Repository**

1. Go to the repository on GitHub
2. Click *Code* and copy the SSH URL (e.g.,`git@github.com:username/repository.git`).
3. In the Ubuntu terminal, run:
   ```bash
   git clone "the SSH URL"
   ```

## **Git Workflow**

**Avoid Direct Changes to `main`**
Follow this worflow to keep `main`clean:

*Step-by-Step Workfow*
1. *Update* `main`:
   ```bash
   git pull origin main
   ```
2. *Create a New Branch*:
   ```bash
   git checkout -b new_branch_name
   ```
3. *Make Changes*
4. *Stage and Commit Changes*:
   ```bash
   git add .
   git commit -m "describe the commit"
   ```
5. *Push the Branch*:
   ```bash
   git push origin new_branch_name
   ```
6. *Create a Pull Request on GitHub*:
Go to your repository on GitHub.
Click "Compare & pull request."
Add a title and description, then click "Create pull request."



