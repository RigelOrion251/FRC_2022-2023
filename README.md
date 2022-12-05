# WPILib Project

[![Gradle](https://github.com/wpilibsuite/allwpilib/actions/workflows/gradle.yml/badge.svg?branch=main)](https://github.com/wpilibsuite/allwpilib/actions/workflows/gradle.yml)
[![Java Documentation](https://img.shields.io/badge/documentation-java-orange)](https://github.wpilib.org/allwpilib/docs/development/java/)

Welcome to the LIGHTSABERS Robot Code Repository for the FRC 2022-2023 Season. This Repositiory Contains the steps for installing the Coding Enviroment and loading in last year's FRC Robot Code.

- [Preliminaries with Git](#wpilib-project)
- [Robot Home](#building-wpilib)
- [Zero to Robot](#building-wpilib)
- [Programing Basics](#building-wpilib)
- [Creating a New Robot Project](#building-wpilib)
- [3rd Party Libraries](#building-wpilib)
- [Copy Last Years Robot Code](#building-wpilib)
- [Git Update](#building-wpilib)
- [Building WPILib](#building-wpilib)
  - [Requirements](#requirements)
  - [Setup](#setup)
  - [Building](#building)
    - [Faster builds](#faster-builds)
    - [Using Development Builds](#using-development-builds)
    - [Custom toolchain location](#custom-toolchain-location)
    - [Formatting/Linting](#formattinglinting)
    - [CMake](#cmake)
  - [Publishing](#publishing)
  - [Structure and Organization](#structure-and-organization)
- [Contributing to WPILib](#contributing-to-wpilib)


## Preliminaries with Git

Before cloning this repository locally, note that is should be placed in the following location on your Windows computer:
"<Drive Letter>:\Users\Public\Documents\GitHub\FRC_2022-2023"
If it was not placed in this location, then: ether remove the local repository and clone it again to the above location, or make note of the new location and adjust the following instructions accordingly.


## Robot Home

In this repository is a folder called "2022-2023_Robot_Code".  All Robot code projects for the 2022-2023 year will be created in this folder.  Each will have it's on folder, which will be named in a manner the provide a basic idea of what the code does.

Last year's robot code will be in the folder "2022-2023_Robot_Code/Last_Year.

## Zero to Robot

Starting with Step 2 of the Zero to Robot instructions (https://docs.wpilib.org/en/stable/docs/zero-to-robot/step-2/wpilib-setup.html) install the WPILib libaries.  When the instructions on this web page have been completed,the following will be installed:
Visual Studio Code - The supported IDE for 2019 and later robot code development. The offline installer sets up a separate copy of VS Code for WPILib development, even if you already have VS Code on your machine. This is done because some of the settings that make the WPILib setup work may break existing workflows if you use VS Code for other projects.

C++ Compiler - The toolchains for building C++ code for the roboRIO

Gradle - The specific version of Gradle used for building/deploying C++ or Java robot code

Java JDK/JRE - A specific version of the Java JDK/JRE that is used to build Java robot code and to run any of the Java based Tools (Dashboards, etc.). This exists side by side with any existing JDK installs and does not overwrite the JAVA_HOME variable

WPILib Tools - SmartDashboard, Shuffleboard, RobotBuilder, Outline Viewer, Pathweaver, Glass, SysID

WPILib Dependencies - OpenCV, etc.

VS Code Extensions - WPILib extensions for robot code development in VS Code

Continue to the next page in the instructions and select the "Visual Studio Code Basics" hyperlink.

## Programing Basics

The "Visual Studio Code Basics" hyperlink redirects to a webpage called "Visual Studio Code Basics and WPILib Extension" (https://docs.wpilib.org/en/stable/docs/software/vscode-overview/vscode-basics.html#visual-studio-code-basics-and-the-wpilib-extension), which is part of the "Programming Basics" Tutorial.

Read this page to get familiar with how to call the WPILib command from the command set list.  Click the "Next" button a the bottom of the page to go to next page where a description of each of the WPILib commands is found.  Click the "Next" button to to to the next page, which starts with a discussion on Robot Program and how a Robot Base Class is selected.  Pay particular attention to the "Timed Robot Template". 


## Creating a New Robot Project
  
Eventually, a section called "Creating a New WPILib Project" is found toward the middle of the the Robot Program Page (https://docs.wpilib.org/en/stable/docs/software/vscode-overview/creating-robot-program.html).  Perform the tasks stated in Visual Studio Code. 

Will doing these tasks a "New Project Creator Window" will pop up.  Select folling on the first row:

Project Type:  Template
Language:      java,
Base:          Timed Robot

Other item on the creator page are filled in as:
Base Folder:  <Drive Letter>/Users/Public/Documents/GitHub/FRC_2022-2023 
Use the "Select a new project folder" button to graphically select the Base Folder

Project Name: Last_Year,
Team Number:  3660.

Then click on the "Generate Project" button.

Visual Studio will now have a project created in the FRC_2022-2023 Folder called "Last_Year".

After creating the project in Visual Studio Code, continue reading to the webpage instructions to end of the page and click the "Next" button to continue to "libraries".

## 3rd Party Libraries

At this point in the process you should be on the 3rd Party Libraries Page of the Web instructions(https://docs.wpilib.org/en/stable/docs/software/vscode-overview/3rd-party-libraries.html). Read down to "Managing VS Code Libraries".  In VS Code, use the "Manage Vendor Libraries > Manage Current Libraries" to show you that:

WPILib-New-Commands

library is already part of your project.  Note: If you libraries are project specific.  If you closed the project, then it must be reopened to show the libraries currently in use.

Pay attention to the "command line" option for installing vendor libraries.  Three-quarters of the way down this page is a list of common vendors and their web pages.

Add the Rev library and the Phenoix Library to your project by:

1. Open a terminal in VS Code from the "View > Terminal" tab.

2. Navigate the directory "<Drive Letter>:\Users\Public\Documents\GitHub\FRC_2022-2023\2022-2023_Robot\FirstRobot"

3. Give the following commands at the terminal command prompt:

> ./gradlew vendordep --url=https://software-metadata.revrobotics.com/REVLib.json

>./gradlew vendordep --url=https://maven.ctr-electronics.com/release/com/ctre/phoenix/Phoenix-frc2022-latest.json

Note: these website addresses were valid for the 2022 season.

In VS Code, use the "Manage Vendor Libraries > Manage Current Libraries" to show you 3 libraries are part of the project:

CTRE-Pheonix
RevLib
WPILib-New-Commands

## Copy Last Years Robot Code

To put last years code into the project, open 2 directory folders.  Navigate to a flash drive contains last year' "robot" directory.  Navigate the Project's "robot" directory (<Drive_Letter>:\Users\Public\Documents\GitHub\FRC_2022-2023\2022-2023_Robot\FirstRobot\src\main\java\frc\robot).  Each directory will have two files:

Main.java
Robot.java

Replace the Project's "Robot.java" file with Last Year's "Robot.java" file. 

## Git Update

Save any open files in the project.  To to GitHub Desktop and commit the changes by putting "Created Project for Last Year's Robot" in the Summary box (lower left) and clicking on the "Commit" button (bottom left). Push the change to the remote repository by clicking on the "Push Origin" button on the top (right side) of the GitHub Desktop window..





  
The WPILib Mission is to enable FIRST Robotics teams to focus on writing game-specific software rather than focusing on hardware details - "raise the floor, don't lower the ceiling". We work to enable teams with limited programming knowledge and/or mentor experience to be as successful as possible, while not hampering the abilities of teams with more advanced programming capabilities. We support Kit of Parts control system components directly in the library. We also strive to keep parity between major features of each language (Java, C++, and NI's LabVIEW), so that teams aren't at a disadvantage for choosing a specific programming language. WPILib is an open source project, licensed under the BSD 3-clause license. You can find a copy of the license [here](LICENSE.md).

# Quick Start

Below is a list of instructions that guide you through cloning, building, publishing and using local allwpilib binaries in a robot project. This quick start is not intended as a replacement for the information further listed in this document.

1. Clone the repository with `git clone https://github.com/wpilibsuite/allwpilib.git`
2. Build the repository with `./gradlew build` or `./gradlew build --build-cache` if you have an internet connection
3. Publish the artifacts locally by running `./gradlew publish`
4. [Update your](DevelopmentBuilds.md) `build.gradle` [to use the artifacts](DevelopmentBuilds.md)

# Building WPILib

Using Gradle makes building WPILib very straightforward. It only has a few dependencies on outside tools, such as the ARM cross compiler for creating roboRIO binaries.

## Requirements

- [JDK 11](https://adoptopenjdk.net/)
    - Note that the JRE is insufficient; the full JDK is required
    - On Ubuntu, run `sudo apt install openjdk-11-jdk`
    - On Windows, install the JDK 11 .msi from the link above
    - On macOS, install the JDK 11 .pkg from the link above
- C++ compiler
    - On Linux, install GCC 11 or greater
    - On Windows, install [Visual Studio Community 2022](https://visualstudio.microsoft.com/vs/community/) and select the C++ programming language during installation (Gradle can't use the build tools for Visual Studio)
    - On macOS, install the Xcode command-line build tools via `xcode-select --install`
- ARM compiler toolchain
    - Run `./gradlew installRoboRioToolchain` after cloning this repository
    - If the WPILib installer was used, this toolchain is already installed
- Raspberry Pi toolchain (optional)
    - Run `./gradlew installArm32Toolchain` after cloning this repository

On macOS ARM, run `softwareupdate --install-rosetta`. This is necessary to be able to use the macOS x86 roboRIO toolchain on ARM.

## Setup

Clone the WPILib repository and follow the instructions above for installing any required tooling.

See the [styleguide README](https://github.com/wpilibsuite/styleguide/blob/main/README.md) for wpiformat setup instructions. We use clang-format 14.

## Building

All build steps are executed using the Gradle wrapper, `gradlew`. Each target that Gradle can build is referred to as a task. The most common Gradle task to use is `build`. This will build all the outputs created by WPILib. To run, open a console and cd into the cloned WPILib directory. Then:

```bash
./gradlew build
```

To build a specific subproject, such as WPILibC, you must access the subproject and run the build task only on that project. Accessing a subproject in Gradle is quite easy. Simply use `:subproject_name:task_name` with the Gradle wrapper. For example, building just WPILibC:

```bash
./gradlew :wpilibc:build
```

The gradlew wrapper only exists in the root of the main project, so be sure to run all commands from there. All of the subprojects have build tasks that can be run. Gradle automatically determines and rebuilds dependencies, so if you make a change in the HAL and then run `./gradlew :wpilibc:build`, the HAL will be rebuilt, then WPILibC.

There are a few tasks other than `build` available. To see them, run the meta-task `tasks`. This will print a list of all available tasks, with a description of each task.

If opening from a fresh clone, generated java dependencies will not exist. Most IDEs will not run the generation tasks, which will cause lots of IDE errors. Manually run `./gradlew compileJava` from a terminal to run all the compile tasks, and then refresh your IDE's configuration (In VS Code open settings.gradle and save).

### Faster builds

`./gradlew build` builds _everything_, which includes debug and release builds for desktop and all installed cross compilers. Many developers don't need or want to build all of this. Therefore, common tasks have shortcuts to only build necessary components for common development and testing tasks.

`./gradlew testDesktopCpp` and `./gradlew testDesktopJava` will build and run the tests for `wpilibc` and `wpilibj` respectively. They will only build the minimum components required to run the tests.

`testDesktopCpp` and `testDesktopJava` tasks also exist for the projects `wpiutil`, `ntcore`, `cscore`, `hal` `wpilibNewCommands` and `cameraserver`. These can be ran with `./gradlew :projectName:task`.

`./gradlew buildDesktopCpp` and `./gradlew buildDesktopJava` will compile `wpilibcExamples` and `wpilibjExamples` respectively. The results can't be ran, but they can compile.

### Build Cache

Run with `--build-cache` on the command-line to use the shared [build cache](https://docs.gradle.org/current/userguide/build_cache.html) artifacts generated by the continuous integration server. Example:

```bash
./gradlew build --build-cache
```

### Using Development Builds

Please read the documentation available [here](DevelopmentBuilds.md)

### Custom toolchain location

If you have installed the FRC Toolchain to a directory other than the default, or if the Toolchain location is not on your System PATH, you can pass the `toolChainPath` property to specify where it is located. Example:

```bash
./gradlew build -PtoolChainPath=some/path/to/frc/toolchain/bin
```

### Formatting/linting

Once a PR has been submitted, formatting can be run in CI by commenting `/format` on the PR. A new commit will be pushed with the formatting changes.

#### wpiformat

wpiformat can be executed anywhere in the repository via `py -3 -m wpiformat -clang 14` on Windows or `python3 -m wpiformat -clang 14` on other platforms.

#### Java Code Quality Tools

The Java code quality tools Checkstyle, PMD, and Spotless can be run via `./gradlew javaFormat`. SpotBugs can be run via the `spotbugsMain`, `spotbugsTest`, and `spotbugsDev` tasks. These tools will all be run automatically by the `build` task. To disable this behavior, pass the `-PskipJavaFormat` flag.

If you only want to run the Java autoformatter, run `./gradlew spotlessApply`.

### CMake

CMake is also supported for building. See [README-CMAKE.md](README-CMAKE.md).

## Publishing

If you are building to test with other dependencies or just want to export the build as a Maven-style dependency, simply run the `publish` task. This task will publish all available packages to ~/releases/maven/development. If you need to publish the project to a different repo, you can specify it with `-Prepo=repo_name`. Valid options are:

- development - The default repo.
- beta - Publishes to ~/releases/maven/beta.
- stable - Publishes to ~/releases/maven/stable.
- release - Publishes to ~/releases/maven/release.

The maven artifacts are described in [MavenArtifacts.md](MavenArtifacts.md)

## Structure and Organization

The main WPILib code you're probably looking for is in WPILibJ and WPILibC. Those directories are split into shared, sim, and athena. Athena contains the WPILib code meant to run on your roboRIO. Sim is WPILib code meant to run on your computer, and shared is code shared between the two. Shared code must be platform-independent, since it will be compiled with both the ARM cross-compiler and whatever desktop compiler you are using (g++, msvc, etc...).

The integration test directories for C++ and Java contain test code that runs on our test-system. When you submit code for review, it is tested by those programs. If you add new functionality you should make sure to write tests for it so we don't break it in the future.

The hal directory contains more C++ code meant to run on the roboRIO. HAL is an acronym for "Hardware Abstraction Layer", and it interfaces with the NI Libraries. The NI Libraries contain the low-level code for controlling devices on your robot. The NI Libraries are found in the [ni-libraries](https://github.com/wpilibsuite/ni-libraries) project.

The upstream_utils directory contains scripts for updating copies of thirdparty code in the repository.

The [styleguide repository](https://github.com/wpilibsuite/styleguide) contains our style guides for C++ and Java code. Anything submitted to the WPILib project needs to follow the code style guides outlined in there. For details about the style, please see the contributors document [here](CONTRIBUTING.md#coding-guidelines).

# Contributing to WPILib

See [CONTRIBUTING.md](CONTRIBUTING.md).
