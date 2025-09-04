#!/bin/bash

is_running=false
debounce_time=0.5 # Seconds
last_press_time=0
gpio_pin=17
max_runtime=180 # Maximum runtime in seconds
start_time=0 # Initialize start time

# Function to start the main program
start_program() {
    echo "Starting main program."
    sudo /home/wro/Documents/WRO-2025-Future-Engineers-Canada/builds/test_esc &
    is_running=true
    start_time=$(date +%s) # Record the start time
}

# Function to stop the main program
stop_program() {
    echo "Stopping main program."
    sudo pkill -f "/home/wro/Documents/WRO-2025-Future-Engineers-Canada/builds/test_esc"
    is_running=false
}

# Trap to handle script termination
trap 'stop_program; exit' SIGINT SIGTERM

# Loop that continues until the program has been running for maximum runtime
while [ "$is_running" = true ] || [ "$(( $(date +%s) - start_time ))" -lt "$max_runtime" ]; do
    current_time=$(date +%s)

    value=$(raspi-gpio get $gpio_pin | grep -o 'level=.' | cut -d= -f2)

    if [ "$value" -eq 0 ]; then
        if (( $(echo "$current_time - $last_press_time > $debounce_time" | bc -l) )); then
            if [ "$is_running" = false ]; then
                start_program
            else
                stop_program
            fi
            last_press_time=$current_time
        fi
    fi

    sleep 0.2
done

# Ensure the program is stopped if the loop exits
if [ "$is_running" = true ]; then
    stop_program
fi
