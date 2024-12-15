# Project Build and Execution Instructions

## Build the Project

To build the project, run the following command in the root directory of the project:

```sh
make
```

Navigate to the Binary Directory
After building the project, navigate to the bin directory:

```sh
cd bin
```

Run the Project
To run the project, execute the following command with the configuration file as an argument:

```sh
./admpor config.txt
```

Configuration File
The config.txt file should be located in the bin directory and contain the following parameters:

```sh
5
100
10
10
10
log.txt
stats.txt
30
```

- Parameter 1: Number of clients
- Parameter 2: Buffer size
- Parameter 3: Number of intermediaries
- Parameter 4: Number of enterprises
- Parameter 5: Maximum operations
- Parameter 6: Log file name
- Parameter 7: Statistics file name
- Parameter 8: Alarm time

## Additional Commands

The program accepts the following commands during execution:

- OP: Perform an operation
- STATUS: Check the status
- STOP: Stop the execution
- HELP: Display help information

Log and Statistics

- Log File: The log file specified in config.txt will contain the log entries.
- Statistics File: The statistics file specified in config.txt will contain the operation statistics.

## Clean Build

To clean the build artifacts, run the following command in the root directory of the project:

```sh
make clean
```