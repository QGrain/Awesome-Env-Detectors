#!/bin/bash

function determin_docker_runtime()
{
    
    #echo "Start determine docker environment!"
    confidence=0
    
    # echo "1. cat /proc/1/cgroup | grep '/docker/'"
    cgroup_info=$(cat /proc/1/cgroup | grep '/docker/' | grep -v grep | head -1)
    
    if [ -n "$cgroup_info" ]; then
        #echo "(/proc/1/cgroup)========>find docker tag:$cgroup_info"
        printf "[*] %-50s%10s\n" "Docker cgroup check" "[ BAD  ]"
        confidence+=1 
    else  printf "[*] %-50s%10s\n" "Docker cgroup check" "[ GOOD  ]"
    fi
    
    
    # echo "2. cat /proc/1/cgroup | grep '/docker/'"
    cpuset_info=$(cat /proc/1/cpuset | grep '/docker/' | grep -v grep | head -1)
    if [ -n "$cpuset_info" ]; then
        #echo "(/proc/1/cpuset)========>find docker tag:$cpuset_info"
    	printf "[*] %-50s%10s\n" "Docker cpuset check" "[ BAD  ]"
    	confidence+=1 
    else  printf "[*] %-50s%10s\n" "Docker cpuset check" "[ GOOD  ]"
    fi
    
    
    # echo "3. cat /proc/1/sched | head -1"
    sched_info=$(cat /proc/1/sched | head -1)
    x=$(echo $sched_info | grep '1, #threads: 1' | grep -v grep)
    if [ -z "$x" ]; then
        #echo "(/proc/1/sched )========>find differ pid:$sched_info"
    	printf "[*] %-50s%10s\n" "Docker sched check" "[ BAD  ]"
    	confidence+=1
    else  printf "[*] %-50s%10s\n" "Docker sched check" "[ GOOD  ]"
    fi
    
    
    # echo "4. [ -f /.dockerenv ]"
    if [ -f "/.dockerenv" ]; then
        #echo "([ -f /.dockerenv ])====>/.dockerenv exist."
    	printf "[*] %-50s%10s\n" "Docker dockerenv check" "[ BAD  ]"
    	confidence+=1
    else  printf "[*] %-50s%10s\n" "Docker dockerenv check" "[ GOOD  ]"
    fi
    return $confidence
    
}

determin_docker_runtime
