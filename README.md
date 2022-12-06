# LIGHTSABERS Getting Started with FRC Robot Code

[![Gradle](https://github.com/wpilibsuite/allwpilib/actions/workflows/gradle.yml/badge.svg?branch=main)](https://github.com/wpilibsuite/allwpilib/actions/workflows/gradle.yml)
[![Java Documentation](https://img.shields.io/badge/documentation-java-orange)](https://github.wpilib.org/allwpilib/docs/development/java/)

Welcome to the LIGHTSABERS Robot Code Repository for the FRC 2022-2023 Season. This Repositiory Contains the steps for installing the Coding Enviroment and loading in last year's FRC Robot Code.

- [Preliminaries with Git](#preliminaries-with-git)
- [Robot Home](#robot-home)
- [Zero to Robot](#zero-to-robot)
- [GitHub Revisited](#GitHub-Revisited)
- [Programming Basics](#programming-basics)
- [Creating a New Robot Project](#creating-a-new-robot-project)
- [3rd Party Libraries](#3rd-party-libraries)
- [Copy Last Years Robot Code](#copy-last-years-robot-code)
- [Git Update](#git-update)


# Preliminaries with Git

First of all, I'm assuming that you are reading this README.md file from the "main" branch on the LIGHTSABERS Web-based GetHub site (i.e. the remote Github repositiory).  This repository should not be cloned from the "main" branch.  The first thing to do is to create a new branch by clicking on the "branch" icon at the top of this repository.  This will bring up a "branch" page and on the right side of this page is a green button called "New Branch".  Click on this button, type "{yourname}Dev" in the Branch Name, makesure that the source is the "main" branch, and then click "Create Branch".  The resulting "Overview" page will then show that there is an Active branch called "<yourname>Dev".  You can click on the "{yourname}Dev" branch and you will see that it has the same files as were in the new branch.
  
At this point, I will assume that you are reading this README.md file from the "{yourname}Dev" branch.  You are now ready to setup your computer to receive your local repository of the FRC_2022-2023 Code.  To avoid having this repository proliferate on your computer, it will be stored in the Public User's workspace.  Create a new storage folder on your Windows computer to act as your local Git repository for the FRC 2022-2023 Code:
  
"{Drive_Letter}:\Users\Public\Documents\GitHub\FRC_2022-2023"

Note: if this location already exists you must cleanup the computer by deleting the GitHub directory.

# Robot Home

In this repository is a folder called "Robot_Code".  All Robot code projects for the 2022-2023 year will be created in this folder.  Each will have it's own folder, which will be named in a manner that provides a basic idea of what the code does.

Last year's robot code will be in the folder "Robot_Code/Last_Year.

# Zero to Robot

Starting with Step 2 of the Zero to Robot instructions (https://docs.wpilib.org/en/stable/docs/zero-to-robot/step-2/wpilib-setup.html) install the WPILib libaries.  When the instructions on this web page have been completed,the following will be installed:
  
Visual Studio Code - The supported IDE for 2019 and later robot code development. The offline installer sets up a separate copy of VS Code for WPILib development, even if you already have VS Code on your machine. This is done because some of the settings that make the WPILib setup work may break existing workflows if you use VS Code for other projects.

C++ Compiler - The toolchains for building C++ code for the roboRIO

Gradle - The specific version of Gradle used for building/deploying C++ or Java robot code

Java JDK/JRE - A specific version of the Java JDK/JRE that is used to build Java robot code and to run any of the Java based Tools (Dashboards, etc.). This exists side by side with any existing JDK installs and does not overwrite the JAVA_HOME variable

WPILib Tools - SmartDashboard, Shuffleboard, RobotBuilder, Outline Viewer, Pathweaver, Glass, SysID

WPILib Dependencies - OpenCV, etc.

VS Code Extensions - WPILib extensions for robot code development in VS Code

Continue to the next page in the instructions and select the "Visual Studio Code Basics" hyperlink.

# GitHub Revisited

Near the top of this repository is a green button called "Code" next to the word "About".  Left click on "Code" and select the "Open with GitHub Desktop" option.  If the GitHub Desktop doesn't exist it will download it and ask you to log-in.  If you don't have an account, create one for free and repeat the process of left clicking on "Code" and logging-in to the GitHub Desktop.  When GitHub Desktop opens, then it will default to looking at the "main" branch.  Switch it to the "{yourname}Dev" branch.

At this point, you can open this repositiory in Visual Studio Code by finding "Open Repository in External Editor" option on GitHub Desktop and selecting the option for "Open in Visual Studio Code".  Now you should open this README.md file in Visual Studio Code. 

# Programming Basics

The "Visual Studio Code Basics" hyperlink redirects to a webpage called "Visual Studio Code Basics and WPILib Extension" (https://docs.wpilib.org/en/stable/docs/software/vscode-overview/vscode-basics.html#visual-studio-code-basics-and-the-wpilib-extension), which is part of the "Programming Basics" Tutorial.

Read this page to get familiar with how to call the WPILib command from the command set list.  Click the "Next" button a the bottom of the page to go to next page where a description of each of the WPILib commands is found.  Click the "Next" button to to to the next page, which starts with a discussion on Robot Program and how a Robot Base Class is selected.  Pay particular attention to the "Timed Robot Template". 


# Creating a New Robot Project
  
Eventually, a section called "Creating a New WPILib Project" is found toward the middle of the the Robot Program Page (https://docs.wpilib.org/en/stable/docs/software/vscode-overview/creating-robot-program.html).  Perform the tasks stated in Visual Studio Code. 

While doing these tasks, a "New Project Creator Window" will pop up.  Select the following on the first row:

Project Type:  Template
Language:      java,
Base:          Timed Robot

Other item on the creator page are filled in as:
Base Folder:  {Drive Letter}:/Users/Public/Documents/GitHub/FRC_2022-2023/Robot_Code 
Use the "Select a new project folder" button to graphically select the Base Folder

Project Name: Last_Year,
Team Number:  3660.

Then click on the "Generate Project" button.

Visual Studio will now have a project created in the FRC_2022-2023 Folder called "Last_Year".

After creating the project in Visual Studio Code, continue reading to the webpage instructions to end of the page and click the "Next" button to continue to "libraries".

# 3rd Party Libraries

At this point in the process, you should be on the 3rd Party Libraries Page of the Web instructions(https://docs.wpilib.org/en/stable/docs/software/vscode-overview/3rd-party-libraries.html). Read down to "Managing VS Code Libraries".  In VS Code, use the "Manage Vendor Libraries > Manage Current Libraries" to show you that:

WPILib-New-Commands

library is already part of your project.  Note: If you libraries are project specific.  If you closed the project, then it must be reopened to show the libraries currently in use.

Pay attention to the "command line" option for installing vendor libraries.  Three-quarters of the way down this page is a list of common vendors and their web pages.

Add the Rev library and the Phenoix Library to your project by:

1. Open a terminal in VS Code from the "View > Terminal" tab.

2. Navigate the directory "<Drive Letter>:\Users\Public\Documents\GitHub\FRC_2022-2023\Robot_Code\Last_Year"

3. Give the following commands at the terminal command prompt:

> ./gradlew vendordep --url=https://software-metadata.revrobotics.com/REVLib.json

>./gradlew vendordep --url=https://maven.ctr-electronics.com/release/com/ctre/phoenix/Phoenix-frc2022-latest.json

Note: these website addresses were valid for the 2022 season.

In VS Code, use the "Manage Vendor Libraries > Manage Current Libraries" to show you 3 libraries are part of the project:

CTRE-Pheonix
RevLib
WPILib-New-Commands

# Copy Last Years Robot Code

To put last years code into the project, open 2 directory folders.  Navigate to a flash drive contains last year' "robot" directory.  Navigate the Project's "robot" directory (<Drive_Letter>:\Users\Public\Documents\GitHub\FRC_2022-2023\Robot_Code\Last_Year\src\main\java\frc\robot).  Each directory will have two files:

Main.java
Robot.java

Replace the Project's "Robot.java" file with Last Year's "Robot.java" file. 

# Git Update

Save any open files in the project.  To to GitHub Desktop and commit the changes by putting "Created Project for Last Year's Robot" in the Summary box (lower left) and clicking on the "Commit" button (bottom left). Push the change to the remote repository by clicking on the "Push Origin" button on the top (right side) of the GitHub Desktop window.
