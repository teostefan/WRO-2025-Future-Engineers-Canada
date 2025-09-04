#!/bin/bash

is_running=false
debounce_time=0.5 # Seconds
last_press_time=0

echo "Script ran at $(date)" >> /home/admin/autostart.log

while true; do
    value=$(raspi-gpio get 17 | grep -o 'level=.' | cut -d= -f2)
    current_time=$(date +%s)

    if [ "$value" -eq 0 ]; then
        # Check if enough time has passed since the last button press, for debouncing.
        if (( $(echo "$current_time - $last_press_time > $debounce_time" | bc -l) )); then
            if [ "$is_running" = false ]; then
                echo "Button press detected; starting main program."
                sudo /home/admin/Documents/WRO-2025-Future-Engineers-Canada/builds/main &
                is_running=true
            else
                echo "Button press detected; stopping main program."
                sudo pkill -f "/home/admin/Documents/WRO-2025-Future-Engineers-Canada/builds/main"
                sudo /home/admin/Documents/WRO-2025-Future-Engineers-Canada/builds/stop &
                raspi-gpio set 23 op dl
                is_running=false
            fi
            last_press_time=$current_time
        fi
    fi

    sleep 0.2 # This should help reduce processing load caused by this script...
done
