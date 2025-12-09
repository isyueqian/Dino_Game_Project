# QT Chrome Dinosaur Game for the BeagleBone Black

![console](./public/console.jpeg)

## Running on QT Creator

To run with QT Creator, open the `Dinosaur.pro` file and run the project in the application.

## Compiling and running on BeagleBone Black

To compile this game into an executable to use for embedded platforms like the BeagleBone Black, run `qmake` followed by `make`. You can then move the generated executable to the board to run and play. To use with physical buttons, the project is currently configured to use GPIO26 as the jump button and GPIO46 as the crouch button.
