#!/bin/bash

# Check if the user provided a PID (process ID) as the first argument
if [ -z "$1" ]; then
  # If no argument is given, print a usage message
  echo "Usage: $0 <PID>"
  exit 1
fi



PID=$1



# Check if the process exists
if ! ps -p "$PID" > /dev/null; then
  echo "Process with PID $PID does not exist."
  exit 1
fi


# Print a header for the output
echo "Analyzing memory usage for PID: $PID"
echo


# -p $PID: filters the output to show only the process with that PID
# -o pid,vsz,rss,comm: specifies the output columns:
#   pid  = process ID
#   vsz  = virtual memory size (KB)
#   rss  = resident set size (physical memory used, in KB)
#   comm = command name (executable)

echo "[Process Info from ps]"
ps -p $PID -o pid,vsz,rss,comm
echo


# Display a header for the next output section
echo "[/proc/$PID/maps]"

# Display the memory mappings of the process by reading from /proc/<PID>/maps
# This file shows how memory is laid out for the process (segments, permissions, and mappings)
cat /proc/$PID/maps
echo


# Create a filename for saving the memory map, including the PID in the name
OUTPUT_FILE="maps_output_${PID}.txt"

# Write the contents of /proc/<PID>/maps to the file for later reference
cat /proc/$PID/maps > $OUTPUT_FILE

# Inform the user that the memory map has been saved
echo "Memory map has been saved to $OUTPUT_FILE"

# End
echo
echo "Done."
